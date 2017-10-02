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


void CollectionDB::addTrack(const Bae::DB &track)
{
    QSqlQuery query;
    if(query.exec("PRAGMA synchronous=OFF"))
    {

        auto url = track[Bae::DBCols::URL];
        auto title = track[Bae::DBCols::TITLE];
        auto artist = track[Bae::DBCols::ARTIST];
        auto album = track[Bae::DBCols::ALBUM];
        auto genre = track[Bae::DBCols::GENRE];
        auto year = track[Bae::DBCols::RELEASE_DATE];
        auto sourceUrl = track[Bae::DBCols::SOURCES_URL];
        auto duration = track[Bae::DBCols::DURATION];
        auto babe = track[Bae::DBCols::BABE];
        auto trackNumber = track[Bae::DBCols::TRACK];

        qDebug()<<"started writing to database...";

        qDebug()<< "writting to db: "<<title<<artist;
        /* first needs to insert album and artist*/
        QVariantMap sourceMap {{Bae::DBColsMap[Bae::DBCols::URL],sourceUrl},
                               {Bae::DBColsMap[Bae::DBCols::SOURCE_TYPE], sourceType(url)}};

        this->insert(Bae::DBTablesMap[Bae::DBTables::SOURCES],sourceMap);

        QVariantMap artistMap {{Bae::DBColsMap[Bae::DBCols::ARTIST], artist},
                               {Bae::DBColsMap[Bae::DBCols::ARTWORK],""},
                               {Bae::DBColsMap[Bae::DBCols::WIKI],""}};

        this->insert(Bae::DBTablesMap[Bae::DBTables::ARTISTS],artistMap);

        QVariantMap albumMap {{Bae::DBColsMap[Bae::DBCols::ALBUM],album},
                              {Bae::DBColsMap[Bae::DBCols::ARTIST],artist},
                              {Bae::DBColsMap[Bae::DBCols::ARTWORK],""},
                              {Bae::DBColsMap[Bae::DBCols::WIKI],""}};
        this->insert(Bae::DBTablesMap[Bae::DBTables::ALBUMS],albumMap);

        QVariantMap trackMap {{Bae::DBColsMap[Bae::DBCols::URL],url},
                              {Bae::DBColsMap[Bae::DBCols::SOURCES_URL],sourceUrl},
                              {Bae::DBColsMap[Bae::DBCols::TRACK],trackNumber},
                              {Bae::DBColsMap[Bae::DBCols::TITLE],title},
                              {Bae::DBColsMap[Bae::DBCols::ARTIST],artist},
                              {Bae::DBColsMap[Bae::DBCols::ALBUM],album},
                              {Bae::DBColsMap[Bae::DBCols::DURATION],duration},
                              {Bae::DBColsMap[Bae::DBCols::PLAYED],0},
                              {Bae::DBColsMap[Bae::DBCols::BABE],babe},
                              {Bae::DBColsMap[Bae::DBCols::STARS],0},
                              {Bae::DBColsMap[Bae::DBCols::RELEASE_DATE],year},
                              {Bae::DBColsMap[Bae::DBCols::ADD_DATE],QDateTime::currentDateTime()},
                              {Bae::DBColsMap[Bae::DBCols::LYRICS],""},
                              {Bae::DBColsMap[Bae::DBCols::GENRE],genre},
                              {Bae::DBColsMap[Bae::DBCols::ART],""}};

        this->insert(Bae::DBTablesMap[Bae::DBTables::TRACKS],trackMap);
        emit trackInserted();

    }
}

bool CollectionDB::rateTrack(const QString &path, const int &value)
{
    if(this->update(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                    Bae::DBColsMap[Bae::DBCols::STARS],
                    value,
                    Bae::DBColsMap[Bae::DBCols::URL],
                    path)) return true;
    return false;
}

bool CollectionDB::babeTrack(const QString &path, const bool &value)
{
    if(this->update(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                    Bae::DBColsMap[Bae::DBCols::BABE],
                    value?1:0,
                    Bae::DBColsMap[Bae::DBCols::URL],
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
                    Bae::DBColsMap[Bae::DBCols::ART],
                    value,
                    Bae::DBColsMap[Bae::DBCols::URL],
                    path)) return true;
    return false;
}

bool CollectionDB::lyricsTrack(const QString &path, const QString &value)
{
    if(this->update(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                    Bae::DBColsMap[Bae::DBCols::LYRICS],
                    value,
                    Bae::DBColsMap[Bae::DBCols::URL],
                    path)) return true;
    return false;
}

bool CollectionDB::tagsTrack(const QString &path, const QString &value)
{
    return false;
}

bool CollectionDB::playedTrack(const QString &url, const int &increment)
{
    auto queryTxt = QString("UPDATE %1 SET %2 = %2 + %3 WHERE %4 = \"%5\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::PLAYED], QString::number(increment), Bae::DBColsMap[Bae::DBCols::URL],url);

    QSqlQuery query(queryTxt);

    if(query.exec())
        return true;

    return false;
}

bool CollectionDB::wikiArtist(const QString &artist, const QString &value)
{
    if(this->update(Bae::DBTablesMap[Bae::DBTables::ARTISTS],
                    Bae::DBColsMap[Bae::DBCols::WIKI],
                    value,
                    Bae::DBColsMap[Bae::DBCols::ARTIST],
                    artist)) return true;
    return false;
}

bool CollectionDB::tagsArtist(const QString &artist, const QString &value)
{
    QVariantMap tagMap {{Bae::DBColsMap[Bae::DBCols::TAG],value}};

    QVariantMap artistTagMap {{Bae::DBColsMap[Bae::DBCols::TAG],value},
                              {Bae::DBColsMap[Bae::DBCols::ARTIST],artist}};

    if(this->insert(Bae::DBTablesMap[Bae::DBTables::TAGS],tagMap) && this->insert(Bae::DBTablesMap[Bae::DBTables::ARTISTS_TAGS],artistTagMap))
        return true;

    return false;
}

bool CollectionDB::wikiAlbum(const QString &album, const QString &artist,  QString value)
{
    auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\" AND %6 = \"%7\"").arg(
                Bae::DBTablesMap[Bae::DBTables::ALBUMS],
            Bae::DBColsMap[Bae::DBCols::WIKI], value.replace("\"","\"\""),
            Bae::DBColsMap[Bae::DBCols::ALBUM],
            album,Bae::DBColsMap[Bae::DBCols::ARTIST], artist);

    qDebug()<<queryStr;
    QSqlQuery query(queryStr);

    if(query.exec())
        return true;

    query.lastError().text();
    return false;
}

bool CollectionDB::tagsAlbum(const QString &album, const QString &artist, const QString &value)
{

    QVariantMap tagMap {{Bae::DBColsMap[Bae::DBCols::TAG],value}};

    QVariantMap albumsTagMap {{Bae::DBColsMap[Bae::DBCols::TAG],value},
                              {Bae::DBColsMap[Bae::DBCols::ARTIST],artist},
                              {Bae::DBColsMap[Bae::DBCols::ALBUM],album}};

    if(this->insert(Bae::DBTablesMap[Bae::DBTables::TAGS],tagMap) && this->insert(Bae::DBTablesMap[Bae::DBTables::ALBUMS_TAGS],albumsTagMap))
        return true;

    return false;
}

bool CollectionDB::addPlaylist(const QString &title)
{
    if(!title.isEmpty())
    {
        QVariantMap playlist {{Bae::DBColsMap[Bae::DBCols::PLAYLIST],title},
                              {Bae::DBColsMap[Bae::DBCols::ADD_DATE],QDate::currentDate()}};

        if(this->insert(Bae::DBTablesMap[Bae::DBTables::PLAYLISTS],playlist))
            return true;
    }

    return false;
}

bool CollectionDB::trackPlaylist(const QString &url, const QString &playlist)
{
    QVariantMap map {{Bae::DBColsMap[Bae::DBCols::PLAYLIST],playlist},
                     {Bae::DBColsMap[Bae::DBCols::URL],url},
                     {Bae::DBColsMap[Bae::DBCols::ADD_DATE],QDate::currentDate()}};

    if(this->insert(Bae::DBTablesMap[Bae::DBTables::TRACKS_PLAYLISTS],map))
        return true;

    return false;
}

Bae::DB_LIST CollectionDB::getTrackData(const QStringList &urls)
{
    Bae::DB_LIST mapList;

    for(auto url:urls)
    {
        QSqlQuery query;
        auto queryTxt = QString("SELECT * FROM %1 WHERE %2 = \"%3\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                Bae::DBColsMap[Bae::DBCols::URL],url);
        query.prepare(queryTxt);
        mapList<<this->getTrackData(query);
    }

    return mapList;
}

Bae::DB_LIST CollectionDB::getTrackData(QSqlQuery &query)
{
    //    qDebug()<<"on getTrackData "<<queryText;
    Bae::DB_LIST mapList;

    if(query.exec())
        while(query.next())
            mapList << Bae::DB
            {
            {Bae::DBCols::TRACK, query.value(Bae::DBColsMap[Bae::DBCols::TRACK]).toString()},
{Bae::DBCols::TITLE, query.value(Bae::DBColsMap[Bae::DBCols::TITLE]).toString()},
{Bae::DBCols::ARTIST, query.value(Bae::DBColsMap[Bae::DBCols::ARTIST]).toString()},
{Bae::DBCols::ALBUM, query.value(Bae::DBColsMap[Bae::DBCols::ALBUM]).toString()},
{Bae::DBCols::DURATION, query.value(Bae::DBColsMap[Bae::DBCols::DURATION]).toString()},
{Bae::DBCols::GENRE, query.value(Bae::DBColsMap[Bae::DBCols::GENRE]).toString()},
{Bae::DBCols::URL, query.value(Bae::DBColsMap[Bae::DBCols::URL]).toString()},
{Bae::DBCols::STARS, query.value(Bae::DBColsMap[Bae::DBCols::STARS]).toString()},
{Bae::DBCols::BABE, query.value(Bae::DBColsMap[Bae::DBCols::BABE]).toString()},
{Bae::DBCols::ART, query.value(Bae::DBColsMap[Bae::DBCols::ART]).toString()},
{Bae::DBCols::PLAYED, query.value(Bae::DBColsMap[Bae::DBCols::PLAYED]).toString()},
{Bae::DBCols::ADD_DATE, query.value(Bae::DBColsMap[Bae::DBCols::ADD_DATE]).toString()},
{Bae::DBCols::RELEASE_DATE, query.value(Bae::DBColsMap[Bae::DBCols::RELEASE_DATE]).toString()},
{Bae::DBCols::LYRICS, query.value(Bae::DBColsMap[Bae::DBCols::LYRICS]).toString()},
{Bae::DBCols::SOURCES_URL, query.value(Bae::DBColsMap[Bae::DBCols::SOURCES_URL]).toString()}
};

return mapList;
}

Bae::DB_LIST CollectionDB::getArtistData(QSqlQuery &query)
{
    Bae::DB_LIST mapList;

    if(query.exec())
        while(query.next())
            mapList << Bae::DB
            {{Bae::DBCols::ARTIST,query.value(Bae::DBColsMap[Bae::DBCols::ARTIST]).toString()},
{Bae::DBCols::ARTWORK,query.value(Bae::DBColsMap[Bae::DBCols::ARTWORK]).toString()},
{Bae::DBCols::WIKI,query.value(Bae::DBColsMap[Bae::DBCols::WIKI]).toString()}};

return mapList;
}

Bae::DB_LIST CollectionDB::getAlbumData(QSqlQuery &query)
{
    Bae::DB_LIST mapList;

    if(query.exec())
        while(query.next())
            mapList << Bae::DB
            {{Bae::DBCols::ALBUM,query.value(Bae::DBColsMap[Bae::DBCols::ALBUM]).toString()},
{Bae::DBCols::ARTIST,query.value(Bae::DBColsMap[Bae::DBCols::ARTIST]).toString()},
{Bae::DBCols::ARTWORK,query.value(Bae::DBColsMap[Bae::DBCols::ARTWORK]).toString()},
{Bae::DBCols::WIKI,query.value(Bae::DBColsMap[Bae::DBCols::WIKI]).toString()}};

return mapList;
}

Bae::DB_LIST CollectionDB::getAlbumTracks(const QString &album, const QString &artist, const Bae::DBCols &orderBy, const Bae::Order &order)
{
    auto queryTxt = QString("SELECT * FROM %1 WHERE %2 = \"%3\" AND %4 = \"%5\" ORDER by %6 %7").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::ARTIST],artist,Bae::DBColsMap[Bae::DBCols::ALBUM],album,Bae::DBColsMap[orderBy], Bae::OrderMap[order]);
    QSqlQuery query(queryTxt);

    return this->getTrackData(query);
}

Bae::DB_LIST CollectionDB::getArtistTracks(const QString &artist, const Bae::DBCols &orderBy, const Bae::Order &order)
{
    auto queryTxt = QString("SELECT * FROM %1 WHERE %2 = \"%3\" ORDER by %4 %5, %6 %5").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::ARTIST],artist,Bae::DBColsMap[orderBy],Bae::OrderMap[order],Bae::DBColsMap[Bae::DBCols::TRACK]);

    QSqlQuery query(queryTxt);

    return this->getTrackData(query);

}

QStringList CollectionDB::getArtistAlbums(const QString &artist)
{
    QStringList albums;

    auto queryTxt = QString("SELECT %4 FROM %1 WHERE %2 = \"%3\" ORDER BY %4 ASC").arg(Bae::DBTablesMap[Bae::DBTables::ALBUMS],
            Bae::DBColsMap[Bae::DBCols::ARTIST],artist,Bae::DBColsMap[Bae::DBCols::ALBUM]);
    QSqlQuery query(queryTxt);
    if(query.exec())
        while(query.next())
            albums<<query.value(Bae::DBColsMap[Bae::DBCols::ALBUM]).toString();
    return albums;
}

Bae::DB_LIST CollectionDB::getBabedTracks(const Bae::DBCols &orderBy, const Bae::Order &order)
{
    auto queryTxt= QString("SELECT * FROM %1 WHERE %2 = 1 ORDER by %3 %4").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::BABE],
            Bae::DBColsMap[orderBy], Bae::OrderMap[order]);
    QSqlQuery query(queryTxt);

    return this->getTrackData(query);
}

Bae::DB_LIST CollectionDB::getSearchedTracks(const Bae::DBCols &where, const QString &search)
{
    QString queryTxt;

    if(where == Bae::DBCols::PLAYED || where == Bae::DBCols::STARS || where == Bae::DBCols::BABE)
        queryTxt= QString("SELECT * FROM %1 WHERE %2 = \"%3\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                Bae::DBColsMap[where],
                search);
    else if(where == Bae::DBCols::WIKI)

        queryTxt= QString("SELECT DISTINCT t.* FROM %1 t INNER JOIN %2 al ON t.%3 = al.%3 INNER JOIN %4 ar ON t.%5 = ar.%5 WHERE al.%6 LIKE \"%%7%\" OR ar.%6 LIKE \"%%7%\" COLLATE NOCASE").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                Bae::DBTablesMap[Bae::DBTables::ALBUMS],Bae::DBColsMap[Bae::DBCols::ALBUM],
                Bae::DBTablesMap[Bae::DBTables::ARTISTS],Bae::DBColsMap[Bae::DBCols::ARTIST],
                Bae::DBColsMap[where],
                search);


    else if(where == Bae::DBCols::PLAYLIST)

        queryTxt= QString("SELECT t.* FROM %1 t INNER JOIN %2 tp ON t.%3 = tp.%3 WHERE tp.%4 = \"%5\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                Bae::DBTablesMap[Bae::DBTables::TRACKS_PLAYLISTS],Bae::DBColsMap[Bae::DBCols::URL],
                Bae::DBColsMap[where],
                search);


    else if(where == Bae::DBCols::TAG)

        queryTxt= QString("SELECT t.* FROM %1 t INNER JOIN %2 tt ON t.%3 = tt.%3 WHERE tt.%4 = \"%5\" COLLATE NOCASE "
                          "UNION "
                          "SELECT t.* FROM %1 t INNER JOIN %6 at ON t.%7 = at.%7 AND t.%8 = at.%8 WHERE at.%4 = \"%5\" COLLATE NOCASE "
                          "UNION "
                          "SELECT t.* FROM %1 t INNER JOIN %9 art ON t.%8 = art.%8 WHERE art.%4 LIKE \"%%5%\" COLLATE NOCASE "
                          "UNION "
                          "SELECT DISTINCT t.* FROM %1 t INNER JOIN %9 at ON t.%8 = at.%4 WHERE at.%8 LIKE \"%%5%\" COLLATE NOCASE").arg(
                    Bae::DBTablesMap[Bae::DBTables::TRACKS],
                Bae::DBTablesMap[Bae::DBTables::TRACKS_TAGS],
                Bae::DBColsMap[Bae::DBCols::URL],
                Bae::DBColsMap[where],
                search,
                Bae::DBTablesMap[Bae::DBTables::ALBUMS_TAGS],
                Bae::DBColsMap[Bae::DBCols::ALBUM],
                Bae::DBColsMap[Bae::DBCols::ARTIST],
                Bae::DBTablesMap[Bae::DBTables::ARTISTS_TAGS]);

    else
        queryTxt= QString("SELECT * FROM %1 WHERE %2 LIKE \"%%3%\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
                Bae::DBColsMap[where],
                search);

    qDebug()<<"SEARCH QUERY:"<<queryTxt;

    QSqlQuery query(queryTxt);

    return this->getTrackData(query);

}

Bae::DB_LIST CollectionDB::getPlaylistTracks(const QString &playlist, const Bae::DBCols &orderBy, const Bae::Order &order)
{
    auto queryTxt= QString("SELECT * FROM %1 t INNER JOIN %2 tp ON t.%3 = tp.%3 WHERE tp.%4 = \"%5\" ORDER BY %6 %7").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBTablesMap[Bae::DBTables::TRACKS_PLAYLISTS],Bae::DBColsMap[Bae::DBCols::URL],
            Bae::DBColsMap[Bae::DBCols::PLAYLIST],
            playlist,Bae::DBColsMap[orderBy],Bae::OrderMap[order],Bae::DBColsMap[Bae::DBCols::TRACK] );

    QSqlQuery query(queryTxt);

    return this->getTrackData(query);
}

Bae::DB_LIST CollectionDB::getFavTracks(const int &stars, const int &limit, const Bae::DBCols &orderBy, const Bae::Order &order)
{
    auto queryTxt= QString("SELECT * FROM %1 WHERE %2 >= %3 ORDER BY %4 %5 LIMIT %6" ).arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::STARS], QString::number(stars),
            Bae::DBColsMap[orderBy],Bae::OrderMap[order],QString::number(limit));

    QSqlQuery query(queryTxt);

    return this->getTrackData(query);
}

Bae::DB_LIST CollectionDB::getRecentTracks(const int &limit, const Bae::DBCols &orderBy, const Bae::Order &order)
{
    auto queryTxt= QString("SELECT * FROM %1 ORDER BY strftime(\"%s\",%2) %3 LIMIT %4" ).arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[orderBy],Bae::OrderMap[order],QString::number(limit));

    QSqlQuery query(queryTxt);

    return this->getTrackData(query);
}

Bae::DB_LIST CollectionDB::getOnlineTracks(const Bae::DBCols &orderBy, const Bae::Order &order)
{
    auto queryTxt= QString("SELECT * FROM %1 WHERE %2 LIKE \"%3%\" ORDER BY %4 %5" ).arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::URL],Bae::YoutubeCachePath,Bae::DBColsMap[orderBy],Bae::OrderMap[order]);

    QSqlQuery query(queryTxt);

    return this->getTrackData(query);
}

Bae::DB_LIST CollectionDB::getMostPlayedTracks(const int &greaterThan, const int &limit, const Bae::DBCols &orderBy, const Bae::Order &order)
{
    auto queryTxt= QString("SELECT * FROM %1 WHERE %2 > %3 ORDER BY %4 %5 LIMIT %6" ).arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::PLAYED], QString::number(greaterThan),
            Bae::DBColsMap[orderBy],Bae::OrderMap[order],QString::number(limit));

    QSqlQuery query(queryTxt);

    return this->getTrackData(query);
}

QString CollectionDB::getTrackLyrics(const QString &url)
{
    QString lyrics;
    QSqlQuery query(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(Bae::DBColsMap[Bae::DBCols::LYRICS],
                    Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::URL],url));

    if(query.exec())
        while (query.next())
            lyrics=query.value(0).toString();

    return lyrics;
}


QString CollectionDB::getTrackArt(const QString &path)
{
    QString color;
    QSqlQuery query(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(Bae::DBColsMap[Bae::DBCols::ART],
                    Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::URL],path));

    if(query.exec())
        while (query.next())
            color=query.value(0).toString();

    return color;
}

QStringList CollectionDB::getTrackTags(const QString &path)
{

}

QString CollectionDB::getArtistArt(const QString &artist)
{
    QString artistHead;

    QSqlQuery queryHead(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(Bae::DBColsMap[Bae::DBCols::ARTWORK],
                        Bae::DBTablesMap[Bae::DBTables::ARTISTS],
            Bae::DBColsMap[Bae::DBCols::ARTIST],artist));

    while (queryHead.next())
        if(!queryHead.value(0).toString().isEmpty()&&queryHead.value(0).toString()!="NULL")
            artistHead = queryHead.value(0).toString();

    return artistHead;
}

QString CollectionDB::getArtistWiki(const QString &artist)
{
    QString wiki;

    QSqlQuery query(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(Bae::DBColsMap[Bae::DBCols::WIKI],
                    Bae::DBTablesMap[Bae::DBTables::ARTISTS],
            Bae::DBColsMap[Bae::DBCols::ARTIST],artist));

    if(query.exec())
        while (query.next())
            wiki = query.value(0).toString();

    return wiki;
}

QStringList CollectionDB::getArtistTags(const QString &artist)
{
    QStringList tags;

    QSqlQuery query(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(Bae::DBColsMap[Bae::DBCols::TAG],
                    Bae::DBTablesMap[Bae::DBTables::ARTISTS_TAGS],
            Bae::DBColsMap[Bae::DBCols::ARTIST],artist));
    if(query.exec())
        while (query.next())
            tags << query.value(0).toString();

    return tags;
}

QString CollectionDB::getAlbumArt(const QString &album, const QString &artist)
{
    QString albumCover;
    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\" AND %5 = \"%6\"").arg(Bae::DBColsMap[Bae::DBCols::ARTWORK],
            Bae::DBTablesMap[Bae::DBTables::ALBUMS],
            Bae::DBColsMap[Bae::DBCols::ALBUM],album,
            Bae::DBColsMap[Bae::DBCols::ARTIST],artist);
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
    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\" AND %5 = \"%6\"").arg(Bae::DBColsMap[Bae::DBCols::WIKI],
            Bae::DBTablesMap[Bae::DBTables::ALBUMS],
            Bae::DBColsMap[Bae::DBCols::ALBUM],album,
            Bae::DBColsMap[Bae::DBCols::ARTIST],artist);
    QSqlQuery query(queryStr);

    if(query.exec())
        while (query.next())
            wiki = query.value(0).toString();

    return wiki;
}

QStringList CollectionDB::getAlbumTags(const QString &album, const QString &artist)
{
    QStringList tags;

    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\" AND %5 = \"%6\"").arg(Bae::DBColsMap[Bae::DBCols::TAG],
            Bae::DBTablesMap[Bae::DBTables::ALBUMS_TAGS],
            Bae::DBColsMap[Bae::DBCols::ALBUM],album,
            Bae::DBColsMap[Bae::DBCols::ARTIST],artist);
    QSqlQuery query(queryStr);

    if(query.exec())
        while (query.next())
            tags << query.value(0).toString();

    return tags;
}

QStringList CollectionDB::getPlaylists()
{

    QStringList playlists;
    QSqlQuery query(QString("SELECT %1, %2 FROM %3 ORDER by %2 desc").arg(Bae::DBColsMap[Bae::DBCols::PLAYLIST],
                    Bae::DBColsMap[Bae::DBCols::ADD_DATE],
            Bae::DBTablesMap[Bae::DBTables::PLAYLISTS]));

    if (query.exec())
        while (query.next())
            playlists << query.value(0).toString();

    return playlists;

}

bool CollectionDB::removeTrack(const QString &path)
{
    auto queryTxt = QString("DELETE FROM %1 WHERE %2 =  \"%3\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::URL],path);
    QSqlQuery query(queryTxt);
    if(query.exec()) return true;

    return false;
}

bool CollectionDB::removeSource(const QString &path)
{
    auto queryTxt = QString("DELETE FROM %1 WHERE %2 =  \"%3\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS],
            Bae::DBColsMap[Bae::DBCols::SOURCES_URL],path);
    QSqlQuery query(queryTxt);
    if(query.exec())
    {
        queryTxt = QString("DELETE FROM %1 WHERE %2 =  \"%3\"").arg(Bae::DBTablesMap[Bae::DBTables::SOURCES],
                Bae::DBColsMap[Bae::DBCols::SOURCES_URL],path);
        query.prepare(queryTxt);
        if(query.exec()) return true;
    }

    return false;
}





CollectionDB::sourceTypes CollectionDB::sourceType(const QString &url)
{
    /*for now*/

    return sourceTypes::LOCAL;
}


QSqlQuery CollectionDB::getQuery(const QString &queryTxt)
{
    QSqlQuery query(queryTxt);
    return query;
}



/*******************OLD STUFF********************/








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
        QString artist = query.value(Bae::DBCols::ARTIST).toString();
        QString album = query.value(Bae::DBCols::ALBUM).toString();
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
            QString artist = query.value(Bae::DBCols::ARTIST).toString();
            QString file = query.value(Bae::DBCols::URL).toString();
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

void CollectionDB::insertArtwork(const Bae::DB &track)
{

    auto artist = track[Bae::DBCols::ARTIST];
    auto album =track[Bae::DBCols::ALBUM];
    auto path = track[Bae::DBCols::ARTWORK];
    qDebug()<<"the path:"<<path<<"the list:"<<track[Bae::DBCols::ARTIST]<<track[Bae::DBCols::ALBUM];

    if(!artist.isEmpty() && !album.isEmpty())
    {
        auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\" AND %6 = \"%7\"").arg(Bae::DBTablesMap[Bae::DBTables::ALBUMS],
                Bae::DBColsMap[Bae::DBCols::ARTWORK],
                path.isEmpty() ? "NULL": path,
                Bae::DBColsMap[Bae::DBCols::ALBUM],
                album,
                Bae::DBColsMap[Bae::DBCols::ARTIST],
                artist);

        QSqlQuery query(queryStr);
        if(query.exec()) qDebug()<<"Artwork[cover] inserted into DB"<<artist<<album;
        else qDebug()<<"COULDNT Artwork[cover] inerted into DB"<<artist<<album;

    }else if(!artist.isEmpty()&&album.isEmpty())
    {
        auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\"").arg(Bae::DBTablesMap[Bae::DBTables::ARTISTS],
                Bae::DBColsMap[Bae::DBCols::ARTWORK],
                path.isEmpty() ? "NULL": path,
                Bae::DBColsMap[Bae::DBCols::ARTIST],
                artist);
        QSqlQuery query(queryStr);
        if(query.exec()) qDebug()<<"Artwork[head] inserted into DB"<<artist;
        else qDebug()<<"COULDNT Artwork[head] inerted into DB"<<artist;
    }
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

bool CollectionDB::removePlaylistTrack(const QString &url, const QString &playlist)
{
    auto queryTxt = QString("DELETE FROM %1 WHERE %2 = \"%3\" AND %4 = \"%5\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS_PLAYLISTS],
            Bae::DBColsMap[Bae::DBCols::PLAYLIST],playlist,Bae::DBColsMap[Bae::DBCols::URL],url);
    QSqlQuery query(queryTxt);
    if(!query.exec()) return false;
    return true;
}

bool CollectionDB::removePlaylist(const QString &playlist)
{
    QSqlQuery query;
    QString queryTxt;
    queryTxt = QString("DELETE FROM %1 WHERE %2 = \"%3\"").arg(Bae::DBTablesMap[Bae::DBTables::TRACKS_PLAYLISTS],
            Bae::DBColsMap[Bae::DBCols::PLAYLIST],playlist);

    query.prepare(queryTxt);
    if(!query.exec()) return false;

    queryTxt = QString("DELETE FROM %1 WHERE %2 = \"%3\"").arg(Bae::DBTablesMap[Bae::DBTables::PLAYLISTS],
            Bae::DBColsMap[Bae::DBCols::PLAYLIST],playlist);

    query.prepare(queryTxt);
    if(!query.exec()) return false;
    return true;
}


