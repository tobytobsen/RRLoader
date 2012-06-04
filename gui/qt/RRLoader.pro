TEMPLATE = app
TARGET = RRLoader
QT += core \
    gui \
    network
HEADERS += src/QtSingleApplication/qtlocalpeer.h \
    src/QtSingleApplication/qtlockedfile.h \
    src/QtSingleApplication/qtsingleapplication.h \
    src/QtSingleApplication/qtsinglecoreapplication.h \
    src/qrrmainwin.h
SOURCES += src/QtSingleApplication/qtlocalpeer.cpp \
    src/QtSingleApplication/qtlockedfile.cpp \
    src/QtSingleApplication/qtlockedfile_unix.cpp \
    src/QtSingleApplication/qtlockedfile_win.cpp \
    src/QtSingleApplication/qtsingleapplication.cpp \
    src/QtSingleApplication/qtsinglecoreapplication.cpp \
    src/qrrmainwin.cpp \
    src/main.cpp
FORMS += src/qrrmainwin.ui
RESOURCES += 
