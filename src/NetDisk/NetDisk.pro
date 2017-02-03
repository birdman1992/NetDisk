#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T09:51:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetDisk
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    QFolder/qfolder.cpp \
    filespanel.cpp \
    qclipboard.cpp

HEADERS  += mainwidget.h \
    QFolder/qfolder.h \
    filespanel.h \
    qclipboard.h

FORMS    += mainwidget.ui \
    QFolder/qfolder.ui \
    filespanel.ui

RESOURCES += \
    img.qrc
