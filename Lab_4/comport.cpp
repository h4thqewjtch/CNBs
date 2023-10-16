#include <QDebug>
#include <QMessageBox>

#include "comport.h"
#include <time.h>

QString COMPort::jam = "";

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
    if(closeChanel==1)
    {
        closeChanel = 0;
        jam=maskJam;
    }
    else closeChanel++;
    if(collision==3)
    {
        collision = 0;
        jam=maskJam;
    }
    else collision++;
    packet p = get_packet();
    DWORD count = 0;
    if (!jam.compare(maskJam))
    {
        srand(time(NULL));
        delay = 1000 + rand() % 5000;
        Sleep(delay);
    }
    if (!WriteFile(com, &p, sizeof(p), &count, 0) ||
        count != sizeof(p))
    {
        CloseHandle(com);
        cout << "ERROR_WriteFile\n";
        return false;
    }
    return true;
}

bool COMPort::read_data()
{
    DWORD count = 0;
    packet p;
    int attempts = NUMBER;
    while(count!=sizeof(p) && attempts)
    {
        if (!ReadFile(com, &p, sizeof(p), &count, NULL))
        {
            CloseHandle(com);
            cout << "ERROR_ReadFile\n";
            return false;
        }
        attempts--;
    }
    if(!set_packet(p.flag,p.destinationAddress, p.sourceAddress, p.data, p.FCS, 0) || (!attempts && count!=sizeof(p)))
    {
        return false;
    }
    return true;
}

void COMPort::close_port()
{
    if (com != 0) {
        CloseHandle(com);
        com = 0;
    }
}

bool COMPort::set_packet(BYTE flag, BYTE destinationAddress, BYTE sourceAddress, BYTE* data, uint16_t FCS, bool staff)
{
    pack.flag = flag;
    pack.destinationAddress = destinationAddress;
    pack.sourceAddress = sourceAddress;
    for(int i = 0;i < NUMBER; i++)
    {
        pack.data[i] = data[i];
    }
    if(staff)
    {
        byte_staffing(staff);
        pack.FCS = calculate_CRC_16(pack.data);
        //printf("byte %d: %02X \n", NUMBER-1, pack.data[NUMBER-1]);
        pack.data[NUMBER-1] ^= (1 << 4);
        //printf("changed byte %d: %02X \n", NUMBER-1, pack.data[NUMBER-1]);
       // cout <<"Sended pack.FCS: "<< pack.FCS <<"\n";
    }
    else
    {
        pack.FCS = FCS;
        if(pack.FCS==calculate_CRC_16(pack.data) || correction())
        {
            byte_staffing(staff);
            //cout <<"Received pack.FCS: "<< pack.FCS <<"\n\n";
        }
        else
        {
            cout << "ERROR_CRC_16\n";
            return false;
        }
    }
    return true;
}

COMPort::packet COMPort::get_packet() const
{
    return pack;
}

void COMPort::byte_staffing(bool staff)
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
        for(int i = 0;i < NUMBER; i++)
        {
            if(pack.data[i]==pack.flag)
            {
                pack.data[i] = 13;
            }
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
        for(int i = 0;i < NUMBER; i++)
        {
            if(pack.data[i]==13)
            {
                pack.data[i] = pack.flag;
            }
        }
    }
}

uint16_t COMPort::calculate_CRC_16(BYTE* array) {
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < NUMBER; i++) {
        crc ^= array[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}

bool COMPort::correction()
{
    BYTE bytes[NUMBER];
    for(int i = 0; i < NUMBER; i++)
    {
        bytes[i] = pack.data[i];
    }
    for(int i=0;i<NUMBER;i++)
    {
        for(int j = 0; j < 8; j++)
        {
                // Изменение бита в массиве байтов на противоположное состояние
                bytes[i] ^= (1 << j);
                if(pack.FCS==calculate_CRC_16(bytes))
                {
                    pack.data[i] = bytes[i];
                    return true;
                }
                else
                {
                    bytes[i] ^= (1 << j);
                }
        }
    }
    return false;
}

int COMPort::get_collision() const
{
    return collision;
}

int COMPort::get_closeChanel() const
{
    return closeChanel;
}

int COMPort::get_delay() const
{
    return delay;
}

QString COMPort::get_maskJam() const
{
    return maskJam;
}

