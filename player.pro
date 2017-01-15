#-------------------------------------------------
#
# Project created by QtCreator 2016-11-16T00:28:36
#
#-------------------------------------------------

unix:!macx: LIBS += -L$$PWD/3rdparty/taglib-1.9.1/taglib/ -ltag
INCLUDEPATH += $$PWD/3rdparty/taglib-1.9.1/taglib/Headers
DEPENDPATH += $$PWD/3rdparty/taglib-1.9.1/taglib/Headers


QT       += core gui
QT       += multimedia
QT       += sql
CONFIG   += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        playlist.cpp \
        track.cpp \
    settings.cpp \
    info.cpp \
    collectionDB.cpp \
    about.cpp \
    babetable.cpp \
    album.cpp \
    albumsview.cpp \
    scrolltext.cpp

HEADERS  += mainwindow.h \
            playlist.h \
            track.h \
    settings.h \
    info.h \
    collectionDB.h \
    about.h \
    babetable.h \
    album.h \
    albumsview.h \
    scrolltext.h

FORMS    += mainwindow.ui \
    settings.ui \
    info.ui \
    babetable.ui \
    albumsview.ui
