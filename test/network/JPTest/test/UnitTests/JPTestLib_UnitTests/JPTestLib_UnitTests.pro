#-------------------------------------------------
#
# Project created by QtCreator 2014-02-02T00:03:28
#
#-------------------------------------------------

QT       += widgets testlib

win32: TARGET = ../../bin/JPTestLib_UnitTests
unix: TARGET = ../bin/JPTestLib_UnitTests
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

PRE_TARGETDEPS += $$PWD/../../../lib/libJPTestLib.a
