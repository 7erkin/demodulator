#-------------------------------------------------
#
# Project created by QtCreator 2017-11-30T14:56:30
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = newdemo
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    demodulator.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    demodulator.h \
    qcustomplot.h \
    receiver.h

FORMS    += mainwindow.ui
