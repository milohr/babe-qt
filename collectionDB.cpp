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

#include "collectionDB.h"

#include "database/tracksdb.h"
#include "database/albumsdb.h"
#include "database/artistsdb.h"
#include "database/playlistsdb.h"

#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QList>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <typeinfo>

CollectionDB::CollectionDB(QObject *parent) : QObject(parent)
  ,m_albumsDB(AlbumsDB::instance())
  ,m_artistsDB(ArtistsDB::instance())
  ,m_playlistsDB(PlaylistsDB::instance())
  ,m_tracksDB(TracksDB::instance())
{
}

CollectionDB::~CollectionDB()
{
}

void CollectionDB::setTrackList(const QList<Track> &trackList)
{
    m_trackList = trackList;
}

bool CollectionDB::trackExists(const QVariantMap &data)
{
    return m_tracksDB->trackExists(data.value("location").toString());
}

void CollectionDB::saveAll(const QVariantMap &trackData)
{
    if (!trackExists(trackData))
         m_tracksDB->save(trackData);
    QVariantMap map;
    QString location(BaeUtils::getLocationFromTrackPath(trackData.value("location").toString()));
    map.insert("title", trackData.value("album"));
    map.insert("artist", trackData.value("artist"));
    map.insert("art", trackData.value("art"));
    map.insert("location", location);
    qDebug() << "saving album map: " << map;
    saveAlbum(map, QVariantMap());

    map.clear();
    map.insert("title", trackData.value("artist"));
    map.insert("art", trackData.value("art"));
    map.insert("location", location);
    qDebug() << "saving artist map: " << map;
    saveArtist(map);
}

void CollectionDB::saveTrack(const QMap<int, QString> &trackData)
{
    QVariantMap map;
    map.insert("track", trackData.value(colums::TRACK));
    map.insert("title", trackData.value(colums::TITLE));
    map.insert("artist", trackData.value(colums::ARTIST));
    map.insert("album", trackData.value(colums::ALBUM));
    map.insert("genre", trackData.value(colums::GENRE));
    map.insert("location", trackData.value(colums::LOCATION));
    map.insert("stars", trackData.value(colums::STARS));
    map.insert("babe", trackData.value(colums::BABE));
    map.insert("art", trackData.value(colums::ART));
    map.insert("played", trackData.value(colums::PLAYED));
    map.insert("playlist", trackData.value(colums::PLAYLIST));
    saveAll(map);
}

void CollectionDB::updateTrackPlayed(const QString &location)
{
    QVariantMap track = m_tracksDB->loadTrack(QVariantMap({{"location", location}}));
    if (!track.isEmpty()) {
        int played = track.value("played").toInt();
        updateTrack("played", location, QVariant(played+1));
    }
}

void CollectionDB::updateTrackArt(const QString &path, const QString &artist, const QString &album)
{
    m_tracksDB->update(QVariantMap({{"art", path}}), QVariantMap({{"album", album}, {"artist", artist}}));
}

int CollectionDB::updateTrack(const QString &column, const QString &location, const QVariant &value)
{
    if (column.isEmpty() || location.isEmpty())
        return false;
    return m_tracksDB->update(QVariantMap({{column, value}}), QVariantMap({{"location", location}}));
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QStringList &urls, int limit, int offset, const QString &orderBy, bool descending)
{
    QList<QMap<int, QString>> mapList;
    for (auto url : urls)
        mapList << getTrackData(QVariantMap({{"location", url}}), limit, offset, orderBy, descending);
    return mapList;
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QVariantMap &where, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    QVariantMap map;
    QList<QMap<int, QString>> list;
    QVariantList result = m_tracksDB->loadTracks(where, limit, offset, orderBy, descending, whereOperator, whereComparator);
    if (result.size()) {
        foreach (const QVariant &item, result) {
            map = item.toMap();
            const QMap<int, QString> _track {
                {TRACK, map.value("track").toString()},
                {TITLE, map.value("title").toString()},
                {ARTIST, map.value("artist").toString()},
                {ALBUM, map.value("album").toString()},
                {GENRE, map.value("genre").toString()},
                {LOCATION, map.value("location").toString()},
                {STARS, map.value("stars").toString()},
                {BABE, map.value("babe").toString()},
                {ART, map.value("art").toString()},
                {PLAYED, map.value("played").toString()},
                {PLAYLIST, map.value("playlist").toString()}
            };
            list << _track;
        }
    }
    return list;
}

QVariantList CollectionDB::loadTracks(const QVariantMap &where, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return m_tracksDB->loadTracks(where, limit, offset, orderBy, descending, whereOperator, whereComparator);
}

QList<QString> CollectionDB::albums()
{
    return m_albums;
}

bool CollectionDB::albumExists(const QVariantMap &data)
{
    return m_albumsDB->albumExists(data.value("artist").toString(), data.value("title").toString());
}

void CollectionDB::saveAlbum(const QVariantMap &albumData, const QVariantMap &where)
{
    if (!albumData.value("title").toString().compare("UNKNOWN") && albumData.value("artist").toString().isEmpty())
        return;
    bool _albumExists = albumExists(albumData);
    if (where.isEmpty() && !albumData.isEmpty() && !_albumExists)
        m_albumsDB->save(albumData);
    else if (_albumExists)
        m_albumsDB->update(albumData, where);
}

QString CollectionDB::getAlbumArt(const QString &title, const QString &artist)
{
    QVariantMap where({{"title", title}, {"artist", artist}});
    QVariantMap result(m_artistsDB->loadItem(where, QStringLiteral("AND")));
    if (result.isEmpty())
        return QStringLiteral("");
    return result.value("art").toString();
}

QVariantList CollectionDB::getAlbumsData(const QVariantMap &where, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return m_albumsDB->loadAlbums(where, limit, offset, orderBy, descending, whereOperator, whereComparator);
}

QList<QString> CollectionDB::artists()
{
    return m_artists;
}

bool CollectionDB::artistExists(const QVariantMap &data)
{
    return m_artistsDB->artistExists(data.value("title").toString());
}

void CollectionDB::saveArtist(const QVariantMap &data, const QVariantMap &where)
{
    QStringList titleArtist;
    titleArtist << data.value("title").toString();
    titleArtist << data.value("artist").toString();
    QString unknown("UNKNOWN");
    if (titleArtist.at(0).isEmpty() || (titleArtist.at(0) == unknown && titleArtist.at(1) == unknown))
        return;
    bool exists = artistExists(data);
    if (!exists && !data.isEmpty())
        m_artistsDB->save(data);
    else if (exists && !where.isEmpty())
        m_artistsDB->update(data, where);
}

QString CollectionDB::getArtistArt(const QString &artist)
{
    return m_artistsDB->getArt(artist);
}

QVariantList CollectionDB::getArtistData(const QVariantMap &where, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return m_artistsDB->loadArtists(where, limit, offset, orderBy, descending, whereOperator, whereComparator);
}

QStringList CollectionDB::getPlaylists(const QString &property)
{
    QStringList files;
    QVariantList result = m_playlistsDB->loadPlaylists(QVariantMap());
    foreach (const QVariant &item, result)
        files << item.toMap().value(property).toString();
    if (files.size())
        files.sort();
    return files;
}

QStringList CollectionDB::getPlaylistsMoods()
{
    return getPlaylists("art");
}

bool CollectionDB::addTrack(const QStringList &paths, int babe)
{
    qDebug() << "addTrack...";
    if (paths.isEmpty())
        return false;
    int i = 0;
    for (auto file : paths) {
        qDebug() << "file: " << file;
        TagInfo info(file);
        int track = info.getTrack();
        QString title(BaeUtils::fixString(info.getTitle()));
        QString artist(BaeUtils::fixString(info.getArtist()));
        QString genre(info.getGenre());
        QString album(BaeUtils::fixString(info.getAlbum()));
        if (album.isEmpty()) {
            info.writeData();
            album = BaeUtils::fixString(info.getAlbum());
        }
        QVariantMap trackData;
        trackData.insert("track", track);
        trackData.insert("title", title);
        trackData.insert("artist", artist);
        trackData.insert("album", album);
        trackData.insert("genre", genre);
        trackData.insert("location", file);
        trackData.insert("stars", 0);
        trackData.insert("babe", babe);
        trackData.insert("art", "");
        trackData.insert("played", 0);
        saveAll(trackData);

        if (!m_albums.contains(artist + " " + album))
            m_albums << artist + " " + album;
        if (!m_artists.contains(artist))
            m_artists << artist;

        emit progress((i++)+1);
    }
    emit dbActionFinished((i>0));
    return (i > 0);
}

void CollectionDB::removeTrack(const QString &path)
{
    if (path.isEmpty())
        return;
    bool success = m_tracksDB->remove(QVariantMap({{"location", path}}), "LIKE") > 0;
    emit dbActionFinished(success);
}

void CollectionDB::insertPlaylist(const QString &name)
{
    if (!name.isEmpty())
        m_playlistsDB->save(QVariantMap({{"title", name}, {"art", ""}}));
}

void CollectionDB::setCollectionLists()
{
    m_albums.clear();
    m_artists.clear();
    QVariantMap map;
    QString artist, album;
    foreach (const QVariant &item, m_tracksDB->loadTracks(QVariantMap())) {
        map = item.toMap();
        artist = map.value("artist").toString();
        album = map.value("album").toString();
        if (artist.isEmpty() && album.isEmpty())
            continue;
        if (!m_albums.contains(artist + " " + album))
            m_albums << artist + " " + album;
        if (!m_artists.contains(artist))
            m_artists << artist;
    }
}

void CollectionDB::refreshArtistsTable()
{
    QString artist, location;
    QVariantMap map, artistsMap;
    QVariantList result = m_tracksDB->loadTracks(QVariantMap());
    foreach (const QVariant &item, result) {
        map = item.toMap();
        artist = map.value("artist").toString();
        location = map.value("location").toString();
        if (!m_artists.contains(artist)) {
            artistsMap.insert("art", "");
            artistsMap.insert("title", artist);
            artistsMap.insert("location", QFileInfo(location).dir().path());
            saveArtist(artistsMap);
            m_artists << artist;
        }
        map.clear();
        artistsMap.clear();
    }
}

void CollectionDB::cleanCollectionLists()
{
    QString artistTitle;
    QVariantMap map;
    foreach (const QVariant &item, m_artistsDB->loadArtists(QVariantMap())) {
        map = item.toMap();
        artistTitle = map.value("title").toString();
        if (artistTitle.isEmpty() || m_artists.contains(artistTitle))
            continue;
        m_artistsDB->remove(QVariantMap({{"title", artistTitle}}));
    }
    map.clear();

    QList<QString> albumsTemp;
    foreach (const QString &s, m_albums)
        albumsTemp.append(s);

    QString albumTitle;
    foreach (const QVariant &album, m_albumsDB->loadAlbums(QVariantMap())) {
        map = album.toMap();
        albumTitle = map.value("artist").toString();
        if (albumTitle.isEmpty() || albumsTemp.contains(albumTitle))
            continue;
        m_albumsDB->remove(QVariantMap({{"title", albumTitle}}));
    }
}

void CollectionDB::insertCoverArt(const QString &path, const QStringList &info)
{
    if (info.size() == 2) {
        QVariantMap albumData({{"title", info[0]}, {"artist", info[1]}});
        int result = m_albumsDB->update(albumData, QVariantMap({{"art", path}}));
        if (result) {
            QVariantMap trackDB = m_tracksDB->loadTrack(albumData);
            updateTrackArt(path, trackDB.value("artist").toString(), trackDB.value("album").toString());
            if (!m_albums.contains(info[0]))
                m_albums << info[1] + " " + info[0];
        }
    }
}

void CollectionDB::insertHeadArt(const QString &path, const QStringList &info)
{
    if (info.size() == 1) {
        if (m_artistsDB->update(QVariantMap({{"art", path}}), QVariantMap({{"title", info[0]}}))) {
            if (!m_artists.contains(info[0]))
                m_artists << info[0];
        }
    }
}
