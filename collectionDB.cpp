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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>

CollectionDB::CollectionDB(QObject *parent) : QObject(parent)
    ,m_database(Database::instance())
{
}

void CollectionDB::removePath(QString path)
{
    qDebug() << "trying to delete all from :"<< path;
    QVariantMap where;
    where.insert("location", path);
    int result = m_database->remove("tracks", where, "LIKE");

    emit DBactionFinished(false);
    if (!result)
        qDebug() << "removePath error!";
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

QList<QMap<int, QString>> CollectionDB::getTrackData(QStringList urls)
{
    QVariantMap where;
    QList<QMap<int, QString>> mapList;
    for (auto url : urls) {
        where.clear();
        where.insert("location", url);
        mapList.append(getTrackData(where).at(0));
    }
    return mapList;
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QVariantMap &filter, const QString &orderBy, const QString &whereOperator)
{
    QList<QMap<int, QString>> mapList;
    QVariantList resultSet = m_database->select("tracks", filter, -1, 0, orderBy, false, Database::SELECT_TYPE::All_Itens_Int, whereOperator);
    int count = 0;
    foreach (const QVariant &set, resultSet) {
        count = 0;
        QMap<int, QString> map;
        QMapIterator<QString, QVariant> entry(set.toMap());
        while (entry.hasNext()) {
            entry.next();
            map.insert(count++, entry.value().toString());
        }
        mapList << map;
    }
    return mapList;
}

void CollectionDB::cleanCollectionLists()
{
    QSqlQuery queryArtists("SELECT * FROM artists");
    if(queryArtists.exec())
    {
        while(queryArtists.next())
        {
            QString  oldArtists = queryArtists.value(0).toString();
            if(artists.contains(oldArtists))
                continue;
            else
            {
                qDebug()<<"artists list does not longer contains: "<<oldArtists;
                QSqlQuery queryArtist_delete;
                queryArtist_delete.prepare("DELETE FROM artists  WHERE title = \""+oldArtists+"\"");
                if(queryArtist_delete.exec()) qDebug()<<"deleted missing artist";
            }
        }
    }

    QSqlQuery queryAlbums("SELECT * FROM albums");
    if(queryAlbums.exec())
    {
        while(queryAlbums.next())
        {
            QString  oldAlbum = queryAlbums.value(1).toString()+" "+queryAlbums.value(0).toString();
            if(albums.contains(oldAlbum))
                continue;
            else
            {
                qDebug()<<"albums list does not longer contains: "<<oldAlbum;
                QSqlQuery queryAlbum_delete;
                queryAlbum_delete.prepare("DELETE FROM albums  WHERE title = \""+queryAlbums.value(0).toString()+"\"");
                if(queryAlbum_delete.exec()) qDebug()<<"deleted missing album";
            }
        }
    }
}

QSqlQuery CollectionDB::getQuery(QString queryTxt)
{
    QSqlQuery query(queryTxt);
    return query;
}

bool CollectionDB::checkQuery(QString queryTxt)
{
    QSqlQuery query(queryTxt);

    qDebug()<<"The Query is: "<<queryTxt;

    if (query.exec())
        if (query.next()) return true;
        else return false;
    else return false;
}

void CollectionDB::setCollectionLists()
{
    albums.clear(); artists.clear();
    QSqlQuery query ("SELECT * FROM tracks");
    while (query.next())
    {
        QString artist = query.value(ARTIST).toString();
        QString album = query.value(ALBUM).toString();
        //QString file = query.value(LOCATION).toString();

        if(!albums.contains(artist+" "+album)) albums<<artist+" "+album;
        if(!artists.contains(artist)) artists<<artist;
    }
    // refreshArtistsTable();
    /*qDebug()<<"artist in collection list::";
    for(auto artist:artists)qDebug()<<artist;
    qDebug()<<"albums in collection list::";
    for(auto album:albums)qDebug()<<album;*/
}

void CollectionDB::refreshArtistsTable()
{
    QSqlQuery query ("SELECT * FROM tracks");

    qDebug()<<"updating artists table";

    if(query.exec())
    {
        while(query.next())
        {
            //success = true;
            QString artist = query.value(ARTIST).toString();
            QString file = query.value(LOCATION).toString();
            if(!artists.contains(artist))
            {
                query.prepare("INSERT INTO artists (title, art, location)" "VALUES (:title, :art, :location)");
                query.bindValue(":title", artist);
                query.bindValue(":art", "");
                query.bindValue(":location", QFileInfo(file).dir().path());
                if(query.exec()) artists<<artist;
            }
        }
    }
}

bool CollectionDB::addTrack(QStringList paths, int babe)
{
    bool success = false;
    if (paths.isEmpty()) return false;
    QSqlQuery query;
    if (query.exec("PRAGMA synchronous=OFF")) {
        success=true;
        int i=0;
        qDebug()<<"started writing to database...";
        for (auto file:paths) {
            qDebug()<<file;
            TagInfo info(file);

            int track;
            QString  title, artist, album, genre;
            // you should check if args are ok first...
            track=info.getTrack();
            title=BaeUtils::fixString(info.getTitle());
            artist=BaeUtils::fixString(info.getArtist());
            genre=info.getGenre();

            if (info.getAlbum().isEmpty()) {
                qDebug()<<"the album has not title, so i'm going to try and get it.";
                info.writeData();
                album=info.getAlbum();
            } else {
                album=info.getAlbum();
            }

            album=BaeUtils::fixString(album);

            query.prepare("INSERT INTO tracks (track, title, artist, album, genre, location, stars, babe, art, played)" "VALUES (:track, :title, :artist, :album, :genre, :location, :stars, :babe, :art, :played) ");
            query.bindValue(":track", track);
            query.bindValue(":title", title);
            query.bindValue(":artist", artist);
            query.bindValue(":album", album);
            query.bindValue(":genre", genre);
            query.bindValue(":location", file);
            query.bindValue(":stars", 0);
            query.bindValue(":babe", babe);
            query.bindValue(":art", "");
            query.bindValue(":played", 0);

            if (query.exec()) {
                success = true;
                qDebug()<< "writting to db: "<< title;
                if (!albums.contains(artist+" "+album)) {
                    query.prepare("INSERT INTO albums (title, artist, art, location)" "VALUES (:title, :artist, :art, :location)");
                    query.bindValue(":title", album);
                    query.bindValue(":artist", artist);
                    query.bindValue(":art", "");
                    // query.bindValue(":location", QFileInfo(file).dir().path());
                    if (query.exec()) {
                        albums<<artist+" "+album;
                        success = true;
                    } else {
                        return false;
                    }
                }

                if(!artists.contains(artist))
                {
                    query.prepare("INSERT INTO artists (title, art, location)" "VALUES (:title, :art, :location)");
                    query.bindValue(":title", artist);
                    query.bindValue(":art", "");
                    // query.bindValue(":location", QFileInfo(file).dir().path());
                    if(query.exec()) artists<<artist;
                }
                emit progress((i++)+1);
            } else {
                qDebug() << "adding track error:  "
                         << query.lastError()
                         <<info.getTitle();
            }
        }
        qDebug()<<"finished wrrting to database";
        emit DBactionFinished(true);
    }
    else return false;
    return success;
}

void CollectionDB::insertCoverArt(QString path,QStringList info)
{
    //UPDATE albums SET art = "lalaltest" WHERE title = "Starboy" AND artist = "The Weeknd"
    qDebug()<<"the path:"<<path<<"the list:"<<info.at(0)<<info.at(1);
    if (info.size() == 2) {
        QSqlQuery query;
        query.prepare("UPDATE albums SET art = (:art) WHERE title = (:title) AND artist = (:artist)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
        query.bindValue(":art",  path.isEmpty()?"NULL": path );
        query.bindValue(":title", info.at(0));
        query.bindValue(":artist", info.at(1));
        if (query.exec()) {
            qDebug()<<"Artwork[cover] inserted into DB"<<info.at(0)<<info.at(1);
            if(!albums.contains(info.at(0))) albums<<info.at(1)+" "+info.at(0);
            //qDebug()<<"insertInto<<"<<"UPDATE "+tableName+" SET "+column+" = "+ value + " WHERE location = "+location;
        } else {
            qDebug()<<"COULDNT Artwork[cover] inerted into DB"<<info.at(0)<<info.at(1);
        }
    }
}

void CollectionDB::insertHeadArt(QString path, QStringList info)
{
    if (info.size() == 1) {
        QSqlQuery query;
        query.prepare("UPDATE artists SET art = (:art) WHERE title = (:title)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
        query.bindValue(":art", path.isEmpty()?"NULL": path );
        query.bindValue(":title", info.at(0));
        if (query.exec()) {
            qDebug()<<"Artwork[head] inerted into DB"<<info.at(0);
            if(!artists.contains(info.at(0)))artists<<info.at(0);
            //qDebug()<<"insertInto<<"<<"UPDATE "+tableName+" SET "+column+" = "+ value + " WHERE location = "+location;
        }
    }
}


void CollectionDB::setTrackList(QList<Track> trackList)
{
    this->trackList=trackList;
}

bool CollectionDB::check_existance(QString tableName, QString searchId, QString search)
{
    QSqlQuery query;
    query.prepare("SELECT "+ searchId +" FROM "+tableName+" WHERE "+searchId+" = (:search)");
    query.bindValue(":search", search);
    if (query.exec()) {
        if (query.next()) {
            qDebug()<< "it exists";
            return true;
        } else {
            qDebug()<<"currnt song doesn't exists in db";
            return false;
        }
    } else {
        return false;
    }
}

bool CollectionDB::execQuery(QString queryTxt)
{
    QSqlQuery query;
    query.prepare(queryTxt);
    //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
    if(query.exec()) {
        qDebug()<<"executing query: "<<queryTxt;
        return true;
    } else {
        return false;
    }
}

bool CollectionDB::insertInto(QString tableName, QString column, QString location, int value)
{
    QSqlQuery query;
    if (query.exec("PRAGMA synchronous=OFF")) {
        query.prepare("UPDATE "+tableName+" SET "+column+" = (:value) WHERE location = (:location)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
        query.bindValue(":value", value);
        query.bindValue(":location", location);
        if(query.exec()) {
            qDebug()<<"insertInto<<"<<"UPDATE "+tableName+" SET "+column+" = "+ value + " WHERE location = "+location;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool CollectionDB::insertInto(QString tableName, QString column, QString location, QString value)
{
    QSqlQuery query;
    query.prepare("UPDATE "+tableName+" SET "+column+" = (:value) WHERE location = (:location)" );
    // query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
    query.bindValue(":value", value);
    query.bindValue(":location", location);
    if (query.exec()) {
        qDebug()<<"insertInto<<"<<"UPDATE "+tableName+" SET "+column+" = "+ value + " WHERE location = "+location;
        return true;
    } else {
        return false;
    }
}

void CollectionDB::insertPlaylist(QString name, QString color)
{
    if (color.isEmpty()) {
        QSqlQuery query;
        query.prepare("INSERT INTO playlists (title)" "VALUES (:title) ");
        query.bindValue(":title", name);
    } else if(name.isEmpty()) {
        QSqlQuery query;
        query.prepare("INSERT INTO playlists (title, art)" "VALUES (:title, :art) ");
        query.bindValue(":title", "mood");
        query.bindValue(":art", color);
    } else if(!name.isEmpty()&&!color.isEmpty()) {
        QSqlQuery query;
        query.prepare("INSERT INTO playlists (title, art)" "VALUES (:title, :art) ");
        query.bindValue(":title", "mood");
        query.bindValue(":art", color);
    }
}

QStringList CollectionDB::getPlaylists(int limit, int offset, QString &orderBy)
{
    QStringList playlists;
    QVariantList resultSet = m_database->select("playlist", QVariantMap(), -1, 0, "title");
    foreach (const QVariant entry, resultSet)
        playlists << entry.toMap().value("title").toString();
    return playlists;
}

QStringList CollectionDB::getPlaylistsMoods()
{
    return getPlaylists(-1, 0);
}
