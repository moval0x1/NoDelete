#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include "mainwindow.h"

#ifdef _WIN32
#include <windows.h>
#include <cstdio>
#include <iostream>
#endif

void AttachConsoleToCLI() {
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
        freopen("CONOUT$", "w", stdout);  // Redirect stdout to console
        freopen("CONOUT$", "w", stderr); // Redirect stderr to console
    }
#endif
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Command-line parser setup
    QCommandLineParser parser;
    parser.setApplicationDescription(
        "NoDelete - Prevent unauthorized folder deletions.\n"
        "Examples:\n"
        "  NoDelete.exe --cli --path 'FolderPath' --user 'UserName'\n"
        "  NoDelete.exe -h"
        );
    parser.addHelpOption(); // Adds --help and -h options automatically

    // Add options for CLI mode
    QCommandLineOption cliOption("cli", "Run in command-line mode");
    QCommandLineOption pathOption("path", "Full path to the folder to lock", "path");
    QCommandLineOption userOption("user", "Username to associate with the lock (default: 'Everyone')", "user");
    parser.addOption(cliOption);
    parser.addOption(pathOption);
    parser.addOption(userOption);

    // Process the command-line arguments
    parser.process(a);

    if (parser.isSet("help")) {
        AttachConsoleToCLI();
        qInfo() << parser.helpText();
        return 0;
    }

    if (parser.isSet(cliOption)) {
        AttachConsoleToCLI(); // Attach a console for CLI mode

        qInfo() << "\n[+] NoDelete is running in command-line mode.";

        QString folderPath = parser.value(pathOption);
        QString username = parser.value(userOption);

        if (folderPath.isEmpty()) {
            qWarning() << "\tUsage: NoDelete.exe --cli --path 'FolderPath' --user 'UserName'\n"
                          "\tIf no user was passed, NoDelete assumes 'Everyone' as the default user.";
            return 1;
        }

        // Create an instance of MainWindow and call the function
        MainWindow mainWindow;
        bool result = mainWindow.LockFolderCLI(folderPath, username);

        if (result) {
            qInfo() << "\t[+] Folder" << folderPath << "was successfully locked!";
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
