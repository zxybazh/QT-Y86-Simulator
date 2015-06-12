#-------------------------------------------------
#
# Project created by QtCreator 2015-06-08T13:23:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Y86
TEMPLATE = app


SOURCES += main.cpp\
        simulator.cpp \
    state.cpp

HEADERS  += simulator.h \
    state.h

FORMS    += simulator.ui

RESOURCES += res.qrc
