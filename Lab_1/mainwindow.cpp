#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("COM-Ports");
}

MainWindow::~MainWindow()
{
    port1.closePort();
    port2.closePort();
    delete ui;
}

void MainWindow::on_ln_Input_textEdited(const QString &arg1)
{
    data = arg1;
}

QString MainWindow::getData() const
{
    return data;
}

QString MainWindow::getPortName1() const
{
    return portName1;
}

QString MainWindow::getPortName2() const
{
    return portName2;
}

void MainWindow::on_btn_Connect_clicked()
{
    if(!port1.openPort(portName1, parity))
    {
        QMessageBox::critical(this, "ERROR", portName1+" is not open");
        return;
    }
    ui->lst_State->addItem(portName1+" is opened");
    if(!port2.openPort(portName2, parity))
    {
       QMessageBox::critical(this, "ERROR", portName2+" is not open");
        return;
    }
    ui->lst_State->addItem(portName2+" is opened");
}

void MainWindow::on_comboBox_activated(int index)
{
    if(!portName1.isEmpty())
    {
        port1.closePort();
        ui->lst_State->addItem(portName1+" is closed");
    }
    if(!portName2.isEmpty())
    {
        port2.closePort();
        ui->lst_State->addItem(portName2+" is closed");
    }
    switch(index)
    {
    case 0:
    {
        portName1 = "COM3";
        portName2 = "COM8";
        break;
    }
    case 1:
    {
        portName1 = "COM7";
        portName2 = "COM4";
        break;
    }
    }
    //qDebug()<<portName1 <<"\n";
    //qDebug()<<portName2 <<"\n";

}


void MainWindow::on_comboBox_2_activated(int index)
{
    switch(index)
    {
    case 0:
    {
        parity = NOPARITY;
        break;
    }
    case 1:
    {
        parity = ODDPARITY;
        break;
    }
    case 2:
    {
        parity = EVENPARITY;
        break;
    }
    case 3:
    {
        parity = MARKPARITY;
        break;
    }
    case 4:
    {
        parity = SPACEPARITY;
        break;
    }
    }
    //qDebug()<< parity <<"\n";
}


void MainWindow::on_ln_Input_returnPressed()
{
    string str="";
    for(int i=0;i < data.toStdString().size();i++)
    {
        if(!port1.writeData((LPCVOID)data.toStdString()[i]))
        {
            QMessageBox::critical(this, "ERROR", "Data not record");
            return;
        }
        char c = 0;
        if(!port2.readData((LPVOID*)&c))
        {
            QMessageBox::critical(this, "ERROR", "Data not read");
            return;
        }
        str.push_back(c);
    }
    ui->lst_State->addItem("Sended size: " + QString::number(data.size()));
    QString res = QString(str.c_str());
    ui->lst_Output->addItem(res);
    ui->lst_State->addItem("Received size: " + QString::number(res.size()));
    if(data.compare(res))
    {
        QMessageBox::critical(this, "ERROR", "Data to send and data to receive are different");
    }
}

