#-------------------------------------------------
#
# Project created by QtCreator 2014-02-01T23:49:14
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32: TARGET = ../../../bin/JPTest
unix: TARGET = ../../bin/JPTest
TEMPLATE = app

SOURCES += \
    jptmainwindow.cpp \
    main.cpp

FORMS += \
    jptmainwindow.ui

HEADERS += \
    jptmainwindow.h

unix|win32: LIBS += -L$$PWD/../../lib/ -lJPTestLib

INCLUDEPATH += $$PWD/../../include
DEPENDPATH += $$PWD/../../include

PRE_TARGETDEPS += $$PWD/../../lib/libJPTestLib.a

# -----------------------------------------------------------------------
# CLS - Keep auto-generated source and object files in separate directory
# -----------------------------------------------------------------------
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build
# -----------------------------------------------------------------------

RESOURCES += \
    icons.qrc
