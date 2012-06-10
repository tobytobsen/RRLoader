#include "qrrmainwin.h"
#include <QDebug>
#include <QBitmap>
#include <QPixmap>
#include <QPainter>
#include <QBrush>
#include <QColor>

const float STYLE_BORDER_RADIUS = 7.0;
const int   ENV_GRIP_WIDTH = 5;


QRRMainWin::QRRMainWin(QWidget *parent)
		: QMainWindow(parent, Qt::FramelessWindowHint)
{
	// prepare window user interface
	ui.setupUi(this);

	// center window
	QRect frect = this->frameGeometry();
	frect.moveCenter(QDesktopWidget().availableGeometry().center());
	this->move(frect.topLeft());

	// set drag mode to none
	this->pMoving = WDRAG_NONE;
}

QRRMainWin::~QRRMainWin()
{

}

void QRRMainWin::messageFromSecondInstance(const QString &message)
{
	// this will be called for each file passed by second application instance
}

void QRRMainWin::resizeEvent(QResizeEvent *event)
{
	// create bitmap mask
	QBitmap tBitmap(this->width(), this->height());
	tBitmap.clear();

	// draw mask
	QPainter tPainter((QPixmap*)&tBitmap);
	tPainter.setBrush( QBrush( QColor(0, 0, 0) ));

	if (this->windowState() & Qt::WindowMaximized)
		tPainter.drawRect(0, 0, this->width()-1, this->height()-1);
	else
		tPainter.drawRoundedRect(QRect(0, 0, this->width()-1, this->height()-1 ), STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS);

	// apply mask
	this->setMask(tBitmap);
}

void QRRMainWin::mousePressEvent(QMouseEvent *event)
{
	QMainWindow::mousePressEvent(event);
	if (event->button() == Qt::LeftButton)
	{
		this->pMoving = this->checkMouseDragFlag(event->globalPos());

		if (this->pMoving & WDRAG_CENTER)
			this->pOffset = event->globalPos();
		else
		{
			this->pRect = this->geometry();

			if (this->pMoving & WDRAG_RIGHT)
				this->pOffset.setX(event->globalPos().x() - pRect.right());
			else
			if(this->pMoving & WDRAG_LEFT)
				this->pOffset.setX(event->globalPos().x() - pRect.left());
		}
	}
}

void QRRMainWin::mouseMoveEvent(QMouseEvent *event)
{
	QMainWindow::mouseMoveEvent(event);

	// handle resize and move events
	if (this->pMoving & WDRAG_CENTER)
		this->move(event->globalPos() - this->pOffset);
	else
	if (this->pMoving)
	{
		QRect tRect = this->pRect;
		QSize tSize = this->minimumSize();
		QPoint tPoint = event->globalPos() - this->pOffset;


		if (this->pMoving & WDRAG_RIGHT)
			tRect.setRight(tPoint.x());
		else
		if (this->pMoving & WDRAG_LEFT)
		{
			tRect.setLeft(tPoint.x());
			if (tRect.width() < tSize.width())
				tRect.setLeft(tRect.right() - tSize.width());
		}

		if (this->pMoving & WDRAG_BOTTOM)
			tRect.setBottom(tPoint.y());
		else
		if (this->pMoving & WDRAG_TOP)
		{
			tRect.setTop(tPoint.y());
			if (tRect.height() < tSize.height())
				tRect.setTop(tRect.bottom() - tSize.height());
		}

		this->setGeometry(tRect);
	}

	// change mouse cursor on the edges of the window
	if (this->pMoving == WDRAG_NONE)
	{
		Qt::CursorShape cursor = Qt::ArrowCursor;
		int mouseFlag = this->checkMouseDragFlag(event->globalPos());

		if (mouseFlag & WDRAG_TOP)
		{
			if (mouseFlag & WDRAG_LEFT)
				cursor = Qt::SizeFDiagCursor;
			else
			if (mouseFlag & WDRAG_RIGHT)
				cursor = Qt::SizeBDiagCursor;
			else
				cursor = Qt::SizeVerCursor;
		}
		else if (mouseFlag & WDRAG_BOTTOM)
		{
			if (mouseFlag & WDRAG_LEFT)
				cursor = Qt::SizeBDiagCursor;
			else
			if (mouseFlag & WDRAG_RIGHT)
				cursor = Qt::SizeFDiagCursor;
			else
				cursor = Qt::SizeVerCursor;
		}
		else
		if ((mouseFlag & WDRAG_LEFT) || (mouseFlag & WDRAG_RIGHT))
			cursor = Qt::SizeHorCursor;

		this->setCursor( cursor );
	}
}

void QRRMainWin::mouseReleaseEvent(QMouseEvent *event)
{
	QMainWindow::mouseReleaseEvent(event);
	if (event->button() == Qt::LeftButton)
		this->pMoving = WDRAG_NONE;
}

int QRRMainWin::checkMouseDragFlag(const QPoint &pos) const
{
	int flag = WDRAG_NONE;
	QRect tRect = this->geometry();

	// mouse within a edge box
	bool top    = QRect(tRect.left(),  tRect.top(),    tRect.width(), 	 ENV_GRIP_WIDTH).contains(pos);
	bool bottom = QRect(tRect.left(),  tRect.bottom(), tRect.width(), 	-ENV_GRIP_WIDTH).contains(pos);
	bool left   = QRect(tRect.left(),  tRect.top(),     ENV_GRIP_WIDTH, tRect.height()) .contains(pos);
	bool right  = QRect(tRect.right(), tRect.top(),    -ENV_GRIP_WIDTH, tRect.height()) .contains(pos);

	// evaluate position calculations
	if (top)
		flag |= WDRAG_TOP;
	if (bottom)
		flag |= WDRAG_BOTTOM;
	if (left)
		flag |= WDRAG_LEFT;
	if (right)
		flag |= WDRAG_RIGHT;

	return flag;
}
