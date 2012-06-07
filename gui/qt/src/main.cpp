#include "qrrmainwin.h"

#include <QtGui>
#include "QtSingleApplication/qtsingleapplication.h"

int main(int argc, char **argv)
{
	QtSingleApplication application(argc, argv);

	{
		QStringList ArgList = application.arguments();
		for (int i=0; i<ArgList.count(); ++i)
		{
			if (application.sendMessage(ArgList.at(i)))
				return 0;
		}
	}

    // init main window
    QRRMainWin mainwindow;

    QObject::connect(&application, SIGNAL(messageReceived(const QString&)), &mainwindow, SLOT(messageFromSecondInstance(const QString&)));

    mainwindow.show();
    return application.exec();
}