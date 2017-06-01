/*
   Babe - tiny music player
   Copyright (C) 2017  Camilo Higuita
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "mpris2.h"
#include <QDBusConnection>
#include <QDBusMessage>

static inline qlonglong convertTime(int t)
{
    return t*1000000;
}

static QString mprisPath;

Mpris::Mpris(QObject *parent) : QObject(parent), pos(-1)
{
    QDBusConnection::sessionBus().registerService("org.mpris.MediaPlayer2.Babe");
    QDBusConnection::sessionBus().registerObject("/org/mpris/MediaPlayer2", this, QDBusConnection::ExportAdaptors);
}

Mpris::~Mpris()
{
    QDBusConnection::sessionBus().unregisterService("org.mpris.MediaPlayer2.cantata");
}

void Mpris::Next()
{
    qDebug() << "next";
}

void Mpris::Previous()
{
    qDebug() << "previous";
}

void Mpris::Pause()
{
    qDebug() << "pause";
}

void Mpris::PlayPause()
{
    qDebug() << "pause";
}

void Mpris::Stop()
{
    qDebug() << "stop";
}

void Mpris::StopAfterCurrent()
{
    qDebug() << "stop after current";
}

void Mpris::Play()
{
    qDebug() << "play";
}

void Mpris::Seek(qlonglong pos)
{
    qDebug() << pos;
}

void Mpris::SetPosition(const QDBusObjectPath &, qlonglong pos)
{
    qDebug() << pos;
}

void Mpris::OpenUri(const QString &)
{
}

QString Mpris::PlaybackStatus() const
{
    qDebug() << "PlaybackStatus";
    return "PlaybackStatus";
}

QString Mpris::LoopStatus()
{
    return "sthm";
}

void Mpris::SetLoopStatus(const QString &s)
{
    emit setRepeat(QLatin1String("None") != s);
}

qlonglong Mpris::Position() const
{
    // Cant use MPDStatus, as we dont poll for track position, but use a timer instead!
    // return MPDStatus::self()->timeElapsed();
    return 1000000000;
}

double Mpris::MinimumRate() const
{
    return 1.0;
}

double Mpris::MaximumRate() const
{
    return 1.0;
}

bool Mpris::CanControl() const
{
    return true;
}

bool Mpris::CanPlay() const
{
    return true;
}

bool Mpris::CanPause() const
{
    return true;
}

bool Mpris::CanSeek() const
{
    return true;
}

bool Mpris::CanGoNext() const
{
    return true;
}

bool Mpris::CanGoPrevious() const
{
    return true;
}

bool Mpris::CanQuit() const
{
    return true;
}

bool Mpris::CanRaise() const
{
    return true;
}

bool Mpris::HasTrackList() const
{
    return false;
}

QString Mpris::Identity() const
{
    return QLatin1String("Cantata");
}

QString Mpris::DesktopEntry() const
{
#ifdef ENABLE_KDE_SUPPORT
    // Desktop file is installed in $prefix/share/applications/kde4/
    // rather than in $prefix/share/applications. The standard way to
    // represent this dir is with a "kde4-" prefix. See:
    // http://standards.freedesktop.org/menu-spec/1.0/go01.html#term-desktop-file-id
    return QLatin1String("kde4-cantata");
#else
    return QLatin1String("cantata");
#endif
}

QStringList Mpris::SupportedUriSchemes() const
{
    return QStringList();
}

QStringList Mpris::SupportedMimeTypes() const
{
    return QStringList();
}

void Mpris::updateStatus()
{
    qDebug() << "updateStatus";
}

void Mpris::updateCurrentCover(const QString &fileName)
{
    if (fileName != currentCover) {
        currentCover = fileName;
        signalUpdate("Metadata", Metadata());
    }
}

void Mpris::updateCurrentSong()
{
    qDebug() << "updateCurrentSong";
}

QVariantMap Mpris::Metadata() const
{
    QVariantMap metadataMap;
    return metadataMap;
}

int Mpris::Rate() const
{
    return 1.0;
}

void Mpris::SetRate(double)
{
}

bool Mpris::Shuffle()
{
    return false;
}

void Mpris::SetShuffle(bool s)
{
    emit setRandom(s);
}

double Mpris::Volume() const
{
    return 100.0;
}

void Mpris::SetVolume(double v)
{
    emit setVolume(v*100);
}

void Mpris::Raise()
{
    emit showMainWindow();
}

void Mpris::Quit()
{
    QApplication::quit();
}

void Mpris::signalUpdate(const QString &property, const QVariant &value)
{
    QVariantMap map;
    map.insert(property, value);
    signalUpdate(map);
}

void Mpris::signalUpdate(const QVariantMap &map)
{
    if (map.isEmpty())
        return;
    QDBusMessage signal = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
                                                     "org.freedesktop.DBus.Properties",
                                                     "PropertiesChanged");
    QVariantList args = QVariantList()
            << "org.mpris.MediaPlayer2.Player"
            << map
            << QStringList();
    signal.setArguments(args);
    QDBusConnection::sessionBus().send(signal);
}
