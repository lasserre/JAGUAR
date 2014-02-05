#-------------------------------------------------
#
# Project created by QtCreator 2014-02-01T23:49:14
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ../../bin/JPTest
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

win32: PRE_TARGETDEPS += $$PWD/../../lib/JPTestLib.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../../lib/libJPTestLib.a

# -----------------------------------------------------------------------
# CLS - Keep auto-generated source and object files in separate directory
# -----------------------------------------------------------------------
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build
# -----------------------------------------------------------------------
