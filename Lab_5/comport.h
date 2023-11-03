#ifndef COMPORT_H
#define COMPORT_H

#include <iostream>
#include <windows.h>
#include <QString>
#define NUMBER 7
using namespace std;

struct packet
{
    BYTE flag;
    BYTE priority;
    BYTE destinationAddress;
    BYTE sourceAddress;
    BYTE data[NUMBER];
    uint16_t FCS;
};

class COMPort
{
public:

    COMPort() = default;

    bool open_port(QString);

    bool write_data();

    bool read_data();

    void close_port();

    bool set_packet(packet,
                    bool);

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
