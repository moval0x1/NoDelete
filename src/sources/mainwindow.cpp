#include "mainwindow.h"
#include "WindowsManagement.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QtGui>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->txtUser->setText("Everyone");

    // Remove the maximize button
    this->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    // Set fixed width and height
    this->setFixedSize(612, 181);

    this->setWindowIcon(QIcon(":/resources/NoDelete.png"));

    this->ui->lblByMoval0x1->setText("<a href='https://github.com/moval0x1'>by moval0x1</a>");
    this->ui->lblByMoval0x1->setOpenExternalLinks(true); // Enables opening links in the browser

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnOpenFolder_clicked()
{
    QString initialDir = QDir::toNativeSeparators(QDir::currentPath());
    QString selectedDir = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, "Open Directory", initialDir, QFileDialog::ShowDirsOnly));

    if (!selectedDir.isEmpty()) {
        this->ui->txtFolderPath->setText(selectedDir);
    }
}


void MainWindow::on_btnLockFolder_clicked()
{
    if(this->ui->txtFolderPath->text() != "" && this->ui->txtUser->text() != ""){

        QString folderPath = this->ui->txtFolderPath->text();
        QString userName = this->ui->txtUser->text();

        WindowsManagement::ClearAllPermissions(this->ui->lblMsg, folderPath.toStdWString());
        WindowsManagement::ListUsersAndPermissions(this->ui->lblMsg, folderPath.toStdWString());
        WindowsManagement::ModifyPermissions(this->ui->lblMsg, folderPath.toStdWString(), userName.toStdWString());
    }
    else{
        this->ui->lblMsg->setText("It would be best if you filled the fields correctly.");
        this->ui->lblMsg->setStyleSheet("QLabel { color : red; }");
    }
}

bool MainWindow::LockFolderCLI(const QString &folderPath, const QString &userName) {

    if (folderPath.isEmpty()) {
        return false;
    }

    WindowsManagement::ClearAllPermissions(this->ui->lblMsg, folderPath.toStdWString());
    WindowsManagement::ListUsersAndPermissions(this->ui->lblMsg, folderPath.toStdWString());
    WindowsManagement::ModifyPermissions(this->ui->lblMsg, folderPath.toStdWString(), userName.toStdWString());

    return true;
}
