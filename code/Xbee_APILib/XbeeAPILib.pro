#-------------------------------------------------
#
# Project created by QtCreator 2014-03-15T13:36:48
#
#-------------------------------------------------

QT       -= core gui

TARGET = XbeeAPILib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    src/xbee_packetparser.cpp \
    src/xbee_api.cpp \
    src/xbee_packetcreator.cpp

HEADERS += \
    include/xbee_packetparser.h \
    include/xbee_api.h \
    include/xbee_packetcreator.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += "include"
