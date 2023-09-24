#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
            void on_exec_button_clicked();
    void setupTable(std::vector<std::vector<int>> content);
    void displayTable(std::vector<std::vector<int>> content);
    void clearTable();
    void wipeOutTable();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QStringList horizontalHeader;
    QStringList verticalHeader;
    bool table_set;
};
#endif // MAINWINDOW_H
