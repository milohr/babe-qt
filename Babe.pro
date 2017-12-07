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
QT       += websockets
QT       += network
QT       += xml
QT       += dbus
QT       += KConfigCore
QT       += KNotifications
QT       += KI18n
QT       += webenginewidgets

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

DISTFILES += \
    extra/extension/background.js \
    extra/extension/popup.js \
    extra/extension/manifest.json \
    src/db/queries.json \
    src/db/script.sql \
    extra/extension/icon_1.png \
    extra/extension/icon_2.png \
    extra/extension/icon_done.png \
    extra/extension/icon_1.svg \
    extra/extension/icon_push.svg \
    extra/extension/style.css \
    extra/extension/popup.html

RESOURCES += \
    data.qrc

FORMS += \
    src/dialogs/about.ui \
    src/dialogs/metadataForm.ui \
    src/dialogs/playlistform.ui \
    src/settings/settings.ui \
    src/views/babewindow.ui \
    src/views/infoview.ui

HEADERS += \
    src/data_models/track.h \
    src/data_models/tracklist.h \
    src/db/collectionDB.h \
    src/db/rabbithole.h \
    src/dialogs/about.h \
    src/dialogs/metadataform.h \
    src/dialogs/playlistform.h \
    src/kde/mpris2.h \
    src/kde/notify.h \
    src/pulpo/services/geniusService.h \
    src/pulpo/services/lastfmService.h \
    src/pulpo/services/lyricwikiaService.h \
    src/pulpo/services/musicbrainzService.h \
    src/pulpo/services/spotifyService.h \
    src/pulpo/enums.h \
    src/pulpo/htmlparser.h \
    src/pulpo/pulpo.h \
    src/pulpo/webengine.h \
    src/services/local/socket.h \
    src/services/local/taginfo.h \
    src/services/web/youtube.h \
    src/settings/fileloader.h \
    src/settings/settings.h \
    src/utils/albumloader.h \
    src/utils/bae.h \
    src/utils/brain.h \
    src/views/albumsview.h \
    src/views/babewindow.h \
    src/views/infoview.h \
    src/views/playlistsview.h \
    src/views/rabbitview.h \
    src/widget_models/babealbum.h \
    src/widget_models/babegrid.h \
    src/widget_models/babetable.h \
    src/widget_models/scrolltext.h

SOURCES += \
    src/data_models/track.cpp \
    src/data_models/tracklist.cpp \
    src/db/collectionDB.cpp \
    src/dialogs/about.cpp \
    src/dialogs/metadataform.cpp \
    src/dialogs/playlistform.cpp \
    src/kde/mpris2.cpp \
    src/kde/notify.cpp \
    src/pulpo/services/geniusService.cpp \
    src/pulpo/services/lastfmService.cpp \
    src/pulpo/services/lyricwikiaService.cpp \
    src/pulpo/services/musicbrainzService.cpp \
    src/pulpo/services/spotifyService.cpp \
    src/pulpo/htmlparser.cpp \
    src/pulpo/pulpo.cpp \
    src/pulpo/webengine.cpp \
    src/services/local/socket.cpp \
    src/services/local/taginfo.cpp \
    src/services/web/youtube.cpp \
    src/settings/settings.cpp \
    src/views/albumsview.cpp \
    src/views/babewindow.cpp \
    src/views/infoview.cpp \
    src/views/playlistsview.cpp \
    src/views/rabbitview.cpp \
    src/widget_models/babealbum.cpp \
    src/widget_models/babegrid.cpp \
    src/widget_models/babetable.cpp \
    src/widget_models/scrolltext.cpp \
    src/main.cpp

