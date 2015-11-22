#-------------------------------------------------
#
# Project created by QtCreator 2015-11-20T12:10:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IPM-08irSZORG
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    frame.cpp \
    packet.cpp \
    scheduler.cpp \
    pipeline.cpp

HEADERS  += mainwindow.h \
    frame.h \
    packet.h \
    common.h \
    scheduler.h \
    pipeline.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
