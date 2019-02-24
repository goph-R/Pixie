#-------------------------------------------------
#
# Project created by QtCreator 2019-02-16T21:58:40
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pixie
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    main.cpp \
    domain/config.cpp \
    domain/filemanagerworker.cpp \
    domain/filemanager.cpp \
    domain/file.cpp \
    domain/foundfile.cpp \
    domain/foundfolder.cpp \
    domain/thumbnailqueue.cpp \
    domain/thumbnailworker.cpp \
    domain/thumbnaildatabase.cpp \
    ui/mainwindow.cpp \
    ui/foldertreewidget.cpp \
    ui/foldertreeitem.cpp \
    ui/filelistwidget.cpp \
    ui/filelistdelegate.cpp \
    ui/filelistitem.cpp \
    ui/viewwindow.cpp \
    ui/viewwidget.cpp

HEADERS += \
    domain/config.h \
    domain/filemanagerworker.h \
    domain/filemanager.h \
    domain/file.h \
    domain/foundfile.h \
    domain/foundfolder.h \
    domain/thumbnailqueue.h \
    domain/thumbnailworker.h \
    domain/thumbnaildatabase.h \
    ui/mainwindow.h \
    ui/foldertreewidget.h \
    ui/foldertreeitem.h \
    ui/filelistwidget.h \
    ui/filelistdelegate.h \
    ui/filelistitem.h \
    ui/viewwindow.h \
    ui/viewwidget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resources.qrc
