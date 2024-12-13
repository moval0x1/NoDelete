#include "util.h"

void Util::setMessage(QLabel* label, const QString& message, const QString& color) {
    label->setText(message);
    label->setStyleSheet(QString("QLabel { color : %1; }").arg(color));
}

std::wstring Util::stringToWString(const std::string &str)
{
    return std::wstring(str.begin(), str.end());
}
