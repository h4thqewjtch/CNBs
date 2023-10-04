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
        BYTE flag;
        BYTE destinationAddress;
        BYTE sourceAddress;
        BYTE data[NUMBER];
        uint16_t FCS;
    };

    COMPort() = default;

    bool open_port(QString, BYTE);

    bool write_data();

    bool read_data();

    void close_port();

    bool set_packet(BYTE, BYTE, BYTE, BYTE*, uint16_t, bool);

    packet get_packet();

    void byte_staffing(bool);

    uint16_t calculate_CRC_16(BYTE*);

    bool correction();

private:

    HANDLE com;
    DCB dcb;
    packet pack;
};



#endif // COMPORT_H
