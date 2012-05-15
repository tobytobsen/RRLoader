#ifndef QRRMAINWIN_H
#define QRRMAINWIN_H

#include <QtGui/QMainWindow>
#include "ui_qrrmainwin.h"

class QRRMainWin : public QMainWindow
{
    Q_OBJECT

public:
    QRRMainWin(QWidget *parent = 0);
    ~QRRMainWin();

private:
    Ui::QRRMainWinClass ui;
};

#endif // QRRMAINWIN_H
