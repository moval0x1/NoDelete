#include "util.h"
#include "windowsManagement.h"

// Definition outside the class
std::mutex Util::logMutex;

void Util::LogEvent(const std::wstring& message) {

    QString iniFilePath = QCoreApplication::applicationDirPath() + Util::CONFIG_PATH;
    std::string logFileName = WindowsManagement::LoadDirectoriesFromIni(NULL, iniFilePath, "LogFile")[0];

    std::lock_guard<std::mutex> lock(Util::logMutex);
    std::wofstream logFile(logFileName, std::ios::app);
    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        logFile << std::put_time(std::localtime(&now), L"%Y-%m-%d %H:%M:%S") << L" - " << message << std::endl;
        logFile.close();
    }
}


void Util::setMessage(QLabel* label, const QString& message, const QString& color) {
    label->setText(message);
    label->setStyleSheet(QString("QLabel { color : %1; }").arg(color));
    Util::LogEvent(message.toStdWString());
}

std::wstring Util::stringToWString(const std::string &str)
{
    return std::wstring(str.begin(), str.end());
}

std::wstring Util::escapeBackslashes(const std::wstring& path) {
    std::wstring escapedPath = path;
    size_t pos = 0;
    while ((pos = escapedPath.find(L'\\', pos)) != std::wstring::npos) {
        escapedPath.insert(pos, L"\\");  // Add an extra backslash
        pos += 2;  // Skip past the inserted backslashes
    }
    return escapedPath;
}

