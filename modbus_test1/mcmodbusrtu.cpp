#include "mcmodbusrtu.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "qdebug.h"
#include <QTimer>
#include <iostream>
#include <windows.h>
/*
 * 20181206维护 接受部分CRC完善，数据操作位数完善
 * 备注：
 *      由于mb发送的是16位数字所以接受过来需要将其
 * 先转换为16位数字然后再转换成其他数据类型可以参考
 * 如下使用方法*/
//      typedef union{
//          int mint;
//          float mfloat;
//          unsigned short mshort[2];
//          unsigned char mchar[4];
//      }ShortToOther;
//      ShortToOther ChangeTemp;
//      ChangeTemp.mshort[0]=(Data[i*2+ii*4]<<8)|(Data[i*2+ii*4+1]&0xFF);
//int Read_TempLong = 0;//情况连接状态
//bool serial_enable = 0;//接收数据长度备份
McModbusRTU::McModbusRTU(QObject *parent) : QObject(parent)
{
    Tick=new QTimer(this);
    connect(Tick, SIGNAL(timeout()), this, SLOT(Tick_Server()));
    serial_enable=0;//情况连接状态
    Read_TempLong=0;//接收数据长度备份
}
/*
 * 连接设备
 * Port设备端口号或者设备地址
 * 返回 大于5则连接成功，否则连接失败
*/
int McModbusRTU::LinkUart(QString Port){
    int Err=0;
    myserial = new QSerialPort;
    //设置串口名
    myserial->setPortName(Port);
    //打开串口
    Err += myserial->open(QIODevice::ReadWrite);
    //设置波特率
    Err += myserial->setBaudRate(QSerialPort::Baud9600);
    //设置数据位数
    Err += myserial->setDataBits(QSerialPort::Data8);
     //设置奇偶校验
    Err += myserial->setParity(QSerialPort::NoParity);
    //设置停止位
    Err += myserial->setStopBits(QSerialPort::OneStop);
    //设置流控制
    Err += myserial->setFlowControl(QSerialPort::NoFlowControl);
    //连接信号量
    connect(myserial, &QSerialPort::readyRead, this, &McModbusRTU::Read_Data);

    if(Err>5){
        Tick->start(5);
        serial_enable=1;//标记当前已连接
    }
    return Err;
}
//分时处理消息
void McModbusRTU::Tick_Server(){
    int i,LenTemp;
    unsigned int CRC16temp;
    unsigned short MBCRCreturn;
    if(Read_Temp.length()>0){
        if(Read_Temp.length()>Read_TempLong){
            Read_TempLong=Read_Temp.length();
        }else{
            //CRC校验
//            LenTemp=Read_Temp.length();
//            unsigned char Read_TempP[LenTemp];
//            for(i=0;i<LenTemp;i++)Read_TempP[i]=Read_Temp[i];
//            McMBCRC16(Read_TempP,LenTemp-2,&CRC16temp);
//            MBCRCreturn=(Read_TempP[LenTemp-1]<<8)|((Read_TempP[LenTemp-2])&0xFF);
//            if(CRC16temp!=MBCRCreturn){//CRC失败
//                Read_TempLong=0;
//                Read_Temp.clear();
//                return;
//            }
            //数据接收
            std::cout<<"start get"<<std::endl;
            Data_temp.clear();
            for(i=3;i<Read_Temp.length()-2;i++)Data_temp.append(Read_Temp[i]);
            emit McModbusRTUMessage(Read_Temp[0],Read_Temp[1],Data_temp);
            Read_TempLong=0;
            Read_Temp.clear();
        }
    }
}
void McModbusRTU::Request06(unsigned char id, unsigned int address, unsigned short data){
    if(serial_enable==0)return;
    unsigned char SenData[8];
    unsigned int CrcTemp;
    int i;
    SenData[0]=id;
    SenData[1]=06; //写单个寄存器 调试器按照标准的来写的
    SenData[2]=address>>8;
    SenData[3]=address&0xFF;
    SenData[4]=data>>8;
    SenData[5]=data&0xFF;
    McMBCRC16(SenData,6,&CrcTemp);
    SenData[6]=CrcTemp&0xFF;
    SenData[7]=CrcTemp>>8;
    myserial->write((const char *)SenData,8);
}
void McModbusRTU::Request03(unsigned char id, unsigned int address, unsigned short dLong){
    if(serial_enable==0)return;
    unsigned char SenData[8];
    unsigned int CrcTemp;
    int i;
    SenData[0]=id;
    SenData[1]=03;
    SenData[2]=address>>8;
    SenData[3]=address&0xFF;
    SenData[4]=dLong>>8;
    SenData[5]=dLong&0xFF;
    McMBCRC16(SenData,6,&CrcTemp);
    SenData[6]=CrcTemp&0xFF;
    SenData[7]=CrcTemp>>8;
    myserial->write((const char *)SenData,8);
}
void McModbusRTU::Request16(unsigned char id, unsigned int address, int dLong, unsigned short *data){
    if(serial_enable==0)return;
    unsigned char SenData[9+dLong*2];
    unsigned int CrcTemp;
    int i;

    SenData[0]=id;
    SenData[1]=16;
    SenData[2]=address>>8;
    SenData[3]=address&0xFF;

    SenData[4]=dLong>>8;
    SenData[5]=dLong&0xFF;

    SenData[6]=dLong*2;

    for(i=0;i<dLong;i++){
        SenData[7+2*i]=data[i]>>8;
        SenData[8+2*i]=data[i]&0xFF;
    }

    McMBCRC16(SenData,7+dLong*2,&CrcTemp);
    SenData[7+dLong*2]=CrcTemp&0xFF;
    SenData[8+dLong*2]=CrcTemp>>8;
    myserial->write((const char *)SenData,9+dLong*2);
//    Sleep(40);
}
void McModbusRTU::Request04(unsigned char id, unsigned int address, unsigned short dLong){
    if(serial_enable==0)return;
    unsigned char SenData[8];
    unsigned int CrcTemp;
    int i;
    SenData[0]=id;
    SenData[1]=04;
    SenData[2]=address>>8;
    SenData[3]=address&0xFF;
    SenData[4]=dLong>>8;
    SenData[5]=dLong&0xFF;
    McMBCRC16(SenData,6,&CrcTemp);
    SenData[6]=CrcTemp&0xFF;
    SenData[7]=CrcTemp>>8;
    myserial->write((const char *)SenData,8);
}
//串口数据接收
void McModbusRTU::Read_Data(){
    Read_Temp.append(myserial->readAll());
    if(Read_Temp.length()>1000)Read_Temp.clear();
}
//关闭串口，释放资源
void McModbusRTU::CloseUart(){
    if(serial_enable){
        myserial->clear();
        myserial->close();
        myserial->deleteLater();
        Tick->stop();
    }
}
// CRC MODBUS 效验
// 输入参数: pDataIn: 数据地址
//           iLenIn: 数据长度
// 输出参数: pCRCOut: 2字节校验值
void McModbusRTU::McMBCRC16(unsigned char *pDataIn, int iLenIn, unsigned int *pCRCOut){
    unsigned char ucCRCHi = 0xFF;
    unsigned char ucCRCLo = 0xFF;
    int iIndex;
    while(iLenIn-- )
    {
        iIndex = ucCRCLo ^ *( pDataIn++ );
        ucCRCLo = ( unsigned char )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    *pCRCOut = ( ucCRCHi << 8 | ucCRCLo );
}
