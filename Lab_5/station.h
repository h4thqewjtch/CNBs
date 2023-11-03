#ifndef STATION_H
#define STATION_H

#include "statbuffer.h"

#include <QDialog>

namespace Ui {
class Station;
}

class Station : public QDialog
{
    Q_OBJECT

public:
    explicit Station(QWidget *parent = nullptr);
    ~Station();
    void open_ports();
    void set_source_dest(QString);
    void choose_buffers();
    QString write_frame();
    QString read_frame();
    bool write_read();
    QStringList get_station_info();

private slots:
    void on_ln_Input_returnPressed();

    void on_ln_Input_textEdited(const QString &arg1);

    void on_destStation_activated(int index);

private:
    Ui::Station *ui;

    static StatBuffer stBuf1, stBuf2, stBuf3;

    static bool begin;

    StatBuffer *curSourceStBuf, *curDestStBuf;

    COMPort sourcePort, destPort;

    QString sourcePortName = "", destPortName = "";

    QString enteredData = "", data = "";
};

#endif // STATION_H
