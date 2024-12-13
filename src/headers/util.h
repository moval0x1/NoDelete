#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QLabel>
#include <QCoreApplication>
#include <mutex>

class Util
{
    // Global mutex for logging
    static std::mutex logMutex;

public:
    static void setMessage(QLabel* label, const QString& message, const QString& color);
    static std::wstring stringToWString(const std::string &str);
    static void LogEvent(const std::wstring& message);
    static std::wstring escapeBackslashes(const std::wstring& path);

    static inline const QString CONFIG_PATH = "/config.ini";
};

#endif // UTIL_H
