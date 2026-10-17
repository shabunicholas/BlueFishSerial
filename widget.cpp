#include "widget.h"
#include "ui_widget.h"
#include<QSerialPortInfo>
#include<QByteArray>
#include<QThread>
#include<QDebug>
#include<QMessageBox>
#include<QRegularExpression>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    ,is_(false)
    ,serCom_(new SerialComm())
    ,thread_(nullptr)
    //,isShow_(false)
{
    ui->setupUi(this);
    init();
}

Widget::~Widget()
{
    if(thread_){
        thread_->quit();
        thread_->wait();
    }

    delete ui;
}

void Widget::onRead(const QByteArray &data)
{


    read_+=data.size();
    //qDebug()<<"读了:"<<read_;
    ui->label_2->setText(QString("读了:%1字节").arg(read_));

    if(ui->checkBox->isChecked()){
       ui->textEdit_show->append("<<"+data.toHex(' ').toUpper());
    }else{
       ui->textEdit_show->append("<<"+QString::fromUtf8(data));
    }
}

void Widget::onOpenF(const QString &str)
{
    QMessageBox::information(this,"提示",QString("串口打开失败(%1)").arg(str));
    ui->open->setText("打开串口");
    is_=false;
}

void Widget::onOpenS(const QString &str)
{
    QMessageBox::information(this,"提示",QString("串口打开成功(%1)").arg(str));
    ui->open->setText("关闭串口");
    is_=true;
}

void Widget::onClose(const QString &str)
{
    QMessageBox::information(this,"提示",QString("串口关闭(%1)").arg(str));
    ui->open->setText("打开串口");
    is_=false;
}



//void Widget::onGetSerialPrint(const QString &str)
//{
//    str_=str;
//}


void Widget::init()
{
    //界面初始化
    write_=read_=0;

    QList<QSerialPortInfo>list= QSerialPortInfo::availablePorts();
    QList<qint32> listB=QSerialPortInfo::standardBaudRates();
    for (int i=0;i< list.size();i++) {
        QSerialPortInfo info=list.at(i);
        ui->PortName->addItem(info.portName());
    }

    for (int i=0;i< listB.size();i++) {
        ui->BaudRate->addItem(QString::number(listB.at(i)));
    }

    ui->BaudRate->setCurrentText(QString::number(serCom_->getConfig().baudRate_));
    ui->PortName->setCurrentText(serCom_->getConfig().name_);

    //链接
    connect(this,&Widget::write,serCom_,&SerialComm::onWrite);
    connect(this,&Widget::open,serCom_,&SerialComm::onOpen);
    connect(this,&Widget::close,serCom_,&SerialComm::onClose);
    connect(this,&Widget::setName,serCom_,&SerialComm::onSetName);
    connect(this,&Widget::setBaudRate,serCom_,&SerialComm::onSetBaudRate);

    connect(serCom_,&SerialComm::openF,this,&Widget::onOpenF);
    connect(serCom_,&SerialComm::openS,this,&Widget::onOpenS);
    connect(serCom_,&SerialComm::close,this,&Widget::onClose);
    connect(serCom_,&SerialComm::read,this,&Widget::onRead);
//    connect(serCom_,&SerialComm::openNote,this,&Widget::onOpen);



    thread_=new QThread(this);
    serCom_->moveToThread(thread_);
    connect(thread_,&QThread::started,serCom_,&SerialComm::init);
    connect(thread_,&QThread::finished,serCom_,&SerialComm::deleteLater);


    thread_->start();

}

//QString Widget::changeShowHex(const QByteArray &data)
//{
//    QString str;
//    bool is=ui->checkBox->isChecked();
//    if(is){
//        str=data.toHex(' ').toUpper();
//    }else{
//       str= QString::fromUtf8(data);
//    }
//    return str;
//}


void Widget::on_send_clicked()
{
    if(!is_){
        QMessageBox::information(this,"提示","串口没打开");
        return;
    }

    QString inputText = ui->textEdit_send->toPlainText();

    if (inputText.isEmpty()) return;

    QByteArray sendArray; // 最终真正要发给串口的真字节

    if (ui->checkBox->isChecked()) {
        QString clearText=inputText.trimmed().remove(' ').toUpper();
        clearText.remove(QRegularExpression("\\s"));

        QRegularExpression rex("^[0-9A-Fa-f]+$");
        QRegularExpressionMatch math=rex.match(clearText);
        if(!math.hasMatch()){
            QMessageBox::information(this,"提示",
                                     QString("没有可发送的十六进制数据(%1)").arg(clearText));
            return;
        }
        if(clearText.length()%2 !=0){
            QMessageBox::information(this,"提示",
                                    "十六进制字符个数必须是偶数");
            return;
        }
        sendArray = QByteArray::fromHex(clearText.toUtf8());
        qDebug() << "--- Hex发送 --- 大小:" << sendArray.size();
        ui->textEdit_show->append(">>"+sendArray.toHex(' ').toUpper());
    }
    else {
        // 纯文本模式
        sendArray = inputText.toUtf8();
        qDebug() << "--- text发送 --- 大小:" << sendArray.size();
        ui->textEdit_show->append(">>"+QString::fromUtf8(sendArray));
    }

    emit write(sendArray);


    write_ += sendArray.size();
    ui->label->setText(QString("写了:%1字节").arg(write_));

    // ui->textEdit_send->clear();
}
void Widget::on_open_clicked()
{
    if(!is_){
       emit open();
       //ui->open->setText("关闭串口");
    }else{
       emit close();
       //ui->open->setText("打开串口");
    }
}



void Widget::on_PortName_activated(const QString &arg1)
{

    emit setName(arg1);
}

void Widget::on_BaudRate_activated(int index)
{
    //qDebug()<<index;
    int baudRate=ui->BaudRate->currentText().toInt();
    emit setBaudRate(baudRate);
}

//void Widget::on_checkBox_clicked()
//{
//    if(ui->checkBox->isChecked()){
//       isShow_=true;
//    }else{
//       isShow_=false;
//    }
//}

void Widget::on_pushButton_clicked()
{
    write_=read_=0;
    ui->textEdit_send->clear();
    ui->textEdit_show->clear();
    ui->label->setText("写了:0字节qwq");
    ui->label_2->setText("读了:0字节qwq");
}
