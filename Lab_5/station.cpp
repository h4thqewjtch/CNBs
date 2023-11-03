#include "station.h"
#include "ui_station.h"

#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QThread>

#include <time.h>

BYTE destinationAddress = 0;

bool Station::begin = true;
StatBuffer Station::stBuf1 = StatBuffer("COM3");
StatBuffer Station::stBuf2 = StatBuffer("COM5");
StatBuffer Station::stBuf3 = StatBuffer("COM7");

Station::Station(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Station)
{
    ui->setupUi(this);
    setWindowTitle("Station");
    curSourceStBuf = new StatBuffer();
    curDestStBuf = new StatBuffer();
}

Station::~Station()
{
    delete curSourceStBuf;
    delete curDestStBuf;
    delete ui;
}

void Station::open_ports()
{
    if(!sourcePort.open_port(sourcePortName))
    {
        QMessageBox::critical(this, "ERROR", sourcePortName+" is not open");
        return;
    }
    ui->lst_State->addItem(sourcePortName+" is opened");
    if(!destPort.open_port(destPortName))
    {
        QMessageBox::critical(this, "ERROR", destPortName+" is not open");
        sourcePort.close_port();
        return;
    }
    ui->lst_State->addItem(destPortName+" is opened");
}

void Station::set_source_dest(QString sourcePortName)
{
    this->sourcePortName = sourcePortName;
    destPortName = "COM";
    char number = this->sourcePortName[3].cell();
    if(number=='7')
    {
        number = '4';
    }
    else number += 3;
    destPortName += QString(number);
    bool ok;
    int num = sourcePortName.mid(3, 1).toInt(&ok, 10)/2;
    ui->lblSource->setText("Station " + QString::number(num));
    if(num==3)
    {
        ui->lblDest->setText("Station " + QString::number(1));
    }
    else
    {
        ui->lblDest->setText("Station " + QString::number(num + 1));
    }
    ui->lst_Output->clear();
    ui->lst_State->clear();
}

void Station::choose_buffers()
{
    switch(sourcePortName[3].cell())
    {
    case '3':
    {
        curSourceStBuf = &stBuf1;
        break;
    }
    case '5':
    {
        curSourceStBuf = &stBuf2;
        break;
    }
    case '7':
    {
        curSourceStBuf = &stBuf3;
        break;
    }
    }
    switch(destPortName[3].cell())
    {
    case '4':
    {
        curDestStBuf = &stBuf1;
        break;
    }
    case '6':
    {
        curDestStBuf = &stBuf2;
        break;
    }
    case '8':
    {
        curDestStBuf = &stBuf3;
        break;
    }
    }
}

QString Station::write_frame()
{
    if(!sourcePort.set_packet(curSourceStBuf->get_frame(), 1))
    {
        QMessageBox::critical(this, "ERROR", "Packet is not setted");
        return "";
    }
    QString sended = "";
    sended.push_back(QChar(sourcePort.get_packet().flag));
    sended.push_back(QChar(sourcePort.get_packet().priority));
    sended.push_back(QChar(sourcePort.get_packet().destinationAddress));
    sended.push_back(QChar(sourcePort.get_packet().sourceAddress));
    for(int i = 0;i<NUMBER;i++)
    {
        sended.push_back(QChar::fromLatin1(sourcePort.get_packet().data[i]));
    }
    sended.push_back(QString::number(sourcePort.get_packet().FCS));
    if(!sourcePort.write_data())
    {
        QMessageBox::critical(this, "ERROR", "Data not record");
        return "";
    }
    return sended;
}

QString Station::read_frame()
{
    if(!destPort.read_data())
    {
        QMessageBox::critical(this, "ERROR", destPortName + " do not read frame");
        return "";
    }
    curDestStBuf->set_frame(destPort.get_packet().flag,
                           destPort.get_packet().priority,
                           destPort.get_packet().destinationAddress,
                           destPort.get_packet().sourceAddress,
                           destPort.get_packet().FCS);
    QString res_data = "";
    for(int i=0;i<NUMBER;i++)
    {
        res_data.append(QChar(destPort.get_packet().data[i]));
    }
    curDestStBuf->init_frame_data(res_data);
    curDestStBuf->add_receivedData();
    QString received = "";
    received.push_back(QChar(destPort.get_packet().flag));
    received.push_back(QChar(destPort.get_packet().priority));
    received.push_back(QChar(destPort.get_packet().destinationAddress));
    received.push_back(QChar(destPort.get_packet().sourceAddress));
    for(int i = 0;i<NUMBER;i++)
    {
        received.push_back(QChar(destPort.get_packet().data[i]));
    }
    received.push_back(QString::number(destPort.get_packet().FCS));
    return received;
}

bool Station::write_read()
{
    for(int j = 0;j<data.size();j+=NUMBER)
    {
        curSourceStBuf->init_frame_data(data.mid(j, NUMBER));
        QString frame = write_frame();
        if(frame.isEmpty())
        {
            return false;
        }
        ui->lst_State->addItem("Sended frame" + QString::number(j/NUMBER+1) + ": " + frame);
        frame = read_frame();
        if(frame.isEmpty())
        {
            return false;
        }
        QString bytes = "";
        for(int i = 0;i<NUMBER;i++)
        {
            bytes.append(QChar(curDestStBuf->get_frame().data[i]));
        }
        //qDebug() <<"bytes: " << bytes <<"\n";
        ui->lst_Output->addItem(bytes);
        ui->lst_State->addItem("Received frame" + QString::number(j/NUMBER+1) + ": " + frame);
    }
    curSourceStBuf->set_token(false);
    curDestStBuf->set_token(true);
    curSourceStBuf->clear_received_info();
    return true;
}

QStringList Station::get_station_info()
{
    QStringList info = QStringList();
    if(curDestStBuf->get_token())
    {
        info.append("token ");
    }
    if(!curDestStBuf->get_receivedData().isEmpty())
    {
        info.append("frames ");
    }
    if(!curSourceStBuf->get_enteredData().isEmpty())
    {
        info.append("unsended data ");
    }
    if(destPortName[3].cell() == curDestStBuf->get_frame().destinationAddress)
    {
        curDestStBuf->clear_received_info();
    }
    return info;
}

void Station::on_destStation_activated(int index)
{
    switch(index)
    {
    case 0:
    {
        destinationAddress = '4';
        break;
    }
    case 1:
    {
        destinationAddress = '6';
        break;
    }
    case 2:
    {
        destinationAddress = '8';
        break;
    }
    }
}

void Station::on_ln_Input_textEdited(const QString &arg1)
{
    enteredData = arg1;
}

void Station::on_ln_Input_returnPressed()
{
    if(destinationAddress == 0)
    {
        QMessageBox::warning(this, "ERROR", "Destination address is empty");
        enteredData.clear();
        return;
    }
    else if(destinationAddress - 1 == sourcePortName[3].cell())
    {
        QMessageBox::warning(this, "ERROR", "Destination address is wrong");
        enteredData.clear();
        return;
    }
    open_ports();
    ui->lst_Output->clear();
    choose_buffers();
    if(destPortName[3].cell() == curSourceStBuf->get_frame().sourceAddress)
    {
        QMessageBox::critical(this, "ERROR", "There is a bad cycle");
        curSourceStBuf->clear_received_info();
        enteredData.clear();
        sourcePort.close_port();
        destPort.close_port();
    }
    if(!curSourceStBuf->get_receivedData().isEmpty())
    {
        QMessageBox::information(this, "", "There is a receivedData");
        data = curSourceStBuf->get_receivedData();
    }
    else if(!curSourceStBuf->get_enteredData().isEmpty())
    {
        QMessageBox::information(this, "", "There is a bufferedData");
        data = curSourceStBuf->get_enteredData();
        curSourceStBuf->clear_enteredData();
        //qDebug() << "data: " << data<<"\n";
    }
    if(!enteredData.isEmpty())
    {
        QMessageBox::information(this, "", "There is an enteredData");
        int count = enteredData.size()%NUMBER;
        if(count)
        {
            while(count++<NUMBER)
            {
                enteredData.push_back(QChar(0));
            }
        }
        if(data.isEmpty())
        {
            data = enteredData;
        }
        else
        {
            curSourceStBuf->set_enteredData(enteredData);
        }
    }
    if(!curSourceStBuf->get_token())
    {
        if(!begin)
        {
            QMessageBox::warning(this, "ERROR", "Station don't have the token");
            ui->ln_Input->setText(curSourceStBuf->get_enteredData());
            data.clear();
            enteredData.clear();
            sourcePort.close_port();
            destPort.close_port();
            return;
        }
        else
        {
            begin = false;
        }
    }
    if(data.isEmpty())
    {
        QMessageBox::information(this, "", "There is no data");
        curSourceStBuf->set_frame('o',
                                 '0',
                                 0,
                                 curSourceStBuf->get_frame().sourceAddress,
                                 0);
        QString token = write_frame();
        if(token.isEmpty())
        {
            ui->ln_Input->setText(curSourceStBuf->get_enteredData());
            data.clear();
            enteredData.clear();
            sourcePort.close_port();
            destPort.close_port();
            return;
        }
        ui->lst_State->addItem("Sended token: " + QString(token[0]) + QString(token[1]));
        token = read_frame();
        if(token.isEmpty())
        {
            ui->ln_Input->setText(curSourceStBuf->get_enteredData());
            data.clear();
            enteredData.clear();
            sourcePort.close_port();
            destPort.close_port();
            return;
        }
        curSourceStBuf->set_token(false);
        curDestStBuf->set_token(true);
        curDestStBuf->clear_received_info();
        ui->lst_State->addItem("Received token: " + QString(token[0]) + QString(token[1]));
    }
    else
    {
        if(sourcePortName[3].cell() == curSourceStBuf->get_frame().sourceAddress ||
        sourcePortName[3].cell() == curSourceStBuf->get_frame().destinationAddress)
        {
            curSourceStBuf->set_frame('o',
                                      '1',
                                      destinationAddress,
                                      curSourceStBuf->get_frame().sourceAddress,
                                      0);
        }
        write_read();
    }
    ui->ln_Input->setText(curSourceStBuf->get_enteredData());
    data.clear();
    enteredData.clear();
    sourcePort.close_port();
    destPort.close_port();
}
