#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T00:03:28
#
#-------------------------------------------------

QT       += widgets testlib

TARGET = ../bin/tst_jptestfilereader_ut
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_jptestfilereader_ut.cpp \
    main.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

# -----------------------------------------------------------------------
# CLS - Keep auto-generated source and object files in separate directory
# -----------------------------------------------------------------------
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build
# -----------------------------------------------------------------------

HEADERS += \
    tst_jptestfilereader_ut.h

unix|win32: LIBS += -L$$PWD/../../../lib/ -lJPTestLib

INCLUDEPATH += $$PWD/../../../include
DEPENDPATH += $$PWD/../../../include

win32: PRE_TARGETDEPS += $$PWD/../../../lib/JPTestLib.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../../../lib/libJPTestLib.a