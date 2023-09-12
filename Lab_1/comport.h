#ifndef COMPORT_H
#define COMPORT_H

#include <iostream>
#include <windows.h>

#include <QString>
using namespace std;


class COMPort
{
public:

    COMPort() = default;

    bool openPort(QString, BYTE);

    bool writeData(LPCVOID);

    bool readData(LPVOID*);

    void closePort();

private:

    HANDLE com;

    DCB dcb;
};

#endif // COMPORT_H
