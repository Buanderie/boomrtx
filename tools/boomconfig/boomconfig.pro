#-------------------------------------------------
#
# Project created by QtCreator 2019-02-05T14:12:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = boomconfig
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    settingsdialog.cpp \
    ../../common/crc16.cpp \
    ledindicator.cpp \
    propertyframe.cpp \
    qroundprogressbar.cpp

HEADERS += \
        mainwindow.h \
    settingsdialog.h \
    ../../common/frame.h \
    ledindicator.h \
    propertyframe.h \
    ../../common/frame.h \
    ../../common/frameparser.h \
    linkquality.h \
    qroundprogressbar.h

FORMS += \
        mainwindow.ui \
    settingsdialog.ui \
    propertyframe.ui
