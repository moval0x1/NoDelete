#include "windowsManagement.h"
#include "mainwindow.h"         // Required for accessing MainWindow's members
#include "util.h"
#include <windows.h>
#include <TlHelp32.h>


std::unordered_map<std::wstring, PACL> WindowsManagement::originalDACLs;

WindowsManagement::WindowsManagement(std::atomic<bool>& runningFlag)
    : running(runningFlag) {
}

QString WindowsManagement::ExpandEnvironmentVariables(const QString &path) {
    QString expandedPath = path;

    // Regex to match %VAR_NAME% patterns
    QRegularExpression regex(R"(%([^%]+)%)");
    QRegularExpressionMatchIterator it = regex.globalMatch(path);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString envVar = match.captured(1);  // Extract variable name
        QString value = qEnvironmentVariable(envVar.toUtf8().constData(), QString(""));  // Get environment variable value

        // Replace the variable in the path
        expandedPath.replace(match.captured(0), value);
    }

    return expandedPath;
}

#include <QSettings>
#include <QFileInfo>
#include <QRegularExpression>
#include <QLabel>
#include <QStringList>
#include <vector>
#include <string>
#include <QDebug>

std::vector<std::string> WindowsManagement::LoadDirectoriesFromIni(QLabel* lblMsg, const QString &filePath, const std::string &sectionName)
{
    std::vector<std::string> lst;

    // Check if the INI file exists
    if (!QFileInfo::exists(filePath)) {
        Util::setMessage(lblMsg, "INI file not found: " + filePath, "red");
        return lst;
    }

    // Define static regular expressions
    static const QRegularExpression dirPattern(R"(^[A-Z]:\\(?:[^\\/:*?"<>|]+\\)*$)");
    static const QRegularExpression filePattern(R"(^[^\\/:*?"<>|]+\.txt$)");

    QSettings settings(filePath, QSettings::IniFormat);
    settings.beginGroup(QString::fromStdString(sectionName));
    QStringList keys = settings.childKeys();

    for (const QString &key : keys) {
        QString value = settings.value(key).toString().trimmed();
        QString expandedValue = WindowsManagement::ExpandEnvironmentVariables(value);

        bool isValid = false;

        // Section-specific validation
        if (sectionName == "Directories" && dirPattern.match(expandedValue).hasMatch()) {
            isValid = true;
        }
        else if (sectionName == "LogFile" && key == "Path" && filePattern.match(expandedValue).hasMatch()) {
            isValid = true;
        }
        else if (sectionName == "Permissions" && key == "Bkp" && filePattern.match(expandedValue).hasMatch()) {
            isValid = true;
        }

        if (isValid) {
            lst.push_back(expandedValue.toStdString());
        } else {
            Util::setMessage(lblMsg, QString("Invalid entry in [%1]: %2=%3")
                                 .arg(QString::fromStdString(sectionName))
                                 .arg(key)
                                 .arg(value), "red");
        }
    }
    settings.endGroup();
    return lst;
}


void  WindowsManagement::AddItemsToList(QListView* lstDirectories, std::vector<std::string> &lst){

    // Initialize the model only once
    QStandardItemModel *model = new QStandardItemModel();

    // Set the model to the list view
    lstDirectories->setModel(model);

    for(const std::string &v : lst){
        // Create a new item and set its text
        QStandardItem *item = new QStandardItem(QIcon(":/resources/folder.png"), QString::fromStdString(v));

        // Add the item to the model
        model->appendRow(item);
    }

}

void WindowsManagement::ShowContextMenu(QListView* lstDirectories, const QPoint &pos)
{
    QModelIndex index = lstDirectories->indexAt(pos);
    if (!index.isValid()) return;

    QMenu contextMenu;
    QAction *openDirAction = contextMenu.addAction("Open Directory");

    // Execute the menu and check the selected action
    QAction *selectedAction = contextMenu.exec(lstDirectories->viewport()->mapToGlobal(pos));
    if (selectedAction == openDirAction) {
        QString itemText = index.data(Qt::DisplayRole).toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(itemText));
    }
}

void WindowsManagement::WatchDirectoryAsync(const std::wstring& directory) {
    std::thread([this, directory]() {
        WatchDirectory(directory);
    }).detach();
}

void WindowsManagement::WatchDirectory(const std::wstring &directory)
{
    HANDLE hDir = CreateFileW(
        directory.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);

    if (hDir == INVALID_HANDLE_VALUE) {
        std::wcerr << L"[ERROR] Failed to open directory: " << directory << std::endl;
        Util::LogEvent(L"[ERROR] Failed to open directory: " + directory);
        return;
    }

    OVERLAPPED overlapped = {0};
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!overlapped.hEvent) {
        std::wcerr << L"[ERROR] Failed to create event for " << directory << std::endl;
        CloseHandle(hDir);
        return;
    }

    const DWORD bufferLength = 2048;
    BYTE buffer[bufferLength];
    DWORD bytesReturned;

    while (running.load()) {
        ResetEvent(overlapped.hEvent);

        if (!ReadDirectoryChangesW(
                hDir,
                buffer,
                bufferLength,
                TRUE,  // Watch subdirectories
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                    FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_LAST_WRITE |
                    FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE,
                NULL,
                &overlapped,
                NULL)) {
            Util::LogEvent(L"[ERROR] ReadDirectoryChangesW failed for " + directory);
            break;
        }

        DWORD waitStatus = WaitForSingleObject(overlapped.hEvent, 500);  // Timeout after 500ms
        if (waitStatus == WAIT_OBJECT_0 && GetOverlappedResult(hDir, &overlapped, &bytesReturned, TRUE)) {
            if (bytesReturned == 0)
                continue;

            FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)buffer;
            do {
                std::wstring fileName(fni->FileName, fni->FileNameLength / sizeof(WCHAR));
                switch (fni->Action) {
                case FILE_ACTION_ADDED:
                    Util::LogEvent(L"[MONITOR] File created in " + directory + L": " + fileName);
                    break;

                case FILE_ACTION_REMOVED:
                    Util::LogEvent(L"[MONITOR] File deleted in " + directory + L": " + fileName);
                    break;

                case FILE_ACTION_MODIFIED:
                    Util::LogEvent(L"[MONITOR] File modified in " + directory + L": " + fileName);
                    break;

                case FILE_ACTION_RENAMED_OLD_NAME:
                    Util::LogEvent(L"[MONITOR] File renamed from " + fileName);
                    break;

                case FILE_ACTION_RENAMED_NEW_NAME:
                    Util::LogEvent(L"[MONITOR] File renamed to " + fileName);
                    break;
                }
                fni = (FILE_NOTIFY_INFORMATION*)((LPBYTE)fni + fni->NextEntryOffset);
            } while (fni->NextEntryOffset != 0);
        } else if (waitStatus == WAIT_TIMEOUT) {
            continue; // Allow periodic running state check
        }
    }

    CloseHandle(overlapped.hEvent);
    CloseHandle(hDir);
}

bool WindowsManagement::SaveOriginalPermissions(QLabel* lblMsg, const std::wstring &folderPath)
{
    PSECURITY_DESCRIPTOR pSD = NULL;
    PACL pDACL = NULL;

    DWORD dwRes = GetNamedSecurityInfoW(folderPath.c_str(),
                                        SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pDACL, NULL, &pSD);

    if (dwRes != ERROR_SUCCESS) {
        Util::setMessage(lblMsg, QString("Failed to get original permissions. - %1")
                             .arg(QString::fromStdWString(folderPath)), "red");
        return false;
    }

    WindowsManagement::originalDACLs[folderPath] = pDACL;  // Save the original DACL
    return true;
}

void WindowsManagement::RestoreOriginalPermissions(QLabel* lblMsg)
{
    for (const auto& [folderPath, originalDACL] : WindowsManagement::originalDACLs) {
        if (originalDACL) {
            DWORD dwRes = SetNamedSecurityInfoW((LPWSTR)folderPath.c_str(),
                                                SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, originalDACL, NULL);

            if (dwRes != ERROR_SUCCESS) {
                Util::setMessage(lblMsg, QString("Failed to restore original permission. - %1")
                                     .arg(QString::fromStdWString(folderPath)), "red");
            } else {
                Util::setMessage(lblMsg, QString("Restored permissions. - %1")
                                     .arg(QString::fromStdWString(folderPath)), "blue");
            }
        }
    }
}

DWORD WindowsManagement::GetSecurityDescriptor(const std::wstring& folderPath, PSECURITY_DESCRIPTOR& pSD, PACL& pOldDACL) {
    return GetNamedSecurityInfoW(
        folderPath.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &pOldDACL,
        nullptr,
        &pSD
        );
}

DWORD WindowsManagement::SetCustomDACL(const std::wstring& folderPath, PACL pNewDACL) {

    DWORD securityInfo = pNewDACL != nullptr ? DACL_SECURITY_INFORMATION : (DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION);

    return SetNamedSecurityInfoW(
        const_cast<LPWSTR>(folderPath.c_str()),
        SE_FILE_OBJECT,
        securityInfo,
        nullptr,
        nullptr,
        pNewDACL, // Pass a nullptr to set an empty DACL
        nullptr
        );
}

void WindowsManagement::ClearAllPermissions(QLabel* lblMsg, const std::wstring& folderPath) {
    PSECURITY_DESCRIPTOR pSD = nullptr;
    PACL pOldDACL = nullptr;
    PACL pNewDACL = nullptr;

    // Get the current security descriptor
    DWORD result = WindowsManagement::GetSecurityDescriptor(folderPath, pSD, pOldDACL);
    if (result != ERROR_SUCCESS) {
        Util::setMessage(lblMsg, QString("Error getting security info: %1")
                             .arg(QString::number(result))
                             .arg(QString::fromStdWString(folderPath)), "red");
        return;
    }

    // Set an empty DACL
    result = WindowsManagement::SetCustomDACL(folderPath);
    if (result != ERROR_SUCCESS) {
        Util::setMessage(lblMsg, QString("Error clearing all permissions: %1")
                             .arg(QString::number(result))
                             .arg(QString::fromStdWString(folderPath)), "red");
    } else {
        Util::setMessage(lblMsg, "Successfully removed all inherited permissions.", "green");
    }

    // Clean up
    if (pSD) LocalFree(pSD);
}

bool WindowsManagement::ModifyFoldersPermissions(QLabel* lblMsg, const std::wstring& folderPath) {
    EXPLICIT_ACCESSW ea = { 0 };
    PACL pOldDACL = nullptr, pNewDACL = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;
    PSID pEveryoneSID = NULL;
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;

    // Create SID for Everyone
    if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSID)) {
        Util::setMessage(lblMsg, QString("Failed to create Everyone SID. - %1")
                             .arg(QString::fromStdWString(folderPath)), "red");
        return false;
    }

    // Get the current security descriptor
    DWORD result = GetNamedSecurityInfoW(folderPath.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
                                         NULL, NULL, &pOldDACL, NULL, &pSD);
    if (result != ERROR_SUCCESS) {
        Util::setMessage(lblMsg, QString("Error getting security info: %1 - %2")
                             .arg(QString::number(result))
                             .arg(QString::fromStdWString(folderPath)), "red");
        FreeSid(pEveryoneSID);
        return false;
    }

    // Set up EXPLICIT_ACCESS to deny delete permissions
    ea.grfAccessPermissions = GENERIC_EXECUTE | GENERIC_WRITE | GENERIC_READ;
    ea.grfAccessMode = GRANT_ACCESS;
    ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea.Trustee.ptstrName = (LPWSTR)pEveryoneSID;

    // Modify ACL
    result = SetEntriesInAclW(1, &ea, pOldDACL, &pNewDACL);
    if (result != ERROR_SUCCESS) {
        Util::setMessage(lblMsg, QString("Error setting ACL entries: %1 - %2")
                             .arg(QString::number(result))
                             .arg(QString::fromStdWString(folderPath)), "red");
        FreeSid(pEveryoneSID);
        LocalFree(pSD);
        return false;
    }

    // Set a new DACL
    result = SetNamedSecurityInfoW((LPWSTR)folderPath.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
                                   NULL, NULL, pNewDACL, NULL);
    if (result != ERROR_SUCCESS) {
        Util::setMessage(lblMsg, QString("Error setting security info: %1 - %2")
                             .arg(QString::number(result))
                             .arg(QString::fromStdWString(folderPath)), "red");
    } else {
        Util::setMessage(lblMsg, "The folders were successfully locked!", "green");
    }

    // Cleanup
    if (pNewDACL) LocalFree(pNewDACL);
    if (pSD) LocalFree(pSD);
    FreeSid(pEveryoneSID);

    return result == ERROR_SUCCESS;
}
