#include "com.h"

#include <QtSerialPort/QSerialPort>

void COM::set_Name1(QString name1)
{
    this->name1 = name1;
}

void COM::set_Name2(QString name2)
{
    this->name2 = name2;
}

QString COM::get_Name1()
{
    return name1;
}

QString COM::get_Name2()
{
    return name2;
}

int COM::Init()
{



    cout << name1.toStdString().c_str() <<endl<<endl;
    com1 = CreateFileA(name1.toStdString().c_str(), GENERIC_READ | GENERIC_WRITE,
                       0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (com1 == INVALID_HANDLE_VALUE) { return GetLastError(); }

    memset(&dcb1,0, sizeof(DCB));
    dcb1.DCBlength = sizeof(DCB);

    if (!GetCommState(com1, &dcb1)) { return GetLastError(); }

    dcb1.BaudRate = baudRate;
    dcb1.Parity = NOPARITY;
    dcb1.ByteSize = 8;
    dcb1.StopBits = ONESTOPBIT;

    if (!SetCommState(com1, &dcb1)) { return GetLastError(); }

    cout << name2.toStdString().c_str() <<endl<<endl;
    com2 = CreateFileA(name2.toStdString().c_str(), GENERIC_READ | GENERIC_WRITE,
                       0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (com2 == INVALID_HANDLE_VALUE) { return GetLastError(); }

    memset(&dcb2, 0, sizeof(DCB));
    dcb2.DCBlength = sizeof(DCB);

    if (!GetCommState(com2, &dcb2)) { return GetLastError(); }

    dcb2.BaudRate = baudRate;
    dcb2.Parity = NOPARITY;
    dcb2.ByteSize = 8;
    dcb2.StopBits = ONESTOPBIT;

    if (!SetCommState(com2, &dcb2)) { return GetLastError(); }
}

int COM::Write(const char* data, int datasize)
{
    if (com1 == 0) { return GetLastError(); }
    DWORD feedback;
    if (!WriteFile(com1, data, datasize, &feedback, 0) ||
        feedback != datasize) {
        CloseHandle(com1);
        com1 = 0;
        return GetLastError();
    }
}

int COM::Read(char* data, int datasize) {
    DWORD feedback = 0;
    int attempts = 3; //количество запросов на чтение
    while (datasize && attempts) {
        if (attempts) attempts--;
        if (!ReadFile(com2, data, datasize, &feedback, NULL))
        {
            CloseHandle(com2);
            com2 = 0;
            return GetLastError();
        }
        datasize -= feedback;
        data += feedback;
    }
}

void COM::Disconnect()
{
    if (com1 != 0) {
        CloseHandle(com1);
        com1 = 0;
    }
    if (com2 != 0) {
        CloseHandle(com2);
        com2 = 0;
    }
}

void COM::SetBaudRate(int baudRate)
{
    this->baudRate = baudRate;
}
