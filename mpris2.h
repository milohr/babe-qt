/*
 * Cantata
 *
 * Copyright (c) 2011-2016 Craig Drummond <craig.p.drummond@gmail.com>
 *
 * ----
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MPRIS2_H
#define MPRIS2_H

#include <QObject>
#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include <QApplication>
#include <QDebug>

class QDBusObjectPath;

class Mpris : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double Rate READ Rate WRITE SetRate)
    Q_PROPERTY(qlonglong Position READ Position)
    Q_PROPERTY(double MinimumRate READ MinimumRate)
    Q_PROPERTY(double MaximumRate READ MaximumRate)
    Q_PROPERTY(bool CanControl READ CanControl)
    Q_PROPERTY(bool CanPlay READ CanPlay)
    Q_PROPERTY(bool CanPause READ CanPause)
    Q_PROPERTY(bool CanSeek READ CanSeek)
    Q_PROPERTY(bool CanGoNext READ CanGoNext)
    Q_PROPERTY(bool CanGoPrevious READ CanGoPrevious)
    Q_PROPERTY(QString PlaybackStatus READ PlaybackStatus)
    Q_PROPERTY(QString LoopStatus READ LoopStatus WRITE SetLoopStatus)
    Q_PROPERTY(bool Shuffle READ Shuffle WRITE SetShuffle)
    Q_PROPERTY(QVariantMap Metadata READ Metadata)
    Q_PROPERTY(double Volume READ Volume WRITE SetVolume)
    Q_PROPERTY(bool CanQuit READ CanQuit)
    Q_PROPERTY(bool CanRaise READ CanRaise)
    Q_PROPERTY(QString DesktopEntry READ DesktopEntry)
    Q_PROPERTY(bool HasTrackList READ HasTrackList)
    Q_PROPERTY(QString Identity READ Identity)
    Q_PROPERTY(QStringList SupportedMimeTypes READ SupportedMimeTypes)
    Q_PROPERTY(QStringList SupportedUriSchemes READ SupportedUriSchemes)

public:
    Mpris(QObject *parent = 0);
    virtual ~Mpris();
    void Next();
    void Previous();
    void Pause();
    void PlayPause();
    void Stop();
    void StopAfterCurrent();
    void Play();
    void Seek(qlonglong pos);
    void SetPosition(const QDBusObjectPath &, qlonglong pos);
    void OpenUri(const QString &);
    QString PlaybackStatus() const;
    QString LoopStatus();
    void SetLoopStatus(const QString &s);
    QVariantMap Metadata() const;
    int Rate() const;
    void SetRate(double);
    bool Shuffle();
    void SetShuffle(bool s);
    double Volume() const;
    void SetVolume(double v);
    qlonglong Position() const;
    double MinimumRate() const;
    double MaximumRate() const;
    bool CanControl() const;
    bool CanPlay() const;
    bool CanPause() const;
    bool CanSeek() const;
    bool CanGoNext() const;
    bool CanGoPrevious() const;

    // org.mpris.MediaPlayer2
    bool CanQuit() const;
    bool CanRaise() const;
    bool HasTrackList() const;
    QString Identity() const;
    QString DesktopEntry() const;
    QStringList SupportedUriSchemes() const;
    QStringList SupportedMimeTypes() const;

public:
    void updateCurrentSong();

public Q_SLOTS:
    void Raise();
    void Quit();

Q_SIGNALS:
    void setRandom(bool toggle);
    void setRepeat(bool toggle);
    void setSeekId(qint32 songId, quint32 time);
    void setVolume(int vol);
    void showMainWindow();

public Q_SLOTS:
    void updateCurrentCover(const QString &fileName);

private Q_SLOTS:
    void updateStatus();

private:
    void signalUpdate(const QString &property, const QVariant &value);
    void signalUpdate(const QVariantMap &map);

private:
    QString currentCover;
    int pos;
};

#endif // MPRIS2_H
