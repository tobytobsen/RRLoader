#include "qrrmainwin.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QRRMainWin w;
    w.show();
    return a.exec();
}
