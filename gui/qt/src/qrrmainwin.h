#ifndef QRRMAINWIN_H
#define QRRMAINWIN_H

#include <QtGui/QMainWindow>
#include <QDesktopWidget>
#include <QMouseEvent>
#include "ui_qrrmainwin.h"

#define WDRAG_NONE			0x0000
#define WDRAG_CENTER		0x0001
#define WDRAG_TOP			0x0002
#define WDRAG_RIGHT			0x0004
#define WDRAG_BOTTOM		0x0008
#define WDRAG_LEFT			0x0010

class QRRMainWin : public QMainWindow
{
    Q_OBJECT

	public:
		QRRMainWin(QWidget *parent = 0);
		~QRRMainWin();

	public slots:
		void messageFromSecondInstance(const QString &message);

	protected:
		void resizeEvent ( QResizeEvent * event );
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		int checkMouseDragFlag(const QPoint &pos) const;

	private:
		Ui::QRRMainWinClass ui;
		QPoint pOffset;
		QRect pRect;
		int pMoving;
};

#endif // QRRMAINWIN_H
