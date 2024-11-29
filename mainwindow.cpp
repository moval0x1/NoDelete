#include "mainwindow.h"
#include "WindowsManagement.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QtGui>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->txtUser->setText("Everyone");
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
        // Process the selected directory
        // qDebug() << "Selected directory: " << selectedDir;
        this->ui->txtFolderPath->setText(selectedDir);
    }
}


void MainWindow::on_btnLockFolder_clicked()
{
    QString folderPath = this->ui->txtFolderPath->text();
    QString userName = this->ui->txtUser->text();

    ClearAllPermissions(this->ui->lblMsg, folderPath.toStdWString());
    //ListUsersAndPermissions(this->ui, folderPath.toStdWString());
    //ModifyPermissions(this->ui, folderPath.toStdWString(), userName.toStdWString());
}

