#include "WindowsManagement.h"
#include "mainwindow.h"         // Required for accessing MainWindow's members

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
        lblMsg->setText("Error getting security info: " + QString::number(result));
        lblMsg->setStyleSheet("QLabel { color : red; }");
        return;
    }

    if (result != ERROR_SUCCESS) {
        lblMsg->setText("Error getting security info: " + QString::number(result));
        lblMsg->setStyleSheet("QLabel { color : red; }");
        return;
    }

    // Set an empty DACL
    result = WindowsManagement::SetCustomDACL(folderPath);
    if (result != ERROR_SUCCESS) {
        lblMsg->setText("Error clearing all permissions: " + QString::number(result));
        lblMsg->setStyleSheet("QLabel { color : red; }");
    } else {
        lblMsg->setText("Successfully removed all inherited permissions.");
        lblMsg->setStyleSheet("QLabel { color : green; }");
    }

    if (result != ERROR_SUCCESS) {
        lblMsg->setText("Error clearing all permissions: " + QString::number(result));
        lblMsg->setStyleSheet("QLabel { color : red; }");
    }
    else {
        lblMsg->setText("Successfully removed all inherited permissions.");
        lblMsg->setStyleSheet("QLabel { color : green; }");
    }

    // Clean up
    if (pSD) LocalFree(pSD);
}

void WindowsManagement::ListUsersAndPermissions(QLabel* lblMsg, const std::wstring& folderPath) {
    PACL pDACL = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;

    // Get the current security descriptor
    DWORD result = WindowsManagement::GetSecurityDescriptor(folderPath, pSD, pDACL);
    if (result != ERROR_SUCCESS) {
        lblMsg->setText("Error getting security info: " + QString::number(result));
        lblMsg->setStyleSheet("QLabel { color : red; }");
        return;
    }

    if (result != ERROR_SUCCESS) {
        lblMsg->setText("Error getting security info: " + QString::number(result));
        lblMsg->setStyleSheet("QLabel { color : red; }");
        return;
    }

    if (!pDACL) {
        lblMsg->setText("No DACL found for the folder.");
        lblMsg->setStyleSheet("QLabel { color : red; }");
        if (pSD) LocalFree(pSD);
        return;
    }

    // Iterate through the ACL
    for (DWORD i = 0; i < pDACL->AceCount; ++i) {
        LPVOID pAce = nullptr;
        if (GetAce(pDACL, i, &pAce)) {
            ACE_HEADER* pHeader = (ACE_HEADER*)pAce;
            if (pHeader->AceType == ACCESS_ALLOWED_ACE_TYPE) {
                ACCESS_ALLOWED_ACE* pAllowedAce = (ACCESS_ALLOWED_ACE*)pAce;
                SID* sid = (SID*)&pAllowedAce->SidStart;
                LPWSTR sidName = nullptr, sidDomain = nullptr;
                DWORD nameSize = 0, domainSize = 0;
                SID_NAME_USE sidType;

                // First call to get size
                LookupAccountSidW(nullptr, sid, nullptr, &nameSize, nullptr, &domainSize, &sidType);
                sidName = new WCHAR[nameSize];
                sidDomain = new WCHAR[domainSize];

                // Second call to get actual data
                if (LookupAccountSidW(nullptr, sid, sidName, &nameSize, sidDomain, &domainSize, &sidType)) {
                    //std::wcout << L"User: " << sidDomain << L"\\" << sidName << std::endl;
                    // TODO: Add a list to list the users from the folder and let the user
                    // choose between a default user or change the permissions to a specific user
                }
                else {
                    lblMsg->setText("Error looking up SID: " + QString::number(GetLastError()));
                    lblMsg->setStyleSheet("QLabel { color : red; }");
                }

                delete[] sidName;
                delete[] sidDomain;
            }
        }
    }

    if (pSD) LocalFree(pSD);
}

void WindowsManagement::ModifyPermissions(QLabel* lblMsg, const std::wstring& folderPath, const std::wstring& userName) {
    EXPLICIT_ACCESSW ea = { 0 };
    PACL pOldDACL = nullptr, pNewDACL = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;

    // Get the current security descriptor
    DWORD result = WindowsManagement::GetSecurityDescriptor(folderPath, pSD, pOldDACL);
    if (result != ERROR_SUCCESS) {
        lblMsg->setText("Error getting security info: " + QString::number(result));
        lblMsg->setStyleSheet("QLabel { color : red; }");
        return;
    }

    if (result != ERROR_SUCCESS) {
        lblMsg->setText("Error getting security info: " + QString::number(result));
        lblMsg->setStyleSheet("QLabel { color : red; }");
        return;
    }

    // Set up EXPLICIT_ACCESS to deny delete permissions
    ea.grfAccessPermissions = GENERIC_EXECUTE | GENERIC_WRITE | GENERIC_READ;
    ea.grfAccessMode = GRANT_ACCESS;
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
    ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
    ea.Trustee.ptstrName = const_cast<LPWSTR>(userName.c_str());

    // Modify ACL
    result = SetEntriesInAclW(1, &ea, pOldDACL, &pNewDACL);
    if (result != ERROR_SUCCESS) {
        lblMsg->setText("Error setting ACL entries: " + QString::number(result));
        if (pSD) LocalFree(pSD);
        return;
    }

    // Set a new DACL
    result = WindowsManagement::SetCustomDACL(folderPath, pNewDACL);
    if (result != ERROR_SUCCESS) {
        lblMsg->setText("Error setting security info: " + QString::number(result));
        lblMsg->setStyleSheet("QLabel { color : red; }");
    } else {
        lblMsg->setText("The folder was successfully locked!");
        lblMsg->setStyleSheet("QLabel { color : green; }");
    }

    if (pNewDACL) LocalFree(pNewDACL);
    if (pSD) LocalFree(pSD);
}
