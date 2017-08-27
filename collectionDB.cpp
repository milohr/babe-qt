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
    auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\"").arg(table, column, newValue.toString().replace("\"","\"\""), op.toString(), id);
    QSqlQuery query(queryStr);
    qDebug()<<queryStr;
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
                QString title = Bae::fixString(info.getTitle()); /* to fix*/
                QString artist = Bae::fixString(info.getArtist());
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

                album=Bae::fixString(album);

                /* first needs to insert album and artist*/
                QVariantMap sourceMap {{"url",sourceUrl},{"source_types_id", sourceType(file)}};
                this->insert(Bae::DBTablesMap[Bae::DBTables::SOURCES],sourceMap);

                QVariantMap artistMap {{Bae::ArtistsColsMap[Bae::ArtistsCols::ARTIST_TITLE], artist},
                                       {Bae::ArtistsColsMap[Bae::ArtistsCols::ARTIST_ARTWORK],""},
                                       {Bae::ArtistsColsMap[Bae::ArtistsCols::ARTIST_WIKI],""}};

                this->insert(Bae::DBTablesMap[Bae::DBTables::ARTISTS],artistMap);

                QVariantMap albumMap {{Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_TITLE],album},
                                      {Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_ARTIST],artist},
                                      {Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_ARTWORK],""},
                                      {Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_WIKI],""}};
                this->insert(Bae::DBTablesMap[Bae::DBTables::ALBUMS],albumMap);

                QVariantMap trackMap {{Bae::TracksColsMap[Bae::TracksCols::URL],file},
                                      {Bae::TracksColsMap[Bae::TracksCols::SOURCES_URL],sourceUrl},
                                      {Bae::TracksColsMap[Bae::TracksCols::TRACK],track},
                                      {Bae::TracksColsMap[Bae::TracksCols::TITLE],title},
                                      {Bae::TracksColsMap[Bae::TracksCols::ARTIST],artist},
                                      {Bae::TracksColsMap[Bae::TracksCols::ALBUM],album},
                                      {Bae::TracksColsMap[Bae::TracksCols::DURATION],duration},
                                      {Bae::TracksColsMap[Bae::TracksCols::PLAYED],0},
                                      {Bae::TracksColsMap[Bae::TracksCols::BABE],babe},
                                      {Bae::TracksColsMap[Bae::TracksCols::STARS],0},
                                      {Bae::TracksColsMap[Bae::TracksCols::RELEASE_DATE],year},
                                      {Bae::TracksColsMap[Bae::TracksCols::ADD_DATE],QDate::currentDate()},
                                      {Bae::TracksColsMap[Bae::TracksCols::LYRICS],""},
                                      {Bae::TracksColsMap[Bae::TracksCols::GENRE],genre},
                                      {Bae::TracksColsMap[Bae::TracksCols::ART],""}};

                this->insert(Bae::DBTablesMap[Bae::DBTables::TRACKS],trackMap);

                emit progress((i++)+1);
            }

            qDebug()<<"finished wrrting to database";
            emit DBactionFinished();
        }

    }
}

bool CollectionDB::rateTrack(const QString &path, const int &value)
{
    if(this->update(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                    Bae::TracksColsMap[Bae::TracksCols::STARS],
                    value,
                    Bae::TracksColsMap[Bae::TracksCols::URL],
                    path)) return true;
    return false;
}

bool CollectionDB::babeTrack(const QString &path, const bool &value)
{
    if(this->update(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                    Bae::TracksColsMap[Bae::TracksCols::BABE],
                    value?1:0,
                    Bae::TracksColsMap[Bae::TracksCols::URL],
                    path)) return true;
    return false;
}

bool CollectionDB::moodTrack(const QString &path, const QString &value)
{

    return false;

}

bool CollectionDB::artTrack(const QString &path, const QString &value)
{
    if(this->update(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                    Bae::TracksColsMap[Bae::TracksCols::ART],
                    value,
                    Bae::TracksColsMap[Bae::TracksCols::URL],
                    path)) return true;
    return false;
}

bool CollectionDB::lyricsTrack(const QString &path, const QString &value)
{
    if(this->update(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                    Bae::TracksColsMap[Bae::TracksCols::LYRICS],
                    value,
                    Bae::TracksColsMap[Bae::TracksCols::URL],
                    path)) return true;
    return false;
}

bool CollectionDB::wikiArtist(const QString &artist, const QString &value)
{
    if(this->update(Bae::DBTablesMap[Bae::DBTables::ARTISTS],
                    Bae::ArtistsColsMap[Bae::ArtistsCols::ARTIST_WIKI],
                    value,
                    Bae::ArtistsColsMap[Bae::ArtistsCols::ARTIST_TITLE],
                    artist)) return true;
    return false;}

bool CollectionDB::wikiAlbum(const QString &album, const QString &artist,  QString value)
{
    auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\" AND %6 = \"%7\"").arg(
                Bae::DBTablesMap[Bae::DBTables::ALBUMS],
            Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_WIKI], value.replace("\"","\"\""),
            Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_TITLE],
            album,Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_ARTIST], artist);

    qDebug()<<queryStr;
    QSqlQuery query(queryStr);

    if(query.exec())
        return true;

    query.lastError().text();
    return false;
}

bool CollectionDB::addPlaylist(const QString &title)
{
    if(!title.isEmpty())
    {
        QVariantMap playlist {{"title",title},{"moods_tag",""},{"addDate",QDate::currentDate()}};
        if(this->insert(Bae::DBTablesMap[Bae::DBTables::PLAYLISTS],playlist))
            return true;
    }

    return false;
}

bool CollectionDB::trackPlaylist(const QString &url, const QString &playlist)
{
    QVariantMap map {{"playlists_title",playlist},{"tracks_url",url},{"addDate",QDate::currentDate()}};

    if(this->insert(Bae::DBTablesMap[Bae::DBTables::TRACKS_PLAYLISTS],map))
        return true;

    return false;
}

Bae::TRACKMAP_LIST CollectionDB::getTrackData(const QStringList &urls)
{
    Bae::TRACKMAP_LIST mapList;

    for(auto url:urls)
        mapList<<this->getTrackData
                 (
                     QString("SELECT * FROM %1 WHERE %2 = \"%3\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                     Bae::TracksColsMap[Bae::TracksCols::URL],url)
                );

    return mapList;
}

Bae::TRACKMAP_LIST CollectionDB::getTrackData(const QString &queryText)
{
    //    qDebug()<<"on getTrackData "<<queryText;
    Bae::TRACKMAP_LIST mapList;

    QSqlQuery query(queryText);

    if(query.exec())
        while(query.next())
            mapList << Bae::TRACKMAP
            {
            {Bae::TracksCols::TRACK,query.value(Bae::TracksCols::TRACK).toString()},
            {Bae::TracksCols::TITLE,query.value(Bae::TracksCols::TITLE).toString()},
            {Bae::TracksCols::ARTIST,query.value(Bae::TracksCols::ARTIST).toString()},
            {Bae::TracksCols::ALBUM,query.value(Bae::TracksCols::ALBUM).toString()},
            {Bae::TracksCols::DURATION,query.value(Bae::TracksCols::DURATION).toString()},
            {Bae::TracksCols::GENRE,query.value(Bae::TracksCols::GENRE).toString()},
            {Bae::TracksCols::URL,query.value(Bae::TracksCols::URL).toString()},
            {Bae::TracksCols::STARS,query.value(Bae::TracksCols::STARS).toString()},
            {Bae::TracksCols::BABE,query.value(Bae::TracksCols::BABE).toString()},
            {Bae::TracksCols::ART,query.value(Bae::TracksCols::ART).toString()},
            {Bae::TracksCols::PLAYED,query.value(Bae::TracksCols::PLAYED).toString()},
            {Bae::TracksCols::ADD_DATE,query.value(Bae::TracksCols::ADD_DATE).toString()},
            {Bae::TracksCols::RELEASE_DATE,query.value(Bae::TracksCols::RELEASE_DATE).toString()},
            {Bae::TracksCols::LYRICS,query.value(Bae::TracksCols::LYRICS).toString()},
            {Bae::TracksCols::SOURCES_URL,query.value(Bae::TracksCols::SOURCES_URL).toString()}
};

    return mapList;
}

QString CollectionDB::getTrackLyrics(const QString &url)
{
    QString lyrics;
    QSqlQuery query(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(Bae::TracksColsMap[Bae::TracksCols::LYRICS],
                    Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::TracksColsMap[Bae::TracksCols::URL],url));

    if(query.exec())
        while (query.next())
            lyrics=query.value(0).toString();

    return lyrics;
}


QString CollectionDB::getTrackArt(const QString &path)
{
    QString color;
    QSqlQuery query(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(Bae::TracksColsMap[Bae::TracksCols::ART],
                    Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::TracksColsMap[Bae::TracksCols::URL],path));

    if(query.exec())
        while (query.next())
            color=query.value(0).toString();

    return color;
}

QString CollectionDB::getArtistArt(const QString &artist)
{
    QString artistHead;

    QSqlQuery queryHead(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(Bae::ArtistsColsMap[Bae::ArtistsCols::ARTIST_ARTWORK],
                        Bae::DBTablesMap[Bae::DBTables::ARTISTS],
            Bae::ArtistsColsMap[Bae::ArtistsCols::ARTIST_TITLE],artist));

    while (queryHead.next())
        if(!queryHead.value(0).toString().isEmpty()&&queryHead.value(0).toString()!="NULL")
            artistHead = queryHead.value(0).toString();

    return artistHead;
}

QString CollectionDB::getArtistWiki(const QString &artist)
{
    QString wiki;

    QSqlQuery query(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(Bae::ArtistsColsMap[Bae::ArtistsCols::ARTIST_WIKI],
                    Bae::DBTablesMap[Bae::DBTables::ARTISTS],
            Bae::ArtistsColsMap[Bae::ArtistsCols::ARTIST_TITLE],artist));

    if(query.exec())
        while (query.next())
            wiki = query.value(0).toString();

    return wiki;
}

QString CollectionDB::getAlbumArt(const QString &album, const QString &artist)
{
    QString albumCover;
    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\" AND %5 = \"%6\"").arg(Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_ARTWORK],
            Bae::DBTablesMap[Bae::DBTables::ALBUMS],
            Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_TITLE],album,
            Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_ARTIST],artist);
    QSqlQuery queryCover(queryStr);
    qDebug()<<queryStr;
    while (queryCover.next())
        if(!queryCover.value(0).toString().isEmpty()&&queryCover.value(0).toString()!="NULL")
            albumCover = queryCover.value(0).toString();

    return albumCover;
}

QString CollectionDB::getAlbumWiki(const QString &album, const QString &artist)
{
    QString wiki;
    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\" AND %5 = \"%6\"").arg(Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_WIKI],
            Bae::DBTablesMap[Bae::DBTables::ALBUMS],
            Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_TITLE],album,
            Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_ARTIST],artist);
    QSqlQuery query(queryStr);

    if(query.exec())
        while (query.next())
            wiki = query.value(0).toString();

    return wiki;
}

QStringList CollectionDB::getPlaylists()
{

    QStringList files;
    QSqlQuery query("SELECT title, addDate FROM playlists ORDER by addDate desc");

    if (query.exec())
        while (query.next())
            files << query.value(0).toString();

    return files;

}


void CollectionDB::insertCoverArt(const QString &path, const Bae::TRACKMAP &track)
{
    qDebug()<<"the path:"<<path<<"the list:"<<track[Bae::TracksCols::ARTIST]<<track[Bae::TracksCols::ALBUM];
    if(!track[Bae::TracksCols::ARTIST].isEmpty()&&!track[Bae::TracksCols::ALBUM].isEmpty())
    {
        auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\" AND %6 = \"%7\"").arg(Bae::DBTablesMap[Bae::DBTables::ALBUMS],
                Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_ARTWORK],
                path.isEmpty()?"NULL": path,
                Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_TITLE],
                track[Bae::TracksCols::ALBUM],
                Bae::AlbumsColsMap[Bae::AlbumsCols::ALBUM_ARTIST],
                track[Bae::TracksCols::ARTIST]);

        QSqlQuery query(queryStr);
        qDebug()<<queryStr;
        if(query.exec()) qDebug()<<"Artwork[cover] inserted into DB"<<track[Bae::TracksCols::ARTIST]<<track[Bae::TracksCols::ALBUM];
        else qDebug()<<"COULDNT Artwork[cover] inerted into DB"<<track[Bae::TracksCols::ARTIST]<<track[Bae::TracksCols::ALBUM];

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
    queryTracks.prepare("DELETE FROM tracks WHERE url LIKE \""+path+"%\"");
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
        QString artist = query.value(Bae::TracksCols::ARTIST).toString();
        QString album = query.value(Bae::TracksCols::ALBUM).toString();
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
            QString artist = query.value(Bae::TracksCols::ARTIST).toString();
            QString file = query.value(Bae::TracksCols::URL).toString();
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



void CollectionDB::insertHeadArt(const QString &path, const Bae::TRACKMAP &track)
{
    if(!track[Bae::TracksCols::ARTIST].isEmpty())
    {
        QSqlQuery query;
        query.prepare("UPDATE artists SET artwork = (:artwork) WHERE title = (:title)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
        query.bindValue(":artwork", path.isEmpty()?"NULL": path );
        query.bindValue(":title", track[Bae::TracksCols::ARTIST]);
        if(query.exec())
        {
            qDebug()<<"Artwork[head] inerted into DB"<<track[Bae::TracksCols::ARTIST];
            if(!artists.contains(track[Bae::TracksCols::ARTIST]))artists<<track[Bae::TracksCols::ARTIST];
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


