#include "serialcomm.h"
#include<QDebug>
SerialComm::SerialComm(QObject *parent) :
    QObject(parent),
    serialPort_(nullptr)
{

}

SerialComm::~SerialComm()
{
    if(serialPort_){
        delete  serialPort_;
        serialPort_=nullptr;
    }
}

void SerialComm::onSetName(const QString &name)
{
    config_.name_=name;
    //qDebug()<<"SerialComm::onSetName:"<<name;
    //serialPort_->setPortName(name);
}

void SerialComm::onSetBaudRate(qint32 v)
{
    config_.baudRate_=v;
    //qDebug()<<"SerialComm::onSetBaudRate:"<<v;
    //serialPort_->setBaudRate(v);
}

void SerialComm::onWrite(const QByteArray &data)
{
    if(serialPort_ && serialPort_->isOpen()){
       serialPort_->write(data);
       //qDebug()<<"写了:"<<data;
    }
}

void SerialComm::onOpen()
{
    if(!serialPort_ || serialPort_->isOpen())return;
    //打开前 初始化一次，让修改的配置生效
    init();
    bool is=serialPort_->open(QIODevice::ReadWrite);
    QString str=printfText();
    //QString str1;
    if(is){
        //str1="串口打开成功";
        emit openS(str);
    }else{
        //str1="串口打开失败";
        QString err=errString();
        str+=" | "+ err;
        emit openF(str);
    }
    //qDebug()<<str1;
//    printfText();
}

void SerialComm::onClose()
{
    if(serialPort_ && serialPort_->isOpen())serialPort_->close();
    //qDebug()<<"串口关闭";
    QString str=printfText();
    emit close(str);
}

void SerialComm::onRead()
{
    qDebug()<<"read";
    if(!serialPort_ || !serialPort_->isOpen())return;
    QByteArray data=serialPort_->readAll();

    if(data.isEmpty())return;
    emit read(data);

}

QString SerialComm::errString()
{
     QString errStr;
     QSerialPort::SerialPortError err= serialPort_->error();
     switch (err) {
     case QSerialPort::DeviceNotFoundError:
         errStr="找不到该串口,检查设备是否插好、端口号是否选对";
         break;
     case QSerialPort::PermissionError:
         errStr="串口被占用,关掉其他串口程序（Modbus Slave、另一份助手）";
         break;
     case QSerialPort::OpenError:
         errStr="串口打开失败 换一个端口试试";
         break;
     case QSerialPort::UnsupportedOperationError:
         errStr="当前参数不被支持 换波特率/数据位";
         break;
     default:
         errStr=serialPort_->errorString();
         if(errStr.isEmpty()) errStr = "未知原因";
     }
     return errStr;
}

void SerialComm::init()
{
    if(!serialPort_)serialPort_=new QSerialPort;
    serialPort_->setPortName(config_.name_);
    serialPort_->setBaudRate(config_.baudRate_);
    serialPort_->setDataBits(QSerialPort::Data8);
    serialPort_->setParity(QSerialPort::NoParity);
    serialPort_->setStopBits(QSerialPort::OneStop);
    serialPort_->setFlowControl(QSerialPort::NoFlowControl);
    /*bool ok=*/connect(serialPort_,&QSerialPort::readyRead,this,&SerialComm::onRead,Qt::UniqueConnection);
    //qDebug()<<"c:"<<ok;
}

QString SerialComm::printfText()
{
    QString str;
    if(serialPort_){
       str=serialPort_->portName()+" "+QString::number(serialPort_->baudRate());
       qDebug()<<str;
       //emit sendSerialPrint(str);
    }
    return str;
}
