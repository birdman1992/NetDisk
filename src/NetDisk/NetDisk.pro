#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T09:51:19
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetDisk
TEMPLATE = app

LIBS += D:\Qt\Qt5.7.1\5.7\mingw53_32\bin\Qt5Ftp.dll

SOURCES += main.cpp\
        mainwidget.cpp \
    QFolder/qfolder.cpp \
    filespanel.cpp \
    Ftp/diskftp.cpp \
    Pathview/pathview.cpp \
    Pathview/pathbox.cpp \
    Http/nethttp.cpp \
    Cjson/cJSON.c \
    Cjson/cJSON_Utils.c

HEADERS  += mainwidget.h \
    QFolder/qfolder.h \
    filespanel.h \
    Ftp/diskftp.h \
    Pathview/pathview.h \
    Pathview/pathbox.h \
    Http/nethttp.h \
    Cjson/cJSON.h \
    Cjson/cJSON_Utils.h

FORMS    += mainwidget.ui \
    QFolder/qfolder.ui \
    filespanel.ui \
    Pathview/pathview.ui

RESOURCES += \
    img.qrc

DISTFILES +=
