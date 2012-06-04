#ifndef QRRMAINWIN_H
#define QRRMAINWIN_H

#include <QtGui/QMainWindow>
#include <QDesktopWidget>
#include <QMouseEvent>
#include "ui_qrrmainwin.h"

class QRRMainWin : public QMainWindow
{
    Q_OBJECT

	public:
		QRRMainWin(QWidget *parent = 0);
		~QRRMainWin();

	protected:
		void resizeEvent ( QResizeEvent * event );

	private:
		Ui::QRRMainWinClass ui;

	public slots:
		void messageFromSecondInstance(const QString &message);

#if defined(Q_OS_WIN)

	protected:
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
	private:
		QPoint offset;
		bool moving;

#endif

};

#endif // QRRMAINWIN_H
