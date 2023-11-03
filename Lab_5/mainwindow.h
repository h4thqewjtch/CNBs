#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "station.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_destStation_activated(int index);

    void on_monitorStation_activated(int index);

private:

    Ui::MainWindow *ui;
    Station st1, st2, st3;
    QString st1_info = "", st2_info = "", st3_info = "";
};

#endif // MAINWINDOW_H
