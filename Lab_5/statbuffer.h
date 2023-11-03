#ifndef STATBUFFER_H
#define STATBUFFER_H

#include "comport.h"

class StatBuffer
{
public:
    StatBuffer() = default;
    StatBuffer(QString sourcePortName)
    {
        sourceAddress = sourcePortName[3].cell();
        frame.sourceAddress = sourceAddress;
    }
    void init_frame_data(QString);
    void set_frame(BYTE,
                  BYTE,
                  BYTE,
                  BYTE,
                  uint16_t);
    packet get_frame();
    void add_receivedData();
    QString get_receivedData();
    void set_enteredData(QString);
    QString get_enteredData();
    void set_token(bool);
    bool get_token();
    void clear_received_info();
    void clear_enteredData();
private:
    bool token;
    packet frame;
    BYTE sourceAddress;
    QString enteredData = "";
    QString receivedData = "";
};

#endif // STATBUFFER_H
