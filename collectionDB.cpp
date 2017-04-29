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
#include <QMapIterator>
#include <QSqlQuery>
#include <QDebug>
#include <QFileInfo>

CollectionDB::CollectionDB(QObject *parent) : QObject(parent)
    ,m_database(Database::instance())
{
    connect(m_database, SIGNAL(logMessage(QString)), this, SLOT(debugDatabaseMessage(QString)));
}

void CollectionDB::removePath(QString path)
{
    QVariantMap where;
    where.insert("location", path);
    if (!m_database->remove("tracks", where, "LIKE"))
        qDebug() << "removePath error!";
    emit DBactionFinished(false);
}

QString CollectionDB::getArtistArt(QString artist)
{
    QVariantMap where;
    where.insert("title", artist);
    QVariantList result = m_database->select("artists", where);

    if (result.isEmpty())
        return QStringLiteral("");
    return result.at(0).toMap().value("art").toString();
}

QString CollectionDB::getAlbumArt(QString album, QString artist)
{
    QVariantMap where;
    where.insert("title", album);
    where.insert("artist", artist);
    QVariantList result = m_database->select("albums", where);

    if (result.isEmpty())
        return QStringLiteral("");
    return result.at(0).toMap().value("art").toString();
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QString &url)
{
    QVariantMap where;
    where.clear();
    where.insert("location", url);
    return getTrackData(where);
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QVariantMap &filter, const QString &orderBy, const QString &whereOperator, const QString &whereComparator)
{
    QList<QMap<int, QString>> mapList;
    QVariantList resultSet = m_database->select("tracks", filter, -1, 0, orderBy, false, Database::SELECT_TYPE::All_Itens_Int, whereOperator, whereComparator);
    int count = 0;
    foreach (const QVariant &set, resultSet) {
        count = 0;
        QMap<int, QString> map;
        QMapIterator<QString, QVariant> entry(set.toMap());
        while (entry.hasNext()) {
            entry.next();
            map.insert(count++, entry.value().toString());
        }
        qDebug() << "map: " << map;
        mapList << map;
    }
    return mapList;
}

void CollectionDB::cleanCollectionLists()
{
    QVariantMap where;
    QString title;
    foreach (const QVariant &entry, m_database->select("artists")) {
        title = entry.toMap().value("title").toString();
        if (artists.contains(title))
            continue;
        where.insert("title", title);
        m_database->remove("artists", where);
        where.clear();
    }

    QString oldAlbum;
    QVariantMap albumMap;
    foreach (const QVariant &entry, m_database->select("albums")) {
        albumMap = entry.toMap();
        oldAlbum = albumMap.value("artist").toString() + albumMap.value("title").toString();
        if (albums.contains(oldAlbum))
            continue;
        where.insert("title", albumMap.value("title"));
        m_database->remove("albums", where);
        where.clear();
    }
}

QSqlQuery CollectionDB::getQuery(const QString &queryTxt)
{
    return m_database->qsqlQuery(queryTxt);
}

bool CollectionDB::checkQuery(QString queryTxt)
{
    return m_database->queryExec(queryTxt);
}

void CollectionDB::setCollectionLists()
{
    albums.clear();
    artists.clear();
    QString album;
    QString artist;
    QVariantMap map;
    foreach (const QVariant &entry, m_database->select("tracks")) {
        map = entry.toMap();
        artist = map.value("artist").toString();
        album = map.value("album").toString();
        if (!albums.contains(artist + " " + album))
            albums << artist + " " + album;
        if (!artists.contains(artist))
            artists << artist;
    }
}

void CollectionDB::refreshArtistsTable()
{
    QString artist;
    QVariantMap map;
    foreach (const QVariant &entry, m_database->select("tracks")) {
        map = entry.toMap();
        artist = map.value("artist").toString();
        QVariantMap insertMap;
        insertMap.insert("title", artist);
        insertMap.insert("art", "");
        insertMap.insert("location", QFileInfo(map.value("location").toString()).dir().path());
        if (!artists.contains(artist) && m_database->insert("artists", insertMap))
            artists << artist;
    }
}

bool CollectionDB::addTrack(QStringList paths, int babe)
{
    if (paths.isEmpty())
        return false;
    bool success = false;
    int i = 0;
    qDebug() << "started writing to database...";
    for (auto file : paths) {
        qDebug() << file;
        TagInfo info(file);
        int track;
        QString title, artist, album, genre;
        track = info.getTrack();
        genre = info.getGenre();
        album = info.getAlbum();
        title = BaeUtils::fixString(info.getTitle());
        artist = BaeUtils::fixString(info.getArtist());

        if (album.isEmpty()) {
            qDebug() << "the album has not title, so i'm going to try and get it.";
            info.writeData();
            album = info.getAlbum();
        }

        album = BaeUtils::fixString(album);

        QVariantMap insertMap;
        insertMap.insert("track", track);
        insertMap.insert("title", title);
        insertMap.insert("artist", artist);
        insertMap.insert("album", album);
        insertMap.insert("genre", genre);
        insertMap.insert("location", file);
        insertMap.insert("stars", 0);
        insertMap.insert("babe", babe);
        insertMap.insert("art", "");
        insertMap.insert("played", 0);

        if (m_database->insert("tracks", insertMap) > 0) {
            success = true;
            qDebug() << "writting to db: " << title;
            if (!albums.contains(artist + " " + album)) {
                insertMap.clear();
                insertMap.insert("title", album);
                insertMap.insert("artist", artist);
                insertMap.insert("art", "");
                if (m_database->insert("albums", insertMap) > 0) {
                    albums << artist + " " + album;
                    success = true;
                } else {
                    qDebug() << "can't save data in database!";
                    return false;
                }
            }
            if (!artists.contains(artist)) {
                insertMap.clear();
                insertMap.insert("title", artist);
                insertMap.insert("art", "");
                if (m_database->insert("artists", insertMap))
                    artists << artist;
            }
            emit progress((i++)+1);
        } else {
            qDebug() << "can't save data in database! " << info.getTitle();
        }
    }
    qDebug() << "finished writing to database! Status: " << success;
    emit DBactionFinished(true);
    return success;
}

void CollectionDB::insertCoverArt(const QString &art, const QStringList &info)
{
    if (info.size() < 2)
        return;
    QVariantMap updateMap;
    updateMap.insert("art", art);
    QVariantMap where;
    where.insert("title", info.at(0));
    where.insert("artist", info.at(1));
    if (m_database->update("albums", updateMap, where) > 0)
        if (!albums.contains(info.at(0)))
            albums << info.at(1) + " " + info.at(0);
}

void CollectionDB::insertHeadArt(const QString &art, const QStringList &info)
{
    if (info.size() <= 0)
        return;
    QVariantMap updateMap;
    updateMap.insert("art", art);
    QVariantMap where;
    where.insert("title", info.at(0));
    if (m_database->update("artists", updateMap, where))
        if (!artists.contains(info.at(0)))
            artists << info.at(0);
}

bool CollectionDB::hasTrack(const QString &searchId, const QString &searchTerm)
{
    QVariantMap where;
    where.insert(searchId, searchTerm);
    QVariantList resultSet = m_database->select("tracks", where);
    if (!resultSet.isEmpty())
        return resultSet.at(0).toMap().isEmpty();
    return false;
}

bool CollectionDB::execQuery(QString queryTxt)
{
    return m_database->queryExec(queryTxt);
}

bool CollectionDB::updateTrack(const QString &column, const QString &location, const QVariant &value)
{
    QVariantMap update;
    update.insert(column, value);
    QVariantMap where;
    where.insert("location", location);
    return m_database->update("tracks", update, where) > 0;
}

void CollectionDB::insertPlaylist(const QString &name, const QString &color)
{
    QVariantMap map;
    map.insert("title", name);
    map.insert("art", color);
    m_database->insert("playlists", map);
}

QStringList CollectionDB::getPlaylists(int limit, int offset, const QString &orderBy)
{
    QStringList playlists;
    QVariantList resultSet = m_database->select("playlists", QVariantMap(), limit, offset, orderBy);
    foreach (const QVariant &entry, resultSet) {
        QString title = entry.toMap().value("title").toString();
        if (!title.contains("mood") && !title.isEmpty())
            playlists << entry.toMap().value("title").toString();
    }
    return playlists;
}

QStringList CollectionDB::getPlaylistsMoods()
{
    return getPlaylists(-1, 0);
}

void CollectionDB::debugDatabaseMessage(const QString &message)
{
    qDebug() << message;
}
