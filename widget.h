#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include"serialcomm.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
signals:
    void write(const QByteArray &data);
    void open();
    void close();
    void setName(const QString &arg1);
    void setBaudRate(int index);
private slots:
    void onRead(const QByteArray &data);
    void onOpenF(const QString &str);
    void onOpenS(const QString &str);
    void onClose(const QString &str);
 //   void onGetSerialPrint(const QString &str);
    void onWriteNote(const QString &str,bool is,qint64 n);

    void on_send_clicked();

    void on_open_clicked();



    void on_PortName_activated(const QString &arg1);

    void on_BaudRate_activated(int index);

    //void on_checkBox_clicked();

    void on_pushButton_clicked();

private:
    void init();
 //   QString changeShowHex(const QByteArray &data);
private:
    Ui::Widget *ui;
    bool is_;
    SerialComm *serCom_;
    QThread *thread_;
    //QString str_;
    //bool isShow_;
    int write_;
    int read_;
    QByteArray sendArray_;
};
#endif // WIDGET_H
