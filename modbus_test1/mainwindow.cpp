#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <iostream>
#include <windows.h>
#include <dos.h>
#include <thread>

int times_count = 0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mmrtu=new McModbusRTU();
    connect(mmrtu,&McModbusRTU::McModbusRTUMessage,this,&MainWindow::McModbusRTUMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::closeEvent(QCloseEvent *ev){ //有错 先注释了
//    linkserver->LinkServerClose();
//}

void MainWindow::McModbusRTUMessage(unsigned char mID, int mFunction, QByteArray Data){
    //若采集数据为普通数据时候
    ShortToOther ChangeTemp;
    //ChangeTemp.mshort[0]=(Data[i*2+ii*4]<<8)|(Data[i*2+ii*4+1]&0xFF);
    //qDebug()<<"bbb" + QString::number(ChangeTemp.mshort[0]);
    //所采集数据是Float数据类型时候
     for(int i=0;i<2;i++){
         ChangeTemp.mshort[i]=(Data[i*2]<<8)|(Data[i*2+1]&0xFF);//前高位后低位
     }
//     DataTemp[ii]=ChangeTemp.mfloat;
//     std::cout<< "0"<<std::endl;
     std::cout<< (Data[0]<<8) <<std::endl;
     std::cout<< (Data[1]&0xFF) <<std::endl;
     std::cout<< (Data[2]<<8) <<std::endl;
     std::cout<< (Data[3]&0xFF) <<std::endl;
}

void MainWindow::on_pushButton_4_clicked()
{
    //
    if(mmrtu->LinkUart(ui->lineEdit_3->text())>5){
        statusBar()->showMessage("连接成功");
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    unsigned short SenDate[120]; //一条最多传递123个数字 即246个字节的数字
    SenDate[0]=5;
    SenDate[1]=6;
    SenDate[2]=7;
    for(int i = 3; i < 120; i++){
        SenDate[i] = i;
    }

//    unsigned short SenDate1[120]; //一条最多传递123个数字 即246个字节的数字
//    SenDate1[0]=1;
//    SenDate1[1]=2;
//    SenDate1[2]=3;
//    for(int i = 3; i < 120; i++){
//        SenDate1[i] = i;
//    }

//    if(times_count == 1){
//        mmrtu->Request16(1,0,120,SenDate);
//        times_count = 0;
//    }else if(times_count == 0){
//        mmrtu->Request16(1,120,120,SenDate);
//        times_count = 1;
//    }
    mmrtu->Request16(1,0,120,SenDate);
//    std::thread* t = new std::thread(mmrtu->Request16,1,0,120,SenDate);
//    t->join();
//    delete t;
//    std::thread first(mmrtu->Request16,1,0,120,SenDate);
//    Sleep(50);
//    mmrtu->Request16(1,120,120,SenDate);
//    mmrtu->Request16(1,240,120,SenDate);
//    mmrtu->Request16(1,360,120,SenDate);
//    mmrtu->Request16(1,480,120,SenDate);
//    mmrtu->Request06(1,1,15);
}

void MainWindow::on_pushButton_clicked()
{

}
