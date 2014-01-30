#-------------------------------------------------
#
# Project created by QtCreator 2014-01-14T15:24:52
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JPTest
TEMPLATE = app


SOURCES += main.cpp\
        jptmainwindow.cpp \
    jptestport.cpp \
    jpacket.cpp \
    jptest.cpp \
    jptestcontroller.cpp

HEADERS  += jptmainwindow.h \
    jptestport.h \
    jpacket.h \
    jptest.h \
    jptestcontroller.h

FORMS    += jptmainwindow.ui

Release: DESTDIR = ../bin/release
Release: OBJECTS_DIR = $${DESTDIR}
Release: MOC_DIR = $${DESTDIR}
Release: RCC_DIR = $${DESTDIR}
Release: UI_DIR = $${DESTDIR}

Debug: DESTDIR = ../bin/debug
Debug: OBJECTS_DIR = $${DESTDIR}
Debug: MOC_DIR = $${DESTDIR}
Debug: RCC_DIR = $${DESTDIR}
Debug: UI_DIR = $${DESTDIR}
