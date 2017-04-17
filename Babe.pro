#-------------------------------------------------
#
# Project created by QtCreator 2016-11-16T00:28:36
#
#-------------------------------------------------


install_it.path = $$OUT_PWD
install_it.files = data/*

unix:!macx: LIBS += -L$$PWD/3rdparty/taglib-1.9.1/taglib/ -ltag
INCLUDEPATH += $$PWD/3rdparty/taglib-1.9.1/taglib/Headers
DEPENDPATH += $$PWD/3rdparty/taglib-1.9.1/taglib/Headers

QT       += core gui
QT       += multimedia
QT       += sql
QT       += network
QT       += xml
QT       += dbus
QT       += KConfigCore
QT       += KNotifications
QT       += KI18n





CONFIG   += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Babe
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



SOURCES += main.cpp\
        mainwindow.cpp \
        playlist.cpp \
        track.cpp \
    settings.cpp \
    collectionDB.cpp \
    about.cpp \
    babetable.cpp \
    album.cpp \
    albumsview.cpp \
    scrolltext.cpp \
    playlistsview.cpp \
    taginfo.cpp \
    artwork.cpp \
    infoview.cpp \
    lyrics.cpp \
    mpris2.cpp \
    notify.cpp \
    youtube.cpp \
    metadataForm.cpp \
    onlineview.cpp \
    web_jgm90.cpp

HEADERS  += mainwindow.h \
            playlist.h \
            track.h \
    settings.h \
    collectionDB.h \
    about.h \
    babetable.h \
    album.h \
    albumsview.h \
    scrolltext.h \
    playlistsview.h \
    taginfo.h \
    artwork.h \
    infoview.h \
    lyrics.h \
    mpris2.h \
    notify.h \
    youtube.h \
    baeUtils.h \
    metadataForm.h \
    onlineview.h \
    web_jgm90.h

FORMS    += mainwindow.ui \
    settings.ui \
    about.ui \
    infoview.ui \
    metadataForm.ui \
    onlineview.ui

RESOURCES += \
    data.qrc


