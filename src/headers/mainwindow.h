#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    bool LockFolderCLI(const QString &folderPath, const QString &username = "Everyone");

private slots:
    void on_btnOpenFolder_clicked();
    void on_btnLockFolder_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
