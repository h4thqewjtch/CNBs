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

    static QString jam;

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

    packet get_packet() const;

    void byte_staffing(bool);

    uint16_t calculate_CRC_16(BYTE*);

    bool correction();

    int get_collision() const;

    int get_closeChanel() const;

    int get_delay() const;

    QString get_maskJam() const;

private:

    HANDLE com;
    DCB dcb;
    packet pack;
    int collision = 0;
    int closeChanel = 0;
    int delay = 0;
    const QString maskJam = "11111111 11111111 11111111 11111111";

};


#endif // COMPORT_H
