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

#include "playlist.h"

#include <QDebug>
#include <QStringList>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QString>

Playlist::Playlist(QObject *parent) : QObject(parent)
{
}

bool Playlist::isMusic(const QString &file)
{
    QMimeDatabase mimeDatabase;
    QMimeType mimeType(mimeDatabase.mimeTypeForFile(QFileInfo(file)));

    QStringList audioFileTypes;
    audioFileTypes << "audio/flac"
                   << "audio/mp4"
                   << "audio/m4a"
                   << "audio/mpeg"
                   << "audio/MPEG-4"
                   << "audio/mp3"
                   << "audio/ogg"
                   << "audio/wav"
                   << "video/mp4"
                   << "video/mpeg";

    foreach (const QString &str, audioFileTypes)
        if (mimeType.inherits(str))
            return true;
    return false;
}

QList<QMap<int, QString>> Playlist::getTracksData()
{
    QList<QMap<int, QString>> mapList;
    for (auto file : getTracks()) {
        const QMap<int, QString> map{
            {BabeTable::TRACK, QString::number(file.getTrack())},
            {BabeTable::TITLE, file.getTitle()},
            {BabeTable::ARTIST, file.getArtist()},
            {BabeTable::ALBUM, file.getAlbum()},
            {BabeTable::GENRE, file.getGenre()},
            {BabeTable::LOCATION, file.getLocation()},
            {BabeTable::STARS, ""},
            {BabeTable::BABE, ""},
            {BabeTable::ART, ""},
            {BabeTable::PLAYED, ""},
            {BabeTable::PLAYLIST, ""}
        };
        mapList << map;
    }
    return mapList;
}

void Playlist::add(QStringList files)
{
    foreach (const QString &file, files) {
        if (isMusic(file)) {
            Track track;
            TagInfo info(file);

            QString title(info.getTitle());
            QString artist(info.getArtist());
            QString album(info.getAlbum());

            title = title.size() > 0 ? title : info.fileName();
            artist = artist.size() > 0 ? artist : "UNKNOWN";
            album = album.size() > 0 ? album : "UNKNOWN";

            track.setTitle(title);
            track.setArtist(artist);
            track.setAlbum(album);
            track.setLocation(file);
            track.setGenre(info.getGenre());
            track.setTrack(info.getTrack());
            track.setArtwork("");
            track.setName(info.fileName());
            tracks.push_back(track);
        }
    }
}

void Playlist::addClean(QStringList files)
{
    foreach (const QString &file, files) {
        Track track;
        TagInfo info(file);
        int track_n = info.getTrack();
        QString title(info.getTitle());
        QString artist(info.getArtist());
        QString album(info.getAlbum());
        QString artwork(""); // here needs to get the artwork;
        QString genre(info.getGenre());

        title = title.size() > 0 ? title : info.fileName();
        artist = artist.size() > 0 ? artist : "UNKNOWN";
        album = album.size() > 0 ? album : "UNKNOWN";

        track.setTitle(title);
        track.setArtist(artist);
        track.setAlbum(album);
        track.setLocation(file);
        track.setGenre(genre);
        track.setTrack(track_n);
        track.setArtwork(artwork);
        track.setName(info.fileName());
        tracks.push_back(track);
    }
}

void Playlist::setMetaData(const QString &title, const QString &artist, const QString &album, const QString &location)
{
    Track track;
    track.setTitle(title);
    track.setArtist(artist);
    track.setAlbum(album);
    track.setLocation(location);
    tracks.push_back(track);
}

void Playlist::remove(int index)
{
    tracks.erase(tracks.begin() + index);
}

void Playlist::removeAll()
{
    tracks.clear();
}

QStringList Playlist::getTracksNameList()
{
    QStringList list;
    for (auto t : tracks)
        list << t.getTitle() + "\nby " + t.getArtist();
    return list;
}

QStringList Playlist::getTracksNameListSimple()
{
    QStringList list;
    for (auto t : tracks)
        list << t.getTitle();
    return list;
}

QList<Track> Playlist::getTracks()
{
    QList<Track> list;
    for (auto t : tracks)
        list.push_back(t);
    return list;

}

QStringList Playlist::getList()
{
    QStringList list;
    for (auto t : tracks)
        list << t.getLocation();
    return list;
}
