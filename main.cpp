#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QByteArray>("QByteArray");
    Widget w;
    w.show();
    return a.exec();
}
