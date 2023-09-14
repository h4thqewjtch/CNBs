#include "comport.h"

#include <QDebug>
#include <QMessageBox>

bool COMPort::openPort(QString portName, BYTE parity)
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
    dcb.BaudRate = CBR_9600;
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

bool COMPort::writeData(LPCVOID data)
{
    if (com == 0)
    {
        cout << "ERROR_HANDLE_WRITE\n";
        return false;
    }
    int datasize = sizeof(data);
    DWORD feedback;
    if (!WriteFile(com, &data, datasize, &feedback, 0) ||
        feedback != datasize) {
        CloseHandle(com);
        com = 0;
        cout << "ERROR_WriteFile\n";
        if (com != 0) {
            CloseHandle(com);
            com = 0;
        }
        return false;
    }
    return true;
}

bool COMPort::readData(LPVOID *data)
{
    DWORD feedback = 0;
    int datasize = sizeof(data);
    int attempts = 5;
    while (datasize && attempts) {
        if (!ReadFile(com, data, datasize, &feedback, NULL))
        {
            CloseHandle(com);
            com = 0;
            cout << "ERROR_ReadFile\n";
            attempts--;
        }
        datasize -= feedback;
        data += feedback;
    }
    if(!attempts && datasize)
    {
        return false;
    }
    else return true;
}

void COMPort::closePort()
{
    if (com != 0) {
        CloseHandle(com);
        com = 0;
    }
}
