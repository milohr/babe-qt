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

CollectionDB::CollectionDB(QObject *parent) : QObject(parent)
  ,m_albumsdb(AlbumsDB::instance())
  ,m_artistsDB(ArtistsDB::instance())
  ,m_tracksDB(TracksDB::instance())
  ,m_playlistsdb(PlaylistsDB::instance())
{
}

void CollectionDB::removePath(const QString &path)
{
    if (path.isEmpty())
        return;
    QVariantMap where;
    where.insert("location", path);
    bool success = m_tracksDB->remove(where, "LIKE") > 0;
    emit dbActionFinished(success);
    if (!success)
        qDebug() << "removePerson error: ";
}

QString CollectionDB::getArtistArt(const QString &artist)
{
    QString artistHead;
    QSqlQuery queryHead("SELECT * FROM artists WHERE title = \""+artist+"\"");
    while (queryHead.next())
        if(!queryHead.value(1).toString().isEmpty()&&queryHead.value(1).toString()!="NULL")
            artistHead = queryHead.value(1).toString();
    return artistHead;
}

QString CollectionDB::getAlbumArt(const QString &album, const QString &artist)
{
    QString albumCover;
    QSqlQuery queryCover("SELECT * FROM albums WHERE title = \""+album+"\" AND artist = \""+artist+"\"");
    while (queryCover.next())
        if (!queryCover.value(2).toString().isEmpty()&&queryCover.value(2).toString() != "NULL")
            albumCover = queryCover.value(2).toString();
    return albumCover;
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QStringList &urls)
{
    QList<QMap<int, QString>> mapList;
    for (auto url : urls) {
        QSqlQuery query("SELECT * FROM tracks WHERE location =\"" + url + "\"");
        if (query.exec()) {
            while (query.next()) {
                QString track = query.value(TRACK).toString();
                QString title = query.value(TITLE).toString();
                QString artist = query.value(ARTIST).toString();
                QString album = query.value(ALBUM).toString();
                QString genre = query.value(GENRE).toString();
                QString location = query.value(LOCATION).toString();
                QString stars = query.value(STARS).toString();
                QString babe = query.value(BABE).toString();
                QString art = query.value(ART).toString();
                QString playlist = query.value(PLAYLIST).toString();
                QString played = query.value(PLAYED).toString();
                const QMap<int, QString> map{{TRACK,track}, {TITLE,title}, {ARTIST,artist},{ALBUM,album},{GENRE,genre},{LOCATION,location},{STARS,stars},{BABE,babe},{ART,art},{PLAYED,played},{PLAYLIST,playlist}};
                mapList<<map;
            }
        }
    }
    return mapList;
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QString &queryText)
{
    QList<QMap<int, QString>> mapList;
    QSqlQuery query;
    query.prepare(queryText);
    if (query.exec()) {
        while(query.next()) {
            QString track = query.value(TRACK).toString();
            QString title = query.value(TITLE).toString();
            QString artist = query.value(ARTIST).toString();
            QString album = query.value(ALBUM).toString();
            QString genre = query.value(GENRE).toString();
            QString location = query.value(LOCATION).toString();
            QString stars = query.value(STARS).toString();
            QString babe = query.value(BABE).toString();
            QString art = query.value(ART).toString();
            QString playlist = query.value(PLAYLIST).toString();
            QString played = query.value(PLAYED).toString();
            const QMap<int, QString> map{{TRACK, track}, {TITLE, title}, {ARTIST, artist}, {ALBUM, album}, {GENRE, genre}, {LOCATION, location}, {STARS, stars}, {BABE, babe}, {ART, art}, {PLAYED, played}, {PLAYLIST, playlist}};
            mapList << map;
        }
    }
    return mapList;
}

void CollectionDB::cleanCollectionLists()
{
    QSqlQuery queryArtists("SELECT * FROM artists");
    if (queryArtists.exec()) {
        while (queryArtists.next()) {
            QString oldArtists = queryArtists.value(0).toString();
            if (oldArtists.isEmpty())
                continue;
            qDebug() << "oldArtists: " << oldArtists;
            qDebug() << "artists size: " << artists.size();
            if (artists.contains(oldArtists)) {
                continue;
            } else {
                qDebug() << "artists list does not longer contains: " << oldArtists;
                QSqlQuery queryArtist_delete;
                queryArtist_delete.prepare("DELETE FROM artists  WHERE title = \"" + oldArtists + "\"");
                if (queryArtist_delete.exec())
                    qDebug() << "deleted missing artist";
            }
        }
    }

    QList<QString> albumsTemp;
    foreach (const QString &s, albums)
        albumsTemp.append(s);
    QSqlQuery queryAlbums("SELECT * FROM albums");
    if (queryAlbums.exec()) {
        while (queryAlbums.next()) {
            QString oldAlbum = queryAlbums.value(1).toString()+" "+queryAlbums.value(0).toString();
            if (oldAlbum.isEmpty())
                continue;
            if (albumsTemp.contains(oldAlbum)) {
                continue;
            } else {
                qDebug() << "albums list does not longer contains: " << oldAlbum;
                QSqlQuery queryAlbum_delete;
                queryAlbum_delete.prepare("DELETE FROM albums WHERE title = \"" + queryAlbums.value(0).toString() + "\"");
                if (queryAlbum_delete.exec())
                    qDebug() << "deleted missing album";
            }
        }
    }
}

QSqlQuery CollectionDB::getQuery(const QString &queryTxt)
{
    QSqlQuery query(queryTxt);
    return query;
}

bool CollectionDB::removeQuery(const QString &queryTxt)
{
    QSqlQuery query;
    query.prepare(queryTxt);
    if (!query.exec()) {
        qDebug() << "removeQuery error: " << query.lastError();
        return false;
    }
    return true;
}

bool CollectionDB::checkQuery(const QString &queryTxt)
{
    QSqlQuery query(queryTxt);
    qDebug() << "The Query is: " << queryTxt;
    if (query.exec())
        return query.next();
    return false;
}

void CollectionDB::setCollectionLists()
{
    albums.clear();
    artists.clear();
    QSqlQuery query("SELECT * FROM tracks");
    while (query.next()) {
        QString artist = query.value(ARTIST).toString();
        QString album = query.value(ALBUM).toString();
        if (artist.isEmpty() && album.isEmpty())
            continue;
        if (!albums.contains(artist + " " + album))
            albums << artist + " " + album;
        if (!artists.contains(artist))
            artists << artist;
    }
}

void CollectionDB::refreshArtistsTable()
{
    QVariantMap map;
    QVariantList result = m_tracksDB->loadTracks(QVariantMap());
    foreach (const QVariant &item, result) {
        map = item.toMap();
        QString artist = map.value("artist").toString();
        QString location = map.value("location").toString();
        if (!artists.contains(artist)) {
            QVariantMap artistsMap;
            artistsMap.insert("title", artist);
            artistsMap.insert("art", "");
            artistsMap.insert("location", QFileInfo(location).dir().path());
            if (m_artistsDB->save(artistsMap))
                artists << artist;
        }
    }
}

bool CollectionDB::addTrack(const QStringList &paths, int babe)
{
    bool success = false;
    if (paths.isEmpty())
        return false;
    QSqlQuery query;
    qDebug() << "call to addTrack....";
    success = true;
    int i = 0;
    for (auto file : paths) {
        qDebug() << file;
        TagInfo info(file);
        QString album;
        int track;
        QString title = BaeUtils::fixString(info.getTitle());
        QString artist = BaeUtils::fixString(info.getArtist());
        QString genre = info.getGenre();
        if (info.getAlbum().isEmpty()) {
            qDebug() << "the album has not title, so i'm going to try and get it.";
            info.writeData();
        }
        track = info.getTrack();
        album = info.getAlbum();
        album = BaeUtils::fixString(album);
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
        int result = m_tracksDB->save(trackData);
        if (result > 0)
            qDebug() << "result ok!!!!!!!!!!!!";
        else
            qDebug() << "result fail!!!!!!!!!!";
        if (result > 0) {
            success = true;
            qDebug() << "writting to db: " << title;
            if (!albums.contains(artist+" "+album)) {
                query.prepare("INSERT INTO albums (title, artist, art, location)" "VALUES (:title, :artist, :art, :location)");
                query.bindValue(":title", album);
                query.bindValue(":artist", artist);
                query.bindValue(":art", "");
                if (query.exec()) {
                    albums << artist + " " + album;
                    success = true;
                } else {
                    return false;
                }
            }
            if (!artists.contains(artist)) {
                query.prepare("INSERT INTO artists (title, art, location)" "VALUES (:title, :art, :location)");
                query.bindValue(":title", artist);
                query.bindValue(":art", "");
                if (query.exec())
                    artists << artist;
            }
            emit progress((i++)+1);
        } else {
            qDebug() << "adding track error:  " << query.lastError() << info.getTitle();
        }
    }
    qDebug() << "finished writing to database";
    emit dbActionFinished(true);
    return success;
    qDebug() << "addTrack fail!";
    return false;
}

void CollectionDB::insertCoverArt(const QString &path, const QStringList &info)
{
    qDebug() << "the path:" << path << "the list:" << info.at(0) << info.at(1);
    if (info.size() == 2) {
        QSqlQuery query;
        query.prepare("UPDATE albums SET art = (:art) WHERE title = (:title) AND artist = (:artist)");
        query.bindValue(":art",  path.isEmpty() ? "NULL" : path);
        query.bindValue(":title", info.at(0));
        query.bindValue(":artist", info.at(1));
        if (query.exec()) {
            qDebug() << "Artwork[cover] inserted into DB" << info.at(0) << info.at(1);
            if (!albums.contains(info.at(0)))
                albums << info.at(1) + " " + info.at(0);
        } else {
            qDebug() << "COULDNT Artwork[cover] inerted into DB" << info.at(0) << info.at(1);
        }
    }
}

void CollectionDB::insertHeadArt(const QString &path, const QStringList &info)
{
    if (info.size() == 1) {
        QSqlQuery query;
        query.prepare("UPDATE artists SET art = (:art) WHERE title = (:title)");
        query.bindValue(":art", path.isEmpty() ? "NULL" : path);
        query.bindValue(":title", info.at(0));
        if  (query.exec()) {
            qDebug() << "Artwork[head] inerted into DB" << info.at(0);
            if (!artists.contains(info.at(0)))
                artists << info.at(0);
        }
    }
}

void CollectionDB::setTrackList(const QList<Track> &trackList)
{
    this->trackList = trackList;
}

bool CollectionDB::check_existance(const QString &tableName, const QString &searchId, const QString &search)
{
    QSqlQuery query;
    query.prepare("SELECT "+ searchId + " FROM " + tableName + " WHERE " + searchId + " = (:search)");
    query.bindValue(":search", search);
    if (query.exec()) {
        if (query.next()) {
            qDebug() << "it exists";
            return true;
        } else {
            qDebug() << "currnt song doesn't exists in db";
            return false;
        }
    }
    return false;
}

bool CollectionDB::execQuery(const QString &queryTxt)
{
    QSqlQuery query;
    query.prepare(queryTxt);
    if (query.exec()) {
        qDebug() << "executing query: " << queryTxt;
        return true;
    }
    return false;
}

bool CollectionDB::insertInto(const QString &tableName, const QString &column, const QString &location, int value)
{
    QSqlQuery query;
    if (query.exec("PRAGMA synchronous=OFF")) {
        query.prepare("UPDATE "+tableName+" SET "+column+" = (:value) WHERE location = (:location)" );
        query.bindValue(":value", value);
        query.bindValue(":location", location);
        if (query.exec()) {
            qDebug() << "insertInto<<" << "UPDATE " + tableName + " SET " + column + " = " + value + " WHERE location = " + location;
            return true;
        }
    }
    return false;
}

bool CollectionDB::insertInto(const QString &tableName, const QString &column, const QString &location, const QString &value)
{
    QSqlQuery query;
    query.prepare("UPDATE " + tableName + " SET " + column + " = (:value) WHERE location = (:location)");
    query.bindValue(":value", value);
    query.bindValue(":location", location);
    if (query.exec()) {
        qDebug() << "insertInto << " << " UPDATE " + tableName + " SET " + column + " = " + value + " WHERE location = " + location;
        return true;
    }
    return false;
}

void CollectionDB::insertPlaylist(const QString &name)
{
    if (!name.isEmpty()) {
        QVariantMap playlistMap;
        playlistMap.insert("title", name);
        PlaylistsDB *playlistDB = PlaylistsDB::instance();
        if (playlistDB->save(playlistMap))
            qDebug() << "playlist " << name << " saved!";
    }
}

QStringList CollectionDB::getPlaylists()
{
    QStringList files;
    PlaylistsDB *playlistDB = PlaylistsDB::instance();
    QVariantList result = playlistDB->loadPlaylists(QVariantMap());
    qDebug() << "playlist result size: " << result.size();
    foreach (const QVariant &item, result)
        files << item.toString();
    return files;
}

QStringList CollectionDB::getPlaylistsMoods()
{
    QStringList moods;
    QSqlQuery query;
    query.prepare("SELECT * FROM playlists order by title");
    if (query.exec()) {
        while (query.next()) {
            if (!query.value(1).toString().isEmpty() && query.value(0).toString().contains("mood"))
                moods << query.value(1).toString();
        }
    }
    return moods;
}
