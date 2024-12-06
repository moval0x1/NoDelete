#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Command-line parser setup
    QCommandLineParser parser;
    parser.setApplicationDescription("NoDelete with GUI and CLI modes");
    parser.addHelpOption();

    // Add options for CLI mode
    QCommandLineOption cliOption("cli", "Run in command-line mode");
    QCommandLineOption pathOption("path", "Full path to the lockfolder", "path");
    QCommandLineOption userOption("user", "Username", "user");
    parser.addOption(cliOption);
    parser.addOption(pathOption);
    parser.addOption(userOption);

    // Process the command-line arguments
    parser.process(a);

    if (parser.isSet(cliOption)) {
        // CLI mode logic
        qDebug() << "[+] NoDelete is running in command-line mode.";

        QString folderPath = parser.value(pathOption);
        QString username = parser.value(userOption);

        if (folderPath.isEmpty()) {
            qWarning() << "\tUsage: --path 'FoderPath' --user 'UserName'\n\tIf no user was passed, NoDelete assumes 'Everyone' as the default user.";
            return 1;
        }

        // Create an instance of MainWindow and call the function
        MainWindow mainWindow;
        bool result = mainWindow.LockFolderCLI(folderPath, username);

        if (result) {
            qDebug() << "\t[+] Folder " << folderPath << " was successfully locked!";
            return 0;
        } else {
            qWarning() << "\t[-] Failed to lock the folder" << folderPath;
            return 1;
        }
    }

    // GUI mode logic


    MainWindow w;
    w.show();
    return a.exec();
}
