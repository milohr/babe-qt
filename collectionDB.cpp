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

void CollectionDB::openCollection(const QString &path)
{
    this->m_db = QSqlDatabase::addDatabase("QSQLITE");
    this->m_db.setDatabaseName(path);
    this->sqlQuery = QSqlQuery(m_db);
    if (!m_db.open()) qDebug() << "Error: connection with database fail" <<m_db.lastError().text();
    else qDebug() << "Database: connection ok";
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

    QString sqlQueryString;
    sqlQueryString = "INSERT INTO " + tableName + "(" + QString(fields.join(",")) + ") VALUES(" + QString(strValues.join(",")) + ")";
    sqlQuery.prepare(sqlQueryString);

    int k = 0;
    foreach (const QVariant &value, values)
        sqlQuery.bindValue(k++, value);

    return sqlQuery.exec();
}


void CollectionDB::addTrack(const QStringList &paths,const int &babe)
{
    if(!paths.isEmpty())
    {


        if(sqlQuery.exec("PRAGMA synchronous=OFF"))
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
                this->insert("sources",sourceMap);

                QVariantMap artistMap {{"title", artist},{"artwork",""},{"wiki",""}};
                this->insert("artists",artistMap);

                QVariantMap albumMap {{"title",album},{"artist",artist},{"artwork",""},{"wiki",""}};
                this->insert("albums",albumMap);

                QVariantMap trackMap {{"url",file},{"sources_url",sourceUrl},{"track",track},{"title",title},{"artist",artist},{"album",album},{"duration",duration},{"played",0},{"babe",0},{"stars",0},{"releaseDate",year},{"addDate",QDate::currentDate()},{"lyrics",""},{"genre",genre},{"art",""}};
                this->insert("tracks",trackMap);

                emit progress((i++)+1);
            }

            qDebug()<<"finished wrrting to database";
            emit DBactionFinished();
        }

    }
}

void CollectionDB::insertCoverArt(QString path,QStringList info)
{
    qDebug()<<"the path:"<<path<<"the list:"<<info.at(0)<<info.at(1);
    if(info.size()==2)
    {
        QSqlQuery query;
        query.prepare("UPDATE albums SET artwork = (:artwork) WHERE title = (:title) AND artist = (:artist)" );
        query.bindValue(":artwork",  path.isEmpty()?"NULL": path );
        query.bindValue(":title", info.at(0));
        query.bindValue(":artist", info.at(1));

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


QString CollectionDB::getArtistArt(QString artist)
{
    QString artistHead;

    QSqlQuery queryHead("SELECT * FROM artists WHERE title = \""+artist+"\"");

    while (queryHead.next())
        if(!queryHead.value(1).toString().isEmpty()&&queryHead.value(1).toString()!="NULL")
            artistHead = queryHead.value(1).toString();

    return artistHead;
}

QString CollectionDB::getAlbumArt(QString album, QString artist)
{
    QString albumCover;

    QSqlQuery queryCover ("SELECT * FROM albums WHERE title = \""+album+"\" AND artist = \""+artist+"\"");
    while (queryCover.next())
        if(!queryCover.value(2).toString().isEmpty()&&queryCover.value(2).toString()!="NULL")
            albumCover = queryCover.value(2).toString();

    return albumCover;
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QStringList &urls)
{
    QList<QMap<int, QString>> mapList;

    for(auto url:urls)
    {
        QString query("SELECT * FROM tracks WHERE location =\""+url+"\"");
        mapList+= this->getTrackData(query);
    }

    return mapList;
}

QList<QMap<int, QString>> CollectionDB::getTrackData(const QString &queryText)
{
    QList<QMap<int, QString>> mapList;
    QSqlQuery query;
    query.prepare(queryText);
    // qDebug()<<queryText;
    if(query.exec())
    {
        //qDebug()<<"getTrackData query passed";
        while(query.next())
        {
            QString track = query.value(BaeUtils::TracksCols::TRACK).toString();
            QString title = query.value(BaeUtils::TracksCols::TITLE).toString();
            QString artist = query.value(BaeUtils::TracksCols::ARTIST).toString();
            QString album = query.value(BaeUtils::TracksCols::ALBUM).toString();
            QString genre = query.value(BaeUtils::TracksCols::GENRE).toString();
            QString location = query.value(BaeUtils::TracksCols::URL).toString();
            QString stars = query.value(BaeUtils::TracksCols::STARS).toString();
            QString babe = query.value(BaeUtils::TracksCols::BABE).toString();
            QString art = query.value(BaeUtils::TracksCols::ART).toString();
            QString played = query.value(BaeUtils::TracksCols::PLAYED).toString();
            QString addDate = query.value(BaeUtils::TracksCols::ADD_DATE).toString();
            QString releaseDate = query.value(BaeUtils::TracksCols::RELEASE_DATE).toString();
            QString duration = query.value(BaeUtils::TracksCols::DURATION).toString();

            const QMap<int, QString> map{{BaeUtils::TracksCols::TRACK,track}, {BaeUtils::TracksCols::TITLE,title}, {BaeUtils::TracksCols::ARTIST,artist},{BaeUtils::TracksCols::ALBUM,album},{BaeUtils::TracksCols::DURATION,duration},{BaeUtils::TracksCols::GENRE,genre},{BaeUtils::TracksCols::URL,location},{BaeUtils::TracksCols::STARS,stars},{BaeUtils::TracksCols::BABE,babe},{BaeUtils::TracksCols::ART,art},{BaeUtils::TracksCols::PLAYED,played},{BaeUtils::TracksCols::ADD_DATE,addDate},{BaeUtils::TracksCols::RELEASE_DATE,releaseDate}};

            mapList<<map;
        }
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

bool CollectionDB::check_existance(QString tableName, QString searchId, QString search)
{
    QSqlQuery query;
    query.prepare("SELECT "+ searchId +" FROM "+tableName+" WHERE "+searchId+" = (:search)");
    query.bindValue(":search", search);

    if (query.exec())
    {
        if (query.next())
        {
            qDebug()<< "song does exists in db";
            return true;
        }else
        {
            qDebug()<<"song does not exists in db";
            return false;
        }


    }else return false;



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
        query.prepare("UPDATE "+tableName+" SET "+column+" = (:value) WHERE location = (:location)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");

        query.bindValue(":value", value);
        query.bindValue(":location", location);
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


