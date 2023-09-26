#ifndef COM_H
#define COM_H
#include <iostream>
#include <windows.h>

#include <QString>
using namespace std;

class COM
{
private:
    DCB dcb1;
    DCB dcb2;
    QString name1="";
    QString name2="";
    HANDLE com1;
    HANDLE com2;
    int baudRate = 9600;
public :
    COM() = default;
    void set_Name1(QString);
    void set_Name2(QString);
    QString get_Name1();
    QString get_Name2();
    int Init();
    int Write(const char*, int);
    int Read(char*, int);
    void Disconnect();
    void SetBaudRate(int);
};

#endif // COM_H
