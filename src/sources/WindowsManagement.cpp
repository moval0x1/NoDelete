#include "WindowsManagement.h"
#include "mainwindow.h"         // Required for accessing MainWindow's members
#include <windows.h>
#include <aclapi.h>


void ClearAllPermissions(QLabel* statusLabel, const std::wstring& folderPath) {
    PSECURITY_DESCRIPTOR pSD = nullptr;
    PACL pOldDACL = nullptr;
    PACL pNewDACL = nullptr;

    // Get the current security descriptor
    DWORD result = GetNamedSecurityInfoW(
        folderPath.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &pOldDACL,
        nullptr,
        &pSD);

    if (result != ERROR_SUCCESS) {
        statusLabel->setText("Error getting security info: " + QString::number(result));
        return;
    }

    // Create an empty DACL
    result = SetNamedSecurityInfoW(
        const_cast<LPWSTR>(folderPath.c_str()),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        nullptr, // Pass a nullptr to set an empty DACL
        nullptr);

    if (result != ERROR_SUCCESS) {
        statusLabel->setText("Error clearing all permissions: " + QString::number(result));
    }
    else {
        statusLabel->setText("Successfully removed all inherited permissions.");

    }

    // Clean up
    if (pSD) LocalFree(pSD);
}

void ListUsersAndPermissions(QLabel* statusLabel, const std::wstring& folderPath) {
    PACL pDACL = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;

    // Get the current DACL
    DWORD result = GetNamedSecurityInfoW(
        folderPath.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &pDACL,
        nullptr,
        &pSD);

    if (result != ERROR_SUCCESS) {
        statusLabel->setText("Error getting security info: " + QString::number(result));
        return;
    }

    if (!pDACL) {
        //std::wcerr << L"No DACL found for the folder." << std::endl;
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
                }
                else {
                    //std::wcerr << L"Error looking up SID: " << GetLastError() << std::endl;
                }

                delete[] sidName;
                delete[] sidDomain;
            }
        }
    }

    if (pSD) LocalFree(pSD);
}

void ModifyPermissions(QLabel* statusLabel, const std::wstring& folderPath, const std::wstring& userName) {
    EXPLICIT_ACCESSW ea = { 0 };
    PACL pOldDACL = nullptr, pNewDACL = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;

    // Get current security descriptor
    DWORD result = GetNamedSecurityInfoW(
        folderPath.c_str(),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        &pOldDACL,
        nullptr,
        &pSD);

    if (result != ERROR_SUCCESS) {
        //std::wcerr << L"Error getting security info: " << result << std::endl;
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
        //std::wcerr << L"Error setting ACL entries: " << result << std::endl;
        if (pSD) LocalFree(pSD);
        return;
    }

    // Set the new DACL
    result = SetNamedSecurityInfoW(
        const_cast<LPWSTR>(folderPath.c_str()),
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        nullptr,
        nullptr,
        pNewDACL,
        nullptr);

    if (result != ERROR_SUCCESS) {
        //std::wcerr << L"Error setting security info: " << result << std::endl;
    }

    if (pNewDACL) LocalFree(pNewDACL);
    if (pSD) LocalFree(pSD);
}

