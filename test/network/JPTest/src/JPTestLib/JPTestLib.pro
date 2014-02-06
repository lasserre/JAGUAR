#-------------------------------------------------
#
# Project created by QtCreator 2014-02-01T23:16:02
#
#-------------------------------------------------

QT       += widgets gui serialport

win32: TARGET = ../../../lib/JPTestLib
unix: TARGET = ../../lib/JPTestLib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    jpacket.cpp \
    jptest.cpp \
    jptestcontroller.cpp \
    jptestoptions.cpp \
    jptestport.cpp \
    jptestfilereader.cpp \
    jptestcoordinator.cpp \
    jpoutbox.cpp \
    jpinbox.cpp \
    jpacketchecker.cpp

HEADERS += \
    ../../include/jpacket.h \
    ../../include/jptest.h \
    ../../include/jptestcontroller.h \
    ../../include/jptestoptions.h \
    ../../include/jptestport.h \
    ../../include/jptestfilereader.h \
    ../../include/jptestcoordinator.h \
    ../../include/jpoutbox.h \
    ../../include/jpinbox.h \
    ../../include/jpacketchecker.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += ../../include/

# -----------------------------------------------------------------------
# CLS - Keep auto-generated source and object files in separate directory
# -----------------------------------------------------------------------
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build
# -----------------------------------------------------------------------
