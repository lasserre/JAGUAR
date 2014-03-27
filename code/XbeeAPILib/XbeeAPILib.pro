#-------------------------------------------------
#
# Project created by QtCreator 2014-03-15T13:36:48
#
#-------------------------------------------------

QT       -= gui

TARGET = lib/XbeeAPILib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    src/xbee_api_basetypes.cpp \
    src/xbee_frameparser.cpp \
    src/xbee_framewriter.cpp

HEADERS += \
    include/xbee_api_basetypes.h \
    include/xbee_frameparser.h \
    include/xbee_api_frame_defines.h \
    include/xbee_frame_structs.h \
    include/xbee_framewriter.h \
    include/xbee_frame_enums.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += "include"

message(sprintf($$PWD))

# -----------------------------------------------------------------------
# CLS - Keep auto-generated source and object files in separate directory
# -----------------------------------------------------------------------
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build
# -----------------------------------------------------------------------
