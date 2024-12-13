#include "mainwindow.h"
#include "windowsManagement.h"
#include "util.h"
#include "ui_mainwindow.h"

std::atomic<bool> running(false);
std::vector<std::jthread> threads;
QString binaryPath;
std::vector<std::string> directories;
std::vector<std::string> permissions;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Remove the maximize button
    this->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    // Set fixed width and height
    this->setFixedSize(611, 463);

    this->setWindowIcon(QIcon(":/resources/NoDelete.png"));

    this->ui->lblByMoval0x1->setText("<a href='https://github.com/moval0x1'>by moval0x1</a>");
    this->ui->lblByMoval0x1->setOpenExternalLinks(true); // Enables opening links in the browser

    // Locate the .ini file in the executable directory
    binaryPath = QCoreApplication::applicationDirPath();
    QString iniFilePath = binaryPath + Util::CONFIG_PATH;
    directories = WindowsManagement::LoadDirectoriesFromIni(this->ui->lblMsg, iniFilePath, "Directories");
    permissions = WindowsManagement::LoadDirectoriesFromIni(this->ui->lblMsg, iniFilePath, "Permissions");

    if(!directories.empty()){
        WindowsManagement::AddItemsToList(this->ui->lstDirectories, directories);
    }

    this->ui->btnStop->setEnabled(false);
    this->ui->lstDirectories->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this->ui->lstDirectories, &QListView::customContextMenuRequested,
            this, [=](const QPoint &pos) { WindowsManagement::ShowContextMenu(this->ui->lstDirectories, pos); });


    Util::setMessage(this->ui->lblMsg, QString::fromStdString("[ NoDelete was started! \\o/]"), "black");

}

MainWindow::~MainWindow()
{
    delete ui;
    wm->RestoreOriginalPermissions(this->ui->lblMsg);
    delete wm;
    wm = nullptr;  // Prevent further access

}

void MainWindow::on_btnRun_clicked()
{
    this->ui->btnStop->setEnabled(true);
    this->ui->btnRun->setEnabled(false);

    wm = new WindowsManagement(running);

    if(this->ui->lstDirectories->model()->rowCount() > 0){

        for (const auto& folder : directories) {
            if (!wm->SaveOriginalPermissions(this->ui->lblMsg, Util::stringToWString(folder))) {
                Util::setMessage(this->ui->lblMsg, QString::fromStdString("Skipping folder due to failure: %1").arg(QString::fromStdString(folder)), "red");
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }

            wm->ClearAllPermissions(this->ui->lblMsg, Util::stringToWString(folder));
            wm->ModifyFoldersPermissions(this->ui->lblMsg, Util::stringToWString(folder));
        }

        if (running.load()) {
            Util::setMessage(this->ui->lblMsg, "Already running!", "red");
            return;
        }

        running = true;  // Set running flag

        for (const auto& dir : directories) {
            std::wstring wDir(dir.begin(), dir.end());

            // Launch threads
            threads.emplace_back([this, wDir]() {
                wm->WatchDirectoryAsync(wDir);
            });
        }

    }
    else{
        Util::setMessage(this->ui->lblMsg, "It would be best if you filled the fields correctly.", "red");
    }
}

void MainWindow::on_btnStop_clicked()
{
    this->ui->btnStop->setEnabled(false);
    this->ui->btnRun->setEnabled(true);

    wm = new WindowsManagement(running);

    if (!running.load()) {
        Util::setMessage(this->ui->lblMsg, "Not running!", "red");
        return;
    }

    running = false;  // Stop monitoring

    // Ensure all threads finish
    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    threads.clear();  // Clear the thread list

    wm->RestoreOriginalPermissions(this->ui->lblMsg);
    delete wm;
    wm = nullptr;  // Prevent further access

    Util::setMessage(this->ui->lblMsg, "Folders unlocked!", "green");
}
