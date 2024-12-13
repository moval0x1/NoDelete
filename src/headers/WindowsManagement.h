#ifndef WINDOWSMANAGEMENT_H
#define WINDOWSMANAGEMENT_H

#include <aclapi.h>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <unordered_map>

#include <QLabel>
#include <QListView>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QString>
#include <QSettings>
#include <QProcessEnvironment>
#include <Qpoint>
#include <QMenu>


class WindowsManagement {  
public:
    static std::unordered_map<std::wstring, PACL> originalDACLs;

    explicit WindowsManagement(std::atomic<bool>& runningFlag);

    static std::vector<std::string> LoadDirectoriesFromIni(QLabel* lblMsg, const QString &filePath, const std::string &sectionName);
    static void AddItemsToList(QListView* lstDirectories, std::vector<std::string> &lst);
    static void ClearAllPermissions(QLabel* lblMsg, const std::wstring& folderPath);
    static bool ModifyPermissions(QLabel* lblMsg, const std::wstring& folderPath);

    void WatchDirectory(const std::wstring& directory);
    static bool SaveOriginalPermissions(QLabel* lblMsg, const std::wstring& folderPath);
    static bool SetNoDeletePermissions(QLabel* lblMsg, const std::wstring& folderPath);
    static void RestoreOriginalPermissions(QLabel* lblMsg);

    static void ShowContextMenu(QListView* lstDirectories, const QPoint &pos);

    std::wstring stringToWString(const std::string& str);

private:
    std::atomic<bool>& running;
    static QString ExpandEnvironmentVariables(const QString &path);
    static void LogEvent(const std::wstring& message);

    static DWORD GetSecurityDescriptor(const std::wstring& folderPath, PSECURITY_DESCRIPTOR& pSD, PACL& pOldDACL);
    static DWORD SetCustomDACL(const std::wstring& folderPath, PACL pNewDACL = nullptr);

};

#endif // WINDOWSMANAGEMENT_H
