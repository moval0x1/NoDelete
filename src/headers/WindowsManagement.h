#ifndef WINDOWSMANAGEMENT_H
#define WINDOWSMANAGEMENT_H

#include <string> // For std::wstring
#include <QLabel>
#include <windows.h>
#include <aclapi.h>

class WindowsManagement {
public:
    static void ClearAllPermissions(QLabel* lblMsg, const std::wstring& folderPath);
    static void ListUsersAndPermissions(QLabel* lblMsg, const std::wstring& folderPath);
    static void ModifyPermissions(QLabel* lblMsg, const std::wstring& folderPath, const std::wstring& userName);

private:
    static DWORD GetSecurityDescriptor(const std::wstring& folderPath, PSECURITY_DESCRIPTOR& pSD, PACL& pOldDACL);
    static DWORD SetCustomDACL(const std::wstring& folderPath, PACL pNewDACL = nullptr);
};

#endif // WINDOWSMANAGEMENT_H
