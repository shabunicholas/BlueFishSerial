#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <QObject>
#include<QSerialPort>
//#include<QSerialPortInfo>

//使用Config保存基本信息，只处理的读写
class SerialComm : public QObject
{

    Q_OBJECT
public:
    struct Config{
        QString name_="COM3";
        qint32 baudRate_=QSerialPort::Baud9600;
    };
    explicit SerialComm(QObject *parent = nullptr);
    ~SerialComm();
//    void open();
//    void setName(const QString &name);
//    void setBaudRate(qint32 v);
//    void setDataBits(QSerialPort::DataBits dataBit);
//    void setFlowControl(QSerialPort::FlowControl flowCon);
//    void setParity(QSerialPort::Parity parity);
//    void setStopBits(QSerialPort::StopBits stopBit);
    Config getConfig()const{return  config_;};
    void init();
    QString printfText();
signals:
    void read(const QByteArray &data);

    void sendSerialPrint(const QString &str);
    void openF(const QString &str);
    void openS(const QString &str);
    void close(const QString &str);
public slots:
    void onSetName(const QString &name);
    void onSetBaudRate(qint32 v);
    void onWrite(const QByteArray &data);
    void onOpen();
    void onClose();
private slots:
    void onRead();
private:
    QString errString();
private:
    QSerialPort *serialPort_;
//    QString name_;
    Config config_;
//    QByteArray array_;
};

#endif // SERIALCOMM_H
