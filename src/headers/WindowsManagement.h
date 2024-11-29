#ifndef WINDOWSMANAGEMENT_H
#define WINDOWSMANAGEMENT_H

#include <string> // For std::wstring
#include <QLabel>

void ClearAllPermissions(QLabel* statusLabel, const std::wstring& folderPath);
void ListUsersAndPermissions(QLabel* statusLabel, const std::wstring& folderPath);
void ModifyPermissions(QLabel* statusLabel, const std::wstring& folderPath, const std::wstring& userName);

#endif // WINDOWSMANAGEMENT_H
