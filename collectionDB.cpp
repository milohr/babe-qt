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


CollectionDB::CollectionDB()
{

}

/*CollectionDB::CollectionDB(bool connect)
{
    if(connect)
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName("../player/collection.db");
        m_db.open();

        if (!m_db.open())
        {
           qDebug() << "Error: connection with database fail";
        }
        else
        {
           qDebug() << "Database: connection ok";

        }
    }
}*/

void CollectionDB::closeConnection()
{
    m_db.close();
}

void CollectionDB::setUpCollection(const QString &path)
{
    this->m_db = QSqlDatabase::addDatabase("QSQLITE");
    this->m_db.setDatabaseName(path);
    this->openDB();
}


void CollectionDB::prepareCollectionDB()
{
    QSqlQuery query;

    QFile file(":/Data/script.sql");

    if (!file.exists())
    {
        QString log = QStringLiteral("Fatal error on build database. The file '");
        log.append(file.fileName() + QStringLiteral("' for database and tables creation query cannot be not found!"));
        qDebug()<<log;
        return;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<QStringLiteral("Fatal error on try to create database! The file with sql queries for database creation cannot be opened!");
        return;
    }

    bool hasText;
    QString line;
    QByteArray readLine;
    QString cleanedLine;
    QStringList strings;



    while (!file.atEnd())
    {
        qDebug()<<"finished creating schema";
        hasText     = false;
        line        = "";
        readLine    = "";
        cleanedLine = "";
        strings.clear();
        while (!hasText)
        {
            readLine    = file.readLine();
            cleanedLine = readLine.trimmed();
            strings     = cleanedLine.split("--");
            cleanedLine = strings.at(0);
            if (!cleanedLine.startsWith("--") && !cleanedLine.startsWith("DROP") && !cleanedLine.isEmpty())
                line += cleanedLine;
            if (cleanedLine.endsWith(";"))
                break;
            if (cleanedLine.startsWith("COMMIT"))
                hasText = true;
        }
        if (!line.isEmpty())
        {
            if (query.exec(line)) qDebug()<<"exec fine";
        } else qDebug()<<"exec wrong"<<query.lastError();
    }

    qDebug()<<"finished creating schema";
    file.close();
}

bool CollectionDB::check_existance(const QString &tableName, const QString &searchId,const QString &search)
{
    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(searchId, tableName, searchId, search);
    QSqlQuery query;
    query.prepare(queryStr);
    qDebug()<<queryStr;
    if (query.exec())
    {
        if (query.next()) return true;
    }else qDebug()<<query.lastError().text();

    return false;

}

bool CollectionDB::insert(const QString &tableName, const QVariantMap &insertData)
{
    if (tableName.isEmpty())
    {
        qDebug()<<QStringLiteral("Fatal error on insert! The table name is empty!");
        return false;
    } else if (insertData.isEmpty())
    {
        qDebug()<<QStringLiteral("Fatal error on insert! The insertData is empty!");
        return false;
    }

    QStringList strValues;
    QStringList fields = insertData.keys();
    QVariantList values = insertData.values();
    int totalFields = fields.size();
    for (int i = 0; i < totalFields; ++i)
        strValues.append("?");

    if (!m_db.isOpen())
    {
        qDebug()<< QStringLiteral("Fatal error on insert! Database connection cannot be opened!");
        return false;
    }

    QString sqlQueryString = "INSERT INTO " + tableName + "(" + QString(fields.join(",")) + ") VALUES(" + QString(strValues.join(",")) + ")";
    QSqlQuery query;
    query.prepare(sqlQueryString);

    int k = 0;
    foreach (const QVariant &value, values)
        query.bindValue(k++, value);

    return query.exec();

}

bool CollectionDB::update(const QString &table,const QString &column,const QVariant &newValue,const QVariant &op, const QString &id)
{
    auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\"").arg(table, column, newValue.toString(), op.toString(), id);
    QSqlQuery query;
    query.prepare(queryStr);

    qDebug()<<"QUERY:"<<queryStr;

    if(query.exec())
        return true;

    query.lastError().text();
    return false;

}

bool CollectionDB::openDB()
{
    if (m_db.open())
        return true;


    qDebug() << "Error: connection with database fail" <<m_db.lastError().text();
    return false;
}


void CollectionDB::addTrack(const QStringList &paths,const int &babe)
{
    if(!paths.isEmpty())
    {
        QSqlQuery query;
        if(query.exec("PRAGMA synchronous=OFF"))
        {
            int i=0;

            qDebug()<<"started writing to database...";
            for(auto file:paths)
            {

                TagInfo info(file);
                QString  album;
                int track;
                QString title = BaeUtils::fixString(info.getTitle()); /* to fix*/
                QString artist = BaeUtils::fixString(info.getArtist());
                QString genre = info.getGenre();
                QString sourceUrl = QFileInfo(file).dir().path();
                int duration = info.getDuration();
                auto year = info.getYear();
                qDebug()<< "writting to db: "<<title<<artist;

                if(info.getAlbum().isEmpty())
                {
                    qDebug()<<"the album has not title, so i'm going to try and get it.";
                    info.writeData(); // actually means to search the name and load it into metadata
                    album=info.getAlbum();
                    track=info.getTrack();
                }else
                {
                    album=info.getAlbum();
                    track=info.getTrack();
                }

                album=BaeUtils::fixString(album);

                /* first needs to insert album and artist*/
                QVariantMap sourceMap {{"url",sourceUrl},{"source_types_id", sourceType(file)}};
                this->insert(BaeUtils::DBTablesMap[BaeUtils::DBTables::SOURCES],sourceMap);

                QVariantMap artistMap {{BaeUtils::ArtistsColsMap[BaeUtils::ArtistsCols::ARTIST_TITLE], artist},
                                       {BaeUtils::ArtistsColsMap[BaeUtils::ArtistsCols::ARTIST_ARTWORK],""},
                                       {BaeUtils::ArtistsColsMap[BaeUtils::ArtistsCols::ARTIST_WIKI],""}};

                this->insert(BaeUtils::DBTablesMap[BaeUtils::DBTables::ARTISTS],artistMap);

                QVariantMap albumMap {{BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_TITLE],album},
                                      {BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_ARTIST],artist},
                                      {BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_ARTWORK],""},
                                      {BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_WIKI],""}};
                this->insert(BaeUtils::DBTablesMap[BaeUtils::DBTables::ALBUMS],albumMap);

                QVariantMap trackMap {{BaeUtils::TracksColsMap[BaeUtils::TracksCols::URL],file},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::SOURCES_URL],sourceUrl},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::TRACK],track},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::TITLE],title},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::ARTIST],artist},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::ALBUM],album},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::DURATION],duration},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::PLAYED],0},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::BABE],babe},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::STARS],0},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::RELEASE_DATE],year},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::ADD_DATE],QDate::currentDate()},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::LYRICS],""},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::GENRE],genre},
                                      {BaeUtils::TracksColsMap[BaeUtils::TracksCols::ART],""}};

                this->insert(BaeUtils::DBTablesMap[BaeUtils::DBTables::TRACKS],trackMap);

                emit progress((i++)+1);
            }

            qDebug()<<"finished wrrting to database";
            emit DBactionFinished();
        }

    }
}

bool CollectionDB::rateTrack(const QString &path, const int &value)
{
    if(this->update(BaeUtils::DBTablesMap[BaeUtils::DBTables::TRACKS],
                    BaeUtils::TracksColsMap[BaeUtils::TracksCols::STARS],
                    value,
                    BaeUtils::TracksColsMap[BaeUtils::TracksCols::URL],
                    path)) return true;
    return false;
}

bool CollectionDB::babeTrack(const QString &path, const bool &value)
{
    if(this->update(BaeUtils::DBTablesMap[BaeUtils::DBTables::TRACKS],
                    BaeUtils::TracksColsMap[BaeUtils::TracksCols::BABE],
                    value?1:0,
                    BaeUtils::TracksColsMap[BaeUtils::TracksCols::URL],
                    path)) return true;
    return false;
}

bool CollectionDB::moodTrack(const QString &path, const QString &value)
{

    return false;

}

bool CollectionDB::artTrack(const QString &path, const QString &value)
{
    if(this->update(BaeUtils::DBTablesMap[BaeUtils::DBTables::TRACKS],
                    BaeUtils::TracksColsMap[BaeUtils::TracksCols::ART],
                    value,
                    BaeUtils::TracksColsMap[BaeUtils::TracksCols::URL],
                    path)) return true;
    return false;
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QStringList &urls)
{
    QList<QMap<int, QString>> mapList;

    for(auto url:urls)
        mapList<<this->getTrackData
                (
                    QString("SELECT * FROM %1 WHERE %2 = \"%3\"").arg(BaeUtils::DBTablesMap[BaeUtils::DBTables::TRACKS],
                    BaeUtils::TracksColsMap[BaeUtils::TracksCols::URL],url)
                );

    return mapList;
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QString &queryText)
{
    //    qDebug()<<"on getTrackData "<<queryText;
    QList<QMap<int, QString>> mapList;

    QSqlQuery query(queryText);

    if(query.exec())
        while(query.next())
            mapList << QMap<int, QString>
            {
                {BaeUtils::TracksCols::TRACK,query.value(BaeUtils::TracksCols::TRACK).toString()},
                {BaeUtils::TracksCols::TITLE,query.value(BaeUtils::TracksCols::TITLE).toString()},
                {BaeUtils::TracksCols::ARTIST,query.value(BaeUtils::TracksCols::ARTIST).toString()},
                {BaeUtils::TracksCols::ALBUM,query.value(BaeUtils::TracksCols::ALBUM).toString()},
                {BaeUtils::TracksCols::DURATION,query.value(BaeUtils::TracksCols::DURATION).toString()},
                {BaeUtils::TracksCols::GENRE,query.value(BaeUtils::TracksCols::GENRE).toString()},
                {BaeUtils::TracksCols::URL,query.value(BaeUtils::TracksCols::URL).toString()},
                {BaeUtils::TracksCols::STARS,query.value(BaeUtils::TracksCols::STARS).toString()},
                {BaeUtils::TracksCols::BABE,query.value(BaeUtils::TracksCols::BABE).toString()},
                {BaeUtils::TracksCols::ART,query.value(BaeUtils::TracksCols::ART).toString()},
                {BaeUtils::TracksCols::PLAYED,query.value(BaeUtils::TracksCols::PLAYED).toString()},
                {BaeUtils::TracksCols::ADD_DATE,query.value(BaeUtils::TracksCols::ADD_DATE).toString()},
                {BaeUtils::TracksCols::RELEASE_DATE,query.value(BaeUtils::TracksCols::RELEASE_DATE).toString()}
            };

    return mapList;
}


QString CollectionDB::getTrackArt(const QString &path)
{
    QString color;
    QSqlQuery query(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(BaeUtils::TracksColsMap[BaeUtils::TracksCols::ART],
                    BaeUtils::DBTablesMap[BaeUtils::DBTables::TRACKS],
            BaeUtils::TracksColsMap[BaeUtils::TracksCols::URL],path));

    if(query.exec())
        while (query.next())
            color=query.value(0).toString();

    return color;
}

QString CollectionDB::getArtistArt(const QString &artist)
{
    QString artistHead;

    QSqlQuery queryHead(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(BaeUtils::ArtistsColsMap[BaeUtils::ArtistsCols::ARTIST_ARTWORK],
                        BaeUtils::DBTablesMap[BaeUtils::DBTables::ARTISTS],
            BaeUtils::ArtistsColsMap[BaeUtils::ArtistsCols::ARTIST_TITLE],artist));

    while (queryHead.next())
        if(!queryHead.value(0).toString().isEmpty()&&queryHead.value(0).toString()!="NULL")
            artistHead = queryHead.value(0).toString();

    return artistHead;
}

QString CollectionDB::getAlbumArt(const QString &album, const QString &artist)
{
    QString albumCover;
    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\" AND %5 = \"%6\"").arg(BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_ARTWORK],
            BaeUtils::DBTablesMap[BaeUtils::DBTables::ALBUMS],
            BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_TITLE],album,
            BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_ARTIST],artist);
    QSqlQuery queryCover(queryStr);
    qDebug()<<queryStr;
    while (queryCover.next())
        if(!queryCover.value(0).toString().isEmpty()&&queryCover.value(0).toString()!="NULL")
            albumCover = queryCover.value(0).toString();

    return albumCover;
}

void CollectionDB::insertCoverArt(QString path,QStringList info)
{
    qDebug()<<"the path:"<<path<<"the list:"<<info.at(0)<<info.at(1);
    if(info.size()==2)
    {
        auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\" AND %6 = \"%7\"").arg(BaeUtils::DBTablesMap[BaeUtils::DBTables::ALBUMS],
                BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_ARTWORK],
                path.isEmpty()?"NULL": path,
                BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_TITLE],
                info.at(0),
                BaeUtils::AlbumsColsMap[BaeUtils::AlbumsCols::ALBUM_ARTIST],
                info.at(1));

        QSqlQuery query(queryStr);
        qDebug()<<queryStr;
        if(query.exec()) qDebug()<<"Artwork[cover] inserted into DB"<<info.at(0)<<info.at(1);
        else qDebug()<<"COULDNT Artwork[cover] inerted into DB"<<info.at(0)<<info.at(1);

    }

}


CollectionDB::sourceTypes CollectionDB::sourceType(const QString &url)
{
    /*for now*/

    return sourceTypes::LOCAL;
}


QSqlQuery CollectionDB::getQuery(QString queryTxt)
{
    QSqlQuery query(queryTxt);
    return query;
}



/*******************OLD STUFF********************/


bool CollectionDB::removePath(const QString &path)
{
    qDebug()<<"trying to delete all from :"<< path;
    QSqlQuery queryTracks;
    queryTracks.prepare("DELETE FROM tracks WHERE location LIKE \"%"+path+"%\"");
    if(queryTracks.exec()) return true;
    else return false;
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





bool CollectionDB::removeQuery(QString queryTxt)
{
    QSqlQuery query;
    query.prepare(queryTxt);

    if(!query.exec())
    {
        qDebug() << "removeQuery error: "<< query.lastError();
        return false;
    }else return true;
}

bool CollectionDB::checkQuery(QString queryTxt)
{
    QSqlQuery query(queryTxt);

    //    qDebug()<<"The Query is: "<<queryTxt;

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
        QString artist = query.value(BaeUtils::TracksCols::ARTIST).toString();
        QString album = query.value(BaeUtils::TracksCols::ALBUM).toString();
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
            QString artist = query.value(BaeUtils::TracksCols::ARTIST).toString();
            QString file = query.value(BaeUtils::TracksCols::URL).toString();
            if(!artists.contains(artist))
            {
                query.prepare("INSERT INTO artists (title, art, location) VALUES (:title, :art, :location)");
                query.bindValue(":title", artist);
                query.bindValue(":art", "");
                query.bindValue(":location", QFileInfo(file).dir().path());
                if(query.exec()) artists<<artist;
            }
        }
    }
}



void CollectionDB::insertHeadArt(QString path, QStringList info)
{
    if(info.size()==1)
    {
        QSqlQuery query;
        query.prepare("UPDATE artists SET artwork = (:artwork) WHERE title = (:title)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
        query.bindValue(":artwork", path.isEmpty()?"NULL": path );
        query.bindValue(":title", info.at(0));
        if(query.exec())
        {
            qDebug()<<"Artwork[head] inerted into DB"<<info.at(0);
            if(!artists.contains(info.at(0)))artists<<info.at(0);
            //qDebug()<<"insertInto<<"<<"UPDATE "+tableName+" SET "+column+" = "+ value + " WHERE location = "+location;
        }
    }
}


void CollectionDB::setTrackList(QList<Track> trackList)
{
    this->trackList=trackList;

    /*for(auto tr:trackList)
        {
            qDebug()<<QString::fromStdString(tr.getTitle());
        }*/
}



bool CollectionDB::execQuery(QString queryTxt)
{

    QSqlQuery query;
    query.prepare(queryTxt);
    //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");

    if(query.exec())
    {
        qDebug()<<"executing query: "<<queryTxt;

        return true;
    }else return false;


}

bool CollectionDB::insertInto(const QString &tableName, const QString &column, const QString &location, const QVariant &value)
{
    QSqlQuery query;

    if(query.exec("PRAGMA synchronous=OFF"))
    {
        query.prepare("UPDATE "+tableName+" SET "+column+" = (:value) WHERE url = (:url)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");

        query.bindValue(":value", value);
        query.bindValue(":url", location);
        if(query.exec())
        {
            return true;
        }else return false;

    }else return false;

}


void CollectionDB::insertPlaylist(const QString &name)
{
    if(!name.isEmpty())
    {
        QSqlQuery query;
        query.prepare("INSERT INTO playlists (title) VALUES (:title) ");

        query.bindValue(":title", name);

        if(query.exec())
            qDebug()<<"playlist created<<"<< name ;
    }
}

QStringList CollectionDB::getPlaylists()
{
    QSqlQuery query;
    QStringList files;
    query.prepare("SELECT * FROM playlists");

    if (query.exec())
        while (query.next())
            if(!query.value(0).toString().contains("mood")&&!query.value(0).toString().isEmpty())
                files << query.value(0).toString();

    return files;

}

QStringList CollectionDB::getPlaylistsMoods()
{
    QStringList moods;
    QSqlQuery query;

    query.prepare("SELECT * FROM playlists order by title");

    if (query.exec())
        while (query.next())
            if(!query.value(1).toString().isEmpty()&&query.value(0).toString().contains("mood"))
                moods << query.value(1).toString();

    return moods;

}


