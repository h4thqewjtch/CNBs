#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "comport.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getPortName2() const;

    QString getPortName1() const;

    QString getData() const;

private slots:

    void on_ln_Input_textEdited(const QString &arg1);

    void on_btn_Connect_clicked();

    void on_comPairs_activated(int index);

    void on_parity_activated(int index);

    void on_ln_Input_returnPressed();

private:

    Ui::MainWindow *ui;

    COMPort port1, port2;

    QString data = "";

    QString portName2;

    QString portName1;

    BYTE parity;
};

#endif // MAINWINDOW_H
