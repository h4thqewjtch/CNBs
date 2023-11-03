#include "statbuffer.h"

void StatBuffer::init_frame_data(QString data)
{
    for(int i = 0;i < NUMBER; i++)
    {
        frame.data[i]=data.toStdString()[i];
    }
}

void StatBuffer::set_frame(BYTE flag,
                          BYTE priority,
                          BYTE destinationAddress,
                          BYTE sourceAddress,
                          uint16_t FCS)
{
    frame.flag = flag;
    frame.priority = priority;
    frame.destinationAddress = destinationAddress;
    frame.sourceAddress = sourceAddress;
    frame.FCS = FCS;
}

packet StatBuffer::get_frame()
{
    return frame;
}

void StatBuffer::add_receivedData()
{
    for(int i = 0;i < NUMBER; i++)
    {
        receivedData.append(QChar(this->frame.data[i]));
    }
}

QString StatBuffer::get_receivedData()
{
    return receivedData;
}

void StatBuffer::set_enteredData(QString enteredData)
{
    this->enteredData = enteredData;
}

QString StatBuffer::get_enteredData()
{
    return enteredData;
}

void StatBuffer::set_token(bool token)
{
    this->token = token;
}

bool StatBuffer::get_token()
{
    return token;
}

void StatBuffer::clear_received_info()
{
    receivedData.clear();
    frame.priority = '0';
    if(sourceAddress!='7')
    {
        frame.destinationAddress = sourceAddress + 3;
    }
    else  frame.destinationAddress = '4';
    frame.sourceAddress = sourceAddress;
    memset(frame.data, 0, sizeof(frame.data));
    frame.FCS = 0;
}

void StatBuffer::clear_enteredData()
{
    enteredData.clear();
}
