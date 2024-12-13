#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QLabel>

class Util
{
public:
    static void setMessage(QLabel* label, const QString& message, const QString& color);
    static std::wstring stringToWString(const std::string &str);
};

#endif // UTIL_H
