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
    while(count!=sizeof(p))
    {
        if (!ReadFile(com, &p, sizeof(p), &count, NULL))
        {
                CloseHandle(com);
                com = 0;
                cout << "ERROR_ReadFile\n";
                return false;
        }
    }
    set_packet(p.flag,p.destinationAddress, p.sourceAddress, p.data, p.FCS, 0);
    return true;
}

void COMPort::close_port()
{
    if (com != 0) {
        CloseHandle(com);
        com = 0;
    }
}

void COMPort::set_packet(BYTE flag, BYTE destinationAddress, BYTE sourceAddress, BYTE* data, BYTE FCS, bool staff)
{
    pack.flag = flag;
    pack.destinationAddress = destinationAddress;
    pack.sourceAddress = sourceAddress;
    for(int i = 0;i < 7; i++)
    {
        pack.data[i] = data[i];
    }
    pack.FCS = FCS;
    byte_staffing(staff,pack);
}

void COMPort::byte_staffing(bool staff,packet &pack)
{
    if(staff)
    {
        if(pack.destinationAddress==pack.flag)
        {
            pack.destinationAddress = 13;
        }
        if(pack.sourceAddress==pack.flag)
        {
            pack.sourceAddress = 13;
        }
        for(int i = 0;i < 7; i++)
        {
            if(pack.data[i]==pack.flag)
            {
                pack.data[i] = 13;
            }
        }
        if(pack.FCS==pack.flag)
        {
            pack.FCS = 13;
        }
    }
    else
    {
        if(pack.destinationAddress==13)
        {
            pack.destinationAddress = pack.flag;
        }
        if(pack.sourceAddress==13)
        {
            pack.sourceAddress = pack.flag;
        }
        for(int i = 0;i < 7; i++)
        {
            if(pack.data[i]==13)
            {
                pack.data[i] = pack.flag;
            }
        }
        if(pack.FCS==13)
        {
            pack.FCS = pack.flag;
        }
    }
}

COMPort::packet COMPort::get_packet()
{
    return pack;
}
