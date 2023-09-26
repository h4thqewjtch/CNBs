#ifndef COMPORT_H
#define COMPORT_H

#include <iostream>
#include <windows.h>
#include <QString>
#define NUMBER 7
using namespace std;


class COMPort
{
public:

    struct packet
    {
        BYTE packetBegin;
        BYTE destinationAddress;
        BYTE sourceAddress;
        BYTE data[NUMBER];
        BYTE FCS;
        BYTE packetEnd;
    };

    COMPort() = default;

    bool open_port(QString, BYTE);

    bool write_data();

    bool read_data();

    void close_port();

    void set_packet(BYTE, BYTE, BYTE, BYTE*, BYTE);

    packet get_packet();

private:

    HANDLE com;
    DCB dcb;
    packet pack;
};



#endif // COMPORT_H
