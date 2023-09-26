#include <QDebug>
#include <QMessageBox>

#include "comport.h"

bool COMPort::open_port(QString portName, BYTE parity)
{
    string str = portName.toStdString();
    com = CreateFileA(str.c_str(), GENERIC_READ | GENERIC_WRITE,
                       0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (com == INVALID_HANDLE_VALUE)
    {
        cout << "INVALID_HANDLE_VALUE_1\n";
        return false;
    }

    memset(&dcb,0, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState(com, &dcb))
    {
        cout << "INVALID_GetCommState\n";
        if (com != 0) {
            CloseHandle(com);
            com = 0;
        }
        return false;
    }

    dcb.BaudRate = 9600;
    dcb.Parity = parity;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(com, &dcb))
    {
        cout << "INVALID_SetCommState\n";
        if (com != 0) {
            CloseHandle(com);
            com = 0;
        }
        return false;
    }
    return true;
}

bool COMPort::write_data()
{
    if (com == 0)
    {
        cout << "ERROR_HANDLE_WRITE\n";
        return false;
    }
    packet p = get_packet();
    DWORD count = 0;
    if (!WriteFile(com, &p, sizeof(p), &count, 0) ||
        count != sizeof(p))
    {
        CloseHandle(com);
        com = 0;
        cout << "ERROR_WriteFile\n";
        if (com != 0)
        {
            CloseHandle(com);
            com = 0;
        }
        return false;
    }
    return true;
}

bool COMPort::read_data()
{
    DWORD count = 0;
    packet p;
    p.packetEnd = 0;
    while(p.packetEnd == 0)
    {
        if (!ReadFile(com, &p, sizeof(p), &count, NULL))
        {
            CloseHandle(com);
            com = 0;
            cout << "ERROR_ReadFile\n";
            return false;
        }
    }
    if(p.destinationAddress == 13)
    {
        p.destinationAddress = p.packetBegin;
    }
    if(p.sourceAddress == 13)
    {
        p.sourceAddress = p.packetBegin;
    }
    for(int i = 0;i < NUMBER; i++)
    {
        if(p.data[i]==p.packetBegin)
        {
            p.data[i] = 13;
        }
    }
    if(p.FCS == 13)
    {
        p.FCS = p.packetBegin;
    }
    set_packet(p.packetEnd,p.destinationAddress, p.sourceAddress, p.data, p.FCS);
    return true;
}

void COMPort::close_port()
{
    if (com != 0) {
        CloseHandle(com);
        com = 0;
    }
}

void COMPort::set_packet(BYTE flag, BYTE destinationAddress, BYTE sourceAddress, BYTE* data, BYTE FCS)
{
    pack.packetBegin = pack.packetEnd = flag;
    pack.destinationAddress = destinationAddress;
    pack.sourceAddress = sourceAddress;
    for(int i = 0; i < NUMBER; i++)
    {
        pack.data[i] = data[i];
    }
    pack.FCS = FCS;
}

COMPort::packet COMPort::get_packet()
{
    return pack;
}
