#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T09:51:19
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LinkRealNetDisk
TEMPLATE = app
#RC_ICONS =app.ico
win32:RC_FILE = LinkRealNetDisk.rc
#RC_FILE = resources.rc

#LIBS += D:\Qt\Qt5.7.1\5.7\mingw53_32\bin\Qt5Ftp.dll

SOURCES += main.cpp\
        mainwidget.cpp \
    QFolder/qfolder.cpp \
    filespanel.cpp \
    Pathview/pathview.cpp \
    Pathview/pathbox.cpp \
    Http/nethttp.cpp \
    Http/nettrans.cpp \
    Translist/translist.cpp \
    Translist/bardelegata.cpp \
    netconfig.cpp \
    configpanel.cpp \
    loading.cpp \
    login.cpp \
    taesclass.cpp \
    Tablewidget/selectdelegate.cpp \
    Tablewidget/transbtn.cpp \
    Tablewidget/listrowwidgets.cpp \
    Netsync/netsync.cpp \
    Translist/synclist.cpp \
    serverconfig.cpp \
    Http/netdonwloader.cpp \
    userinfopanel.cpp \
    Share/sharelink.cpp \
    Tablewidget/linkrowwidgets.cpp

HEADERS  += mainwidget.h \
    QFolder/qfolder.h \
    filespanel.h \
    Pathview/pathview.h \
    Pathview/pathbox.h \
    Http/nethttp.h \
    Http/nettrans.h \
    Translist/translist.h \
    Translist/bardelegata.h \
    netconfig.h \
    configpanel.h \
    loading.h \
    login.h \
    taesclass.h \
    Tablewidget/selectdelegate.h \
    Tablewidget/transbtn.h \
    Netsync/netsync.h \
    Translist/synclist.h \
    serverconfig.h \
    Http/netdonwloader.h \
    userinfopanel.h \
    Tablewidget/listrowwidgets.h \
    Share/sharelink.h \
    Tablewidget/linkrowwidgets.h

FORMS    += mainwidget.ui \
    QFolder/qfolder.ui \
    filespanel.ui \
    Pathview/pathview.ui \
    Translist/translist.ui \
    configpanel.ui \
    loading.ui \
    login.ui \
    Translist/synclist.ui \
    serverconfig.ui \
    userinfopanel.ui \
    Share/sharelink.ui

RESOURCES += \
    img.qrc

DISTFILES +=
