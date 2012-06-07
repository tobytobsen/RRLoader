#include "qrrmainwin.h"
#include <QBitmap>
#include <QPixmap>
#include <QPainter>
#include <QBrush>
#include <QColor>

QRRMainWin::QRRMainWin(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint)
{
	// prepare window user interface
	ui.setupUi(this);

	// center window
	QRect frect = this->frameGeometry();
	frect.moveCenter(QDesktopWidget().availableGeometry().center());
	this->move(frect.topLeft());

#if defined(Q_OS_WIN)
	moving = false;
#endif
}

QRRMainWin::~QRRMainWin()
{

}

void QRRMainWin::resizeEvent (QResizeEvent *event)
{
	// create bitmap mask
	QBitmap tBitmap(this->width(), this->height());
	tBitmap.clear();

	// draw mask
	QPainter tPainter((QPixmap*)&tBitmap);
	tPainter.setBrush( QBrush( QColor(0, 0, 0) ));
	tPainter.drawRoundedRect(QRect(0, 0, this->width()-1, this->height()-1 ), 7.0, 7.0);

	// apply mask
	this->setMask(tBitmap);
}

void QRRMainWin::messageFromSecondInstance(const QString &message)
{
	// this will be called for each file passed by second application instance
}






#if defined(Q_OS_WIN)

// implemention of moving window under win os
// see moving FramelessWindowHint window:
// http://qt-project.org/faq/answer/how_can_i_move_a_window_around_when_using_qtframelesswindowhint

void QRRMainWin::mousePressEvent(QMouseEvent *event)
{
    QMainWindow::mousePressEvent(event);
    if((event->button() == Qt::LeftButton)) {
        moving = true;
        offset = event->pos();
    }
}
 
void QRRMainWin::mouseMoveEvent(QMouseEvent *event)
{
    QMainWindow::mouseMoveEvent(event);
    if(moving)
        this->move(event->globalPos() - offset);
}
 
void QRRMainWin::mouseReleaseEvent(QMouseEvent *event)
{
    QMainWindow::mouseReleaseEvent(event);
    if(event->button() == Qt::LeftButton) {
        moving = false;
    }
}
#endif