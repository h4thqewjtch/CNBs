#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Token Ring");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_destStation_activated(int index)
{
    QString sourcePortName;
    switch(index)
    {
    case 0:
    {
        sourcePortName = "COM3";
        st1.set_source_dest(sourcePortName);
        st1.exec();
        if(st2_info.contains("token "))
        {
            st2_info.remove("token ");
        }
        if(st2_info.contains("frames "))
        {
            st2_info.remove("frames ");
        }
        QString st_info = "";
        foreach(QString info, st1.get_station_info())
        {
            st_info.append(info);
        }
        if(st_info.contains("unsended data "))
        {
            st1_info = "unsended data ";
            st_info.remove("unsended data ");
        }
        else st1_info.clear();
        st2_info.append(st_info);
        break;
    }
    case 1:
    {
        sourcePortName = "COM5";
        st2.set_source_dest(sourcePortName);
        st2.exec();
        if(st3_info.contains("token "))
        {
            st3_info.remove("token ");
        }
        if(st3_info.contains("frames "))
        {
            st3_info.remove("frames ");
        }
        QString st_info = "";
        foreach(QString info, st2.get_station_info())
        {
            st_info.append(info);
        }
        if(st_info.contains("unsended data "))
        {
            st2_info = "unsended data ";
            st_info.remove("unsended data ");
        }
        else st2_info.clear();
        st3_info.append(st_info);
        break;
    }
    case 2:
    {
        sourcePortName = "COM7";
        st3.set_source_dest(sourcePortName);
        st3.exec();
        if(st1_info.contains("token "))
        {
            st1_info.remove("token ");
        }
        if(st1_info.contains("frames "))
        {
            st1_info.remove("frames ");
        }
        QString st_info = "";
        foreach(QString info, st3.get_station_info())
        {
            st_info.append(info);
        }
        if(st_info.contains("unsended data "))
        {
            st3_info = "unsended data ";
            st_info.remove("unsended data ");
        }
        else st3_info.clear();
        st1_info.append(st_info);
        break;
    }
    }
    ui->infoList->clear();
    ui->infoList->addItem("Station 1: "+st1_info);
    ui->infoList->addItem("Station 2: "+st2_info);
    ui->infoList->addItem("Station 3: "+st3_info);
}

void MainWindow::on_monitorStation_activated(int index)
{
    ui->label->setAlignment(Qt::AlignHCenter);
    switch(index)
    {
    case 0:
    {
        ui->label->setText("Monitor Station 1");
        break;
    }
    case 1:
    {
        ui->label->setText("Monitor Station 2");
        break;
    }
    case 2:
    {
        ui->label->setText("Monitor Station 3");
        break;
    }
    }
}

