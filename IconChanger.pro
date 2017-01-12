#-------------------------------------------------
#
# Project created by QtCreator 2016-12-13T20:06:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IconChanger
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h \
    cicontoimage.h

FORMS    += dialog.ui

win32:LIBS += -luser32 -lgdiplus -lShlwapi -lgdi32
