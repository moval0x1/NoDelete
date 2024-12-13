#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtGui>
#include <QStandardItemModel>
#include <QMessageBox>
#include <thread>
#include <atomic>

#include "windowsManagement.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool LockFolderCLI();

private slots:
    void on_btnRun_clicked();
    void on_btnStop_clicked();

private:
    Ui::MainWindow *ui;
    WindowsManagement* wm;
};
#endif // MAINWINDOW_H
