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
    port1.close_port();
    port2.close_port();
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
    if(!port1.open_port(portName1, parity))
    {
        QMessageBox::critical(this, "ERROR", portName1+" is not open");
        return;
    }
    ui->lst_State->addItem(portName1+" is opened");
    if(!port2.open_port(portName2, parity))
    {
       QMessageBox::critical(this, "ERROR", portName2+" is not open");
        return;
    }
    ui->lst_State->addItem(portName2+" is opened");
}

void MainWindow::on_comPairs_activated(int index)
{
    if(!portName1.isEmpty())
    {
        port1.close_port();
        ui->lst_State->addItem(portName1+" is closed");
    }
    if(!portName2.isEmpty())
    {
        port2.close_port();
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
}

void MainWindow::on_parity_activated(int index)
{
    switch(index)
    {
    case 0:
    {
        parity = 0;
        break;
    }
    case 1:
    {
        parity = 2;
        break;
    }
    case 2:
    {
        parity = 3;
        break;
    }
    case 3:
    {
        parity = 4;
        break;
    }
    case 4:
    {
        parity = 5;
        break;
    }
    }
}

void MainWindow::on_ln_Input_returnPressed()
{
    //BYTE flag = 'z' + NUMBER;
    BYTE flag = 'o';
    int count = data.size()%NUMBER;
    if(count)
    {
        while(count++<NUMBER)
        {
            data.push_back(QChar(0));
        }
    }
    for(int j = 0;j<data.size();j+=NUMBER)
    {

        BYTE destinationAddress = '0';
        BYTE sourceAddress =  portName1[3].cell();
        BYTE bytes[7];
        for(int i = j, k = 0;i < j+NUMBER; i++, k++)
        {
            bytes[k]=data.toStdString()[i];
        }
        port1.set_packet(flag, destinationAddress, sourceAddress, bytes, 0, 1);
        QString sended = "";
        sended.push_back(QChar::fromLatin1(port1.get_packet().flag));
        sended.push_back(QChar::fromLatin1(port1.get_packet().destinationAddress));
        sended.push_back(QChar::fromLatin1(port1.get_packet().sourceAddress));
        for(int i = 0;i<NUMBER;i++)
        {
            sended.push_back(QChar::fromLatin1(port1.get_packet().data[i]));
        }
        sended.push_back(QChar::fromLatin1(port1.get_packet().FCS));
        ui->lst_State->addItem("Sended shot" + QString::number(j/NUMBER+1) + ": " + sended);
        if(!port1.write_data())
        {
            QMessageBox::critical(this, "ERROR", "Data not record");
            return;
        }
        if(!COMPort::jam.compare(port1.get_maskJam()))
        {
            cout << "\n\njam\n\n";
            if(!port1.get_collision())
            {
                ui->lst_State->addItem("Collision detected");
            }
            if(!port1.get_closeChanel())
            {
                ui->lst_State->addItem("Chanel was closed");
            }
            ui->lst_State->addItem("Delay: " + QString::number(port1.get_delay())+"ms");
            COMPort::jam = "";
        }
        if(!port2.read_data())
        {
            QMessageBox::critical(this, "ERROR", "Data not read");
            return;
        }
        QString str="";
        for(int i = 0;i<NUMBER;i++)
        {
            str.push_back(QChar::fromLatin1(port2.get_packet().data[i]));
        }
        ui->lst_Output->addItem(str);
        QString received = "";
        received.push_back(QChar::fromLatin1(port2.get_packet().flag));
        received.push_back(QChar::fromLatin1(port2.get_packet().destinationAddress));
        received.push_back(QChar::fromLatin1(port2.get_packet().sourceAddress));
        for(int i = 0;i<NUMBER;i++)
        {
            received.push_back(QChar::fromLatin1(port2.get_packet().data[i]));
        }
        received.push_back(QChar::fromLatin1(port2.get_packet().FCS));
        ui->lst_State->addItem("Received shot" + QString::number(j/NUMBER+1) + ": " + received);
    }
}
