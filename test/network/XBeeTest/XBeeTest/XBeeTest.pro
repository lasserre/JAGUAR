#-------------------------------------------------
#
# Project created by QtCreator 2014-03-22T11:03:55
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XBeeTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/../../../../code/XbeeAPILib/include
DEPENDPATH += $$PWD/../../../../code/XbeeAPILib/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../code/XbeeAPILib/lib/ -lXbeeAPILib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../code/XbeeAPILib/lib/ -lXbeeAPILibd
else:unix: LIBS += -L$$PWD/../../../../code/XbeeAPILib/lib/ -lXbeeAPILib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../code/XbeeAPILib/lib/libXbeeAPILib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../code/XbeeAPILib/lib/libXbeeAPILibd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../code/XbeeAPILib/lib/XbeeAPILib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../code/XbeeAPILib/lib/XbeeAPILibd.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../code/XbeeAPILib/lib/libXbeeAPILib.a
