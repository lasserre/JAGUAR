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

Debug: DESTDIR = ../bin/debug
