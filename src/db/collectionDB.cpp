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
#include <QUuid>

#include "../pulpo/enums.h"

using namespace BAE;

CollectionDB::CollectionDB(QObject *parent) : QObject(parent)
{
    this->name = QUuid::createUuid().toString();
    if(!BAE::fileExists(BAE::CollectionDBPath + BAE::DBName))
    {
        this->openDB(this->name);
        qDebug()<<"Collection doesn't exists, trying to create it" << BAE::CollectionDBPath + BAE::DBName;
        this->prepareCollectionDB();
    }else this->openDB(this->name);
}

CollectionDB::CollectionDB(const QString &name, QObject *parent) : QObject(parent)
{
    this->name = name.isEmpty()? QUuid::createUuid().toString() : name;
    qDebug()<<"Creating DB for: "<<this->name;

    if(!BAE::fileExists(BAE::CollectionDBPath + BAE::DBName))
    {
        qDebug()<<"Collection doesn't exists, trying to create it" << BAE::CollectionDBPath + BAE::DBName;
        this->openDB(this->name);
        this->prepareCollectionDB();
    }
}

CollectionDB::~CollectionDB()
{
    this->m_db.close();
}

void CollectionDB::closeConnection()
{
    qDebug()<<"CLOSING COLLECTIONDB";
}

void CollectionDB::prepareCollectionDB() const
{
    QSqlQuery query(this->m_db);

    QFile file(":/Data/src/db/script.sql");

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
            if (!query.exec(line))
            {
                qDebug()<<"exec failed"<<query.lastQuery()<<query.lastError();
            }

        } else qDebug()<<"exec wrong"<<query.lastError();
    }
    file.close();
}

bool CollectionDB::check_existance(const QString &tableName, const QString &searchId,const QString &search)
{
    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(searchId, tableName, searchId, search);
    auto query = this->getQuery(queryStr);

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

    QString sqlQueryString = "INSERT INTO " + tableName + " (" + QString(fields.join(",")) + ") VALUES(" + QString(strValues.join(",")) + ")";
    QSqlQuery query(this->m_db);
    query.prepare(sqlQueryString);

    int k = 0;
    foreach (const QVariant &value, values)
        query.bindValue(k++, value);

    return query.exec();
}

bool CollectionDB::update(const QString &tableName, const BAE::DB &updateData, const QVariantMap &where)
{
    if (tableName.isEmpty())
    {
        qDebug()<<QStringLiteral("Fatal error on insert! The table name is empty!");
        return false;
    } else if (updateData.isEmpty())
    {
        qDebug()<<QStringLiteral("Fatal error on insert! The insertData is empty!");
        return false;
    }

    QStringList set;
    for (auto key : updateData.keys())
        set.append(KEYMAP[key]+" = '"+updateData[key]+"'");

    QStringList condition;
    for (auto key : where.keys())
        condition.append(key+" = '"+where[key].toString()+"'");

    QString sqlQueryString = "UPDATE " + tableName + " SET " + QString(set.join(",")) + " WHERE " + QString(condition.join(",")) ;
    auto query = this->getQuery(sqlQueryString);
    qDebug()<<sqlQueryString;
    return query.exec();
}

bool CollectionDB::update(const QString &table, const QString &column, const QVariant &newValue, const QVariant &op, const QString &id)
{
    auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\"").arg(table, column, newValue.toString().replace("\"","\"\""), op.toString(), id);
    auto query = this->getQuery(queryStr);
    return query.exec();
}

bool CollectionDB::execQuery(QSqlQuery &query) const
{
    if(query.exec()) return true;
    qDebug()<<"ERROR ON EXEC QUERY";
    qDebug()<<query.lastError()<<query.lastQuery();
    return false;
}

bool CollectionDB::execQuery(const QString &queryTxt)
{
    auto query = this->getQuery(queryTxt);
    return query.exec();
}

void CollectionDB::openDB(const QString &name)
{
    if(!QSqlDatabase::contains(name))
    {
        this->m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), name);
        this->m_db.setDatabaseName(BAE::CollectionDBPath + BAE::DBName);
    }

    if (!this->m_db.isOpen())
    {
        if(!this->m_db.open())
            qDebug()<<"ERROR OPENING DB"<<this->m_db.lastError().text()<<m_db.connectionName();
    }
}

void CollectionDB::addTrack(const DB &track)
{
    auto query = this->getQuery("PRAGMA synchronous=OFF");
    if(query.exec())
    {
        auto url = track[KEY::URL];
        auto title = track[KEY::TITLE];
        auto artist = track[KEY::ARTIST];
        auto album = track[KEY::ALBUM];
        auto genre = track[KEY::GENRE];
        auto year = track[KEY::RELEASE_DATE];
        auto sourceUrl = track[KEY::SOURCES_URL];
        auto duration = track[KEY::DURATION];
        auto babe = track[KEY::BABE];
        auto trackNumber = track[KEY::TRACK];

        auto artwork = track[KEY::ARTWORK].isEmpty()? "" : track[KEY::ARTWORK];

        qDebug()<< "writting to db: "<<title<<artist;
        /* first needs to insert album and artist*/
        QVariantMap sourceMap {{KEYMAP[KEY::URL],sourceUrl},
                               {KEYMAP[KEY::SOURCE_TYPE], sourceType(url)}};

        insert(TABLEMAP[TABLE::SOURCES],sourceMap);

        QVariantMap artistMap {{KEYMAP[KEY::ARTIST], artist},
                               {KEYMAP[KEY::ARTWORK], ""},
                               {KEYMAP[KEY::WIKI],""}};

        insert(TABLEMAP[TABLE::ARTISTS],artistMap);

        QVariantMap albumMap {{KEYMAP[KEY::ALBUM], album},
                              {KEYMAP[KEY::ARTIST], artist},
                              {KEYMAP[KEY::ARTWORK], artwork},
                              {KEYMAP[KEY::WIKI],""}};
        insert(TABLEMAP[TABLE::ALBUMS],albumMap);

        QVariantMap trackMap {{KEYMAP[KEY::URL], url},
                              {KEYMAP[KEY::SOURCES_URL], sourceUrl},
                              {KEYMAP[KEY::TRACK], trackNumber},
                              {KEYMAP[KEY::TITLE], title},
                              {KEYMAP[KEY::ARTIST], artist},
                              {KEYMAP[KEY::ALBUM], album},
                              {KEYMAP[KEY::DURATION], duration},
                              {KEYMAP[KEY::PLAYED], 0},
                              {KEYMAP[KEY::BABE], babe},
                              {KEYMAP[KEY::STARS], 0},
                              {KEYMAP[KEY::RELEASE_DATE], year},
                              {KEYMAP[KEY::ADD_DATE], QDateTime::currentDateTime()},
                              {KEYMAP[KEY::LYRICS],""},
                              {KEYMAP[KEY::GENRE], genre},
                              {KEYMAP[KEY::ART], ""},
                              {KEYMAP[KEY::WIKI], ""},
                              {KEYMAP[KEY::COMMENT], ""}};

        insert(TABLEMAP[TABLE::TRACKS],trackMap);
    }else
    {
        qDebug()<< "Failed to insert async";
    }

    emit trackInserted();
}

bool CollectionDB::updateTrack(const DB &track)
{
    if(this->check_existance(TABLEMAP[TABLE::TRACKS], KEYMAP[KEY::URL], track[KEY::URL]))
    {
        QVariantMap artistMap {{KEYMAP[KEY::ARTIST], track[KEY::ARTIST]},
                               {KEYMAP[KEY::ARTWORK], ""},
                               {KEYMAP[KEY::WIKI],""}};

        insert(TABLEMAP[TABLE::ARTISTS],artistMap);

        QVariantMap albumMap {{KEYMAP[KEY::ALBUM], track[KEY::ALBUM]},
                              {KEYMAP[KEY::ARTIST], track[KEY::ARTIST]},
                              {KEYMAP[KEY::ARTWORK], ""},
                              {KEYMAP[KEY::WIKI],""}};
        insert(TABLEMAP[TABLE::ALBUMS],albumMap);

        QVariantMap condition {{KEYMAP[KEY::URL], track[KEY::URL]}};

        if(this->update(TABLEMAP[TABLE::TRACKS], track, condition))
            if(cleanAlbums()) cleanArtists();

        return true;
    }

    return false;
}

bool CollectionDB::rateTrack(const QString &path, const int &value)
{
    if(update(TABLEMAP[TABLE::TRACKS],
              KEYMAP[KEY::STARS],
              value,
              KEYMAP[KEY::URL],
              path)) return true;
    return false;
}

bool CollectionDB::babeTrack(const QString &path, const bool &value)
{
    if(update(TABLEMAP[TABLE::TRACKS],
              KEYMAP[KEY::BABE],
              value?1:0,
              KEYMAP[KEY::URL],
              path)) return true;
    return false;
}

bool CollectionDB::moodTrack(const QString &path, const QString &value)
{
    Q_UNUSED(path);
    Q_UNUSED(value);
    return false;
}

bool CollectionDB::artTrack(const QString &path, const QString &value)
{
    if(update(TABLEMAP[TABLE::TRACKS],
              KEYMAP[KEY::ART],
              value,
              KEYMAP[KEY::URL],
              path)) return true;
    return false;
}

bool CollectionDB::lyricsTrack(const DB &track, const QString &value)
{
    if(update(TABLEMAP[TABLE::TRACKS],
              KEYMAP[KEY::LYRICS],
              value,
              KEYMAP[KEY::URL],
              track[KEY::URL])) return true;
    return false;
}

bool CollectionDB::tagsTrack(const DB &track, const QString &value, const QString &context)
{
    auto url = track[KEY::URL];

    qDebug()<<"INSERTIN TRACK TAG"<<value<<context<<url;

    QVariantMap tagMap {{KEYMAP[KEY::TAG],value},{KEYMAP[KEY::CONTEXT],context}};

    QVariantMap trackTagMap {{KEYMAP[KEY::TAG],value},
                             {KEYMAP[KEY::CONTEXT],context},
                             {KEYMAP[KEY::URL],url}};

    insert(TABLEMAP[TABLE::TAGS],tagMap);
    insert(TABLEMAP[TABLE::TRACKS_TAGS],trackTagMap);
    return true;
}

bool CollectionDB::albumTrack(const DB &track, const QString &value)
{
    auto album = track[KEY::ALBUM];
    auto artist = track[KEY::ARTIST];
    auto url = track[KEY::URL];

    auto queryTxt = QString("SELECT * %1 WHERE %2 = %3 AND %4 = %5").arg(TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::ALBUM],album, KEYMAP[KEY::ARTIST], artist);

    auto result = this->getDBData(queryTxt);
    if(result.isEmpty())  return false;

    auto oldAlbum = result.first();
    QVariantMap albumMap {{KEYMAP[KEY::ALBUM],value},
                          {KEYMAP[KEY::ARTIST],oldAlbum[KEY::ARTIST]},
                          {KEYMAP[KEY::ARTWORK],oldAlbum[KEY::ARTWORK]},
                          {KEYMAP[KEY::WIKI],oldAlbum[KEY::WIKI]}};

    if (!insert(TABLEMAP[TABLE::ALBUMS],albumMap)) return false;

    // update albums SET album = "newalbumname" WHERE album = "albumname" NAD artist = "aretist name";
    queryTxt = QString("UPDATE %1 SET %2 = %3 AND %4 = %5 WHERE %2 = %6 AND %4 = %5").arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::ALBUM],value,
            KEYMAP[KEY::ARTIST], oldAlbum[KEY::ARTIST],oldAlbum[KEY::ALBUM]);
    auto query = this->getQuery(queryTxt);

    if(!execQuery(query)) return false;

    queryTxt = QString("DELETE FROM %1 WHERE %2 = %3 AND %4 = %5").arg(TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::ALBUM],oldAlbum[KEY::ALBUM], KEYMAP[KEY::ARTIST], artist);
    query.prepare(queryTxt);

    if(!execQuery(query)) return false;

    return true;
}

bool CollectionDB::playedTrack(const QString &url, const int &increment)
{
    auto queryTxt = QString("UPDATE %1 SET %2 = %2 + %3 WHERE %4 = \"%5\"").arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::PLAYED], QString::number(increment), KEYMAP[KEY::URL],url);

    auto query = this->getQuery(queryTxt);

    if(query.exec())
        return true;

    return false;
}

bool CollectionDB::wikiTrack(const DB &track, const QString &value)
{
    auto url = track[KEY::URL];

    if(update(TABLEMAP[TABLE::TRACKS],
              KEYMAP[KEY::WIKI],
              value,
              KEYMAP[KEY::URL],
              url)) return true;

    return false;
}

bool CollectionDB::wikiArtist(const DB &track, const QString &value)
{
    auto artist = track[KEY::ARTIST];

    if(update(TABLEMAP[TABLE::ARTISTS],
              KEYMAP[KEY::WIKI],
              value,
              KEYMAP[KEY::ARTIST],
              artist)) return true;
    return false;
}

bool CollectionDB::tagsArtist(const DB &track, const QString &value, const QString &context)
{
    auto artist = track[KEY::ARTIST];

    QVariantMap tagMap {{KEYMAP[KEY::TAG],value},{KEYMAP[KEY::CONTEXT],context}};

    QVariantMap artistTagMap {{KEYMAP[KEY::TAG],value},
                              {KEYMAP[KEY::CONTEXT],context},
                              {KEYMAP[KEY::ARTIST],artist}};

    insert(TABLEMAP[TABLE::TAGS],tagMap);
    insert(TABLEMAP[TABLE::ARTISTS_TAGS],artistTagMap);

    return true;

}

bool CollectionDB::wikiAlbum(const DB &track,  QString value)
{
    auto artist = track[KEY::ARTIST];
    auto album = track[KEY::ALBUM];

    auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\" AND %6 = \"%7\"").arg(
                TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::WIKI], value.replace("\"","\"\""),
            KEYMAP[KEY::ALBUM],
            album,KEYMAP[KEY::ARTIST], artist);


    qDebug()<<queryStr;
    auto query = this->getQuery(queryStr);
    return query.exec();
}

bool CollectionDB::tagsAlbum(const DB &track, const QString &value, const QString &context)
{
    auto artist = track[KEY::ARTIST];
    auto album = track[KEY::ALBUM];

    QVariantMap tagMap {{KEYMAP[KEY::TAG],value},{KEYMAP[KEY::CONTEXT],context}};

    QVariantMap albumsTagMap {{KEYMAP[KEY::TAG],value},
                              {KEYMAP[KEY::CONTEXT],context},
                              {KEYMAP[KEY::ARTIST],artist},
                              {KEYMAP[KEY::ALBUM],album}};

    insert(TABLEMAP[TABLE::TAGS],tagMap);
    insert(TABLEMAP[TABLE::ALBUMS_TAGS],albumsTagMap);
    return true;
}

bool CollectionDB::addPlaylist(const QString &title)
{
    if(!title.isEmpty())
    {
        QVariantMap playlist {{KEYMAP[KEY::PLAYLIST],title},
                              {KEYMAP[KEY::ADD_DATE],QDate::currentDate()}};

        if(insert(TABLEMAP[TABLE::PLAYLISTS],playlist))
            return true;
    }

    return false;
}

bool CollectionDB::trackPlaylist(const QString &url, const QString &playlist)
{
    QVariantMap map {{KEYMAP[KEY::PLAYLIST],playlist},
                     {KEYMAP[KEY::URL],url},
                     {KEYMAP[KEY::ADD_DATE],QDate::currentDate()}};

    if(insert(TABLEMAP[TABLE::TRACKS_PLAYLISTS],map))
        return true;

    return false;
}


DB_LIST CollectionDB::getDBData(const QStringList &urls)
{
    DB_LIST mapList;

    for(auto url : urls)
    {
        auto queryTxt = QString("SELECT * FROM %1 WHERE %2 = \"%3\"").arg(TABLEMAP[TABLE::TRACKS],
                KEYMAP[KEY::URL],url);

        mapList << this->getDBData(queryTxt);
    }

    return mapList;
}

DB_LIST CollectionDB::getDBData(const QString &queryTxt)
{
    DB_LIST mapList;

    auto query = this->getQuery(queryTxt);

    if(query.exec())
    {
        while(query.next())
        {
            DB data;
            for(auto key : KEYMAP.keys())
                if(query.record().indexOf(KEYMAP[key])>-1)
                    data.insert(key, query.value(KEYMAP[key]).toString());

            mapList<< data;
        }

    }else qDebug()<< query.lastError()<< query.lastQuery();

    return mapList;
}


DB_LIST CollectionDB::getAlbumTracks(const QString &album, const QString &artist, const KEY &orderBy, const BAE::W &order)
{
    auto queryTxt = QString("SELECT * FROM %1 WHERE %2 = \"%3\" AND %4 = \"%5\" ORDER by %6 %7").arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::ARTIST],artist,KEYMAP[KEY::ALBUM],album,KEYMAP[orderBy], SLANG[order]);

    return this->getDBData(queryTxt);
}

DB_LIST CollectionDB::getArtistTracks(const QString &artist, const KEY &orderBy, const BAE::W &order)
{
    auto queryTxt = QString("SELECT * FROM %1 WHERE %2 = \"%3\" ORDER by %4 %5, %6 %5").arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::ARTIST],artist,KEYMAP[orderBy],SLANG[order],KEYMAP[KEY::TRACK]);

    return this->getDBData(queryTxt);
}

QStringList CollectionDB::getArtistAlbums(const QString &artist)
{
    QStringList albums;

    auto queryTxt = QString("SELECT %4 FROM %1 WHERE %2 = \"%3\" ORDER BY %4 ASC").arg(TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::ARTIST],artist,KEYMAP[KEY::ALBUM]);
    auto query = this->getDBData(queryTxt);

    for(auto track : query)
        albums << track[KEY::ALBUM];

    return albums;
}

DB_LIST CollectionDB::getBabedTracks(const KEY &orderBy, const BAE::W &order)
{
    auto queryTxt = QString("SELECT * FROM %1 WHERE %2 = 1 ORDER by %3 %4").arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::BABE],
            KEYMAP[orderBy], SLANG[order]);
    return this->getDBData(queryTxt);
}

DB_LIST CollectionDB::getSearchedTracks(const KEY &where, const QString &search)
{
    QString queryTxt;

    if(where == KEY::PLAYED || where == KEY::STARS || where == KEY::BABE)
        queryTxt = QString("SELECT * FROM %1 WHERE %2 = \"%3\"").arg(TABLEMAP[TABLE::TRACKS],
                KEYMAP[where],
                search);
    else if(where == KEY::WIKI)

        queryTxt = QString("SELECT DISTINCT t.* FROM %1 t INNER JOIN %2 al ON t.%3 = al.%3 INNER JOIN %4 ar ON t.%5 = ar.%5 WHERE al.%6 LIKE \"%%7%\" OR ar.%6 LIKE \"%%7%\" COLLATE NOCASE").arg(TABLEMAP[TABLE::TRACKS],
                TABLEMAP[TABLE::ALBUMS],KEYMAP[KEY::ALBUM],
                TABLEMAP[TABLE::ARTISTS],KEYMAP[KEY::ARTIST],
                KEYMAP[where],
                search);


    else if(where == KEY::PLAYLIST)

        queryTxt = QString("SELECT t.* FROM %1 t INNER JOIN %2 tp ON t.%3 = tp.%3 WHERE tp.%4 = \"%5\"").arg(TABLEMAP[TABLE::TRACKS],
                TABLEMAP[TABLE::TRACKS_PLAYLISTS],KEYMAP[KEY::URL],
                KEYMAP[where],
                search);


    else if(where == KEY::TAG)

        queryTxt = QString("SELECT t.* FROM %1 t INNER JOIN %2 tt ON t.%3 = tt.%3 WHERE tt.%4 = \"%5\" COLLATE NOCASE "
                           "UNION "
                           "SELECT t.* FROM %1 t INNER JOIN %6 at ON t.%7 = at.%7 AND t.%8 = at.%8 WHERE at.%4 = \"%5\" COLLATE NOCASE "
                           "UNION "
                           "SELECT t.* FROM %1 t INNER JOIN %9 art ON t.%8 = art.%8 WHERE art.%4 LIKE \"%%5%\" COLLATE NOCASE "
                           "UNION "
                           "SELECT DISTINCT t.* FROM %1 t INNER JOIN %9 at ON t.%8 = at.%4 WHERE at.%8 LIKE \"%%5%\" COLLATE NOCASE").arg(
                    TABLEMAP[TABLE::TRACKS],
                TABLEMAP[TABLE::TRACKS_TAGS],
                KEYMAP[KEY::URL],
                KEYMAP[where],
                search,
                TABLEMAP[TABLE::ALBUMS_TAGS],
                KEYMAP[KEY::ALBUM],
                KEYMAP[KEY::ARTIST],
                TABLEMAP[TABLE::ARTISTS_TAGS]);

    else
        queryTxt = QString("SELECT * FROM %1 WHERE %2 LIKE \"%%3%\"").arg(TABLEMAP[TABLE::TRACKS],
                KEYMAP[where],
                search);

    qDebug()<<"SEARCH QUERY:"<<queryTxt;

    return this->getDBData(queryTxt);

}

DB_LIST CollectionDB::getPlaylistTracks(const QString &playlist, const KEY &orderBy, const BAE::W &order)
{
    auto queryTxt= QString("SELECT t.* FROM %1 t INNER JOIN %2 tp ON t.%3 = tp.%3 WHERE tp.%4 = '%5' ORDER BY tp.%6 %7").arg(TABLEMAP[TABLE::TRACKS],
            TABLEMAP[TABLE::TRACKS_PLAYLISTS], KEYMAP[KEY::URL],
            KEYMAP[KEY::PLAYLIST],
            playlist, KEYMAP[orderBy], SLANG[order]);

    return this->getDBData(queryTxt);
}

DB_LIST CollectionDB::getFavTracks(const int &stars, const int &limit, const KEY &orderBy, const BAE::W &order)
{
    auto queryTxt= QString("SELECT * FROM %1 WHERE %2 >= %3 ORDER BY %4 %5 LIMIT %6" ).arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::STARS], QString::number(stars),
            KEYMAP[orderBy],SLANG[order],QString::number(limit));

    return this->getDBData(queryTxt);
}

DB_LIST CollectionDB::getRecentTracks(const int &limit, const KEY &orderBy, const BAE::W &order)
{
    auto queryTxt= QString("SELECT * FROM %1 ORDER BY strftime(\"%s\",%2) %3 LIMIT %4" ).arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[orderBy],SLANG[order],QString::number(limit));

    return this->getDBData(queryTxt);
}

DB_LIST CollectionDB::getOnlineTracks(const KEY &orderBy, const BAE::W &order)
{
    auto queryTxt= QString("SELECT * FROM %1 WHERE %2 LIKE \"%3%\" ORDER BY %4 %5" ).arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::URL],YoutubeCachePath,KEYMAP[orderBy],SLANG[order]);

    return this->getDBData(queryTxt);
}

DB_LIST CollectionDB::getMostPlayedTracks(const int &greaterThan, const int &limit, const KEY &orderBy, const BAE::W &order)
{
    auto queryTxt = QString("SELECT * FROM %1 WHERE %2 > %3 ORDER BY %4 %5 LIMIT %6" ).arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::PLAYED], QString::number(greaterThan),
            KEYMAP[orderBy],SLANG[order],QString::number(limit));

    return this->getDBData(queryTxt);
}

QString CollectionDB::getTrackLyrics(const QString &url)
{
    QString lyrics;
    auto query = this->getDBData(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(KEYMAP[KEY::LYRICS],
                                 TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::URL],url));

    for(auto track : query)
        lyrics = track[KEY::LYRICS];

    return lyrics;
}


QString CollectionDB::getTrackArt(const QString &path)
{
    QString color;
    auto query = this->getDBData(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(KEYMAP[KEY::ART],
                                 TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::URL],path));

    for(auto track : query)
        color = track[KEY::ART];

    return color;
}

QStringList CollectionDB::getTrackTags(const QString &path)
{
    Q_UNUSED(path);
    return {};
}

int CollectionDB::getTrackStars(const QString &path)
{
    int stars = 0;
    auto query = this->getDBData(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(KEYMAP[KEY::STARS],
                                 TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::URL],path));

    for(auto track : query)
        stars = track[KEY::STARS].toInt();

    return stars;
}

int CollectionDB::getTrackBabe(const QString &path)
{
    int babe = 0;
    auto query = this->getDBData(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(KEYMAP[KEY::BABE],
                                 TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::URL],path));

    for(auto track : query)
        babe = track[KEY::BABE].toInt();

    return babe;
}

QString CollectionDB::getArtistArt(const QString &artist)
{
    QString artistHead;

    auto query = this->getDBData(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(KEYMAP[KEY::ARTWORK],
                                 TABLEMAP[TABLE::ARTISTS],
            KEYMAP[KEY::ARTIST],artist));

    for(auto track : query)
        if(!track[KEY::ARTWORK].isEmpty() && track[KEY::ARTWORK] != SLANG[W::NONE])
            artistHead = track[KEY::ARTWORK];

    return artistHead;
}

QString CollectionDB::getArtistWiki(const QString &artist)
{
    QString wiki;
    auto query = this->getDBData(QString("SELECT %1 FROM %2 WHERE %3 = \"%4\"").arg(KEYMAP[KEY::WIKI],
                                 TABLEMAP[TABLE::ARTISTS],
            KEYMAP[KEY::ARTIST],artist));

    for(auto track : query)
        wiki = track[KEY::WIKI];

    return wiki;
}

QStringList CollectionDB::getArtistTags(const QString &artist)
{
    QStringList tags;

    auto queryStr = QString("SELECT at.%1 FROM %2 at "
                            "INNER JOIN %3 ta ON ta.%1 = at.%1 "
                            "WHERE ta.%4 = '%5' "
                            "AND at.%6 = \"%7\"").arg(KEYMAP[KEY::TAG],
            TABLEMAP[TABLE::ARTISTS_TAGS],
            TABLEMAP[TABLE::TAGS],
            KEYMAP[KEY::CONTEXT],
            PULPO::CONTEXT_MAP[PULPO::CONTEXT::ARTIST_SIMILAR],
            KEYMAP[KEY::ARTIST],artist);
    auto query = this->getDBData(queryStr);
    for(auto track : query)
        tags << track[KEY::TAG];

    return tags;
}

QString CollectionDB::getAlbumArt(const QString &album, const QString &artist)
{
    QString albumCover;
    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\" AND %5 = \"%6\"").arg(KEYMAP[KEY::ARTWORK],
            TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::ALBUM],album,
            KEYMAP[KEY::ARTIST],artist);
    auto query = this->getDBData(queryStr);

    for(auto track : query)
        if(!track[KEY::ARTWORK].isEmpty() && track[KEY::ARTWORK] != SLANG[W::NONE])
            albumCover = track[KEY::ARTWORK];

    return albumCover;
}

QString CollectionDB::getAlbumWiki(const QString &album, const QString &artist)
{
    QString wiki;
    auto queryStr = QString("SELECT %1 FROM %2 WHERE %3 = \"%4\" AND %5 = \"%6\"").arg(KEYMAP[KEY::WIKI],
            TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::ALBUM],album,
            KEYMAP[KEY::ARTIST],artist);
    auto query = this->getDBData(queryStr);

    for(auto track : query)
        wiki = track[KEY::WIKI];

    return wiki;
}

QStringList CollectionDB::getAlbumTags(const QString &album, const QString &artist)
{
    QStringList tags;

    auto queryStr = QString("SELECT at.%1 FROM %2 at "
                            "INNER JOIN %3 ta ON ta.%1 = at.%1 "
                            "WHERE ta.%4 = '%5' AND at.%6 = \"%7\" AND at.%8 = \"%9\"").arg(KEYMAP[KEY::TAG],
            TABLEMAP[TABLE::ALBUMS_TAGS],
            TABLEMAP[TABLE::TAGS],
            KEYMAP[KEY::CONTEXT],
            PULPO::CONTEXT_MAP[PULPO::CONTEXT::TAG],
            KEYMAP[KEY::ALBUM],album,
            KEYMAP[KEY::ARTIST],artist);

    auto query = this->getDBData(queryStr);

    for(auto track : query)
        tags << track[KEY::TAG];

    return tags;
}

QStringList CollectionDB::getPlaylists()
{
    QStringList playlists;
    auto queryTxt = QString("SELECT %1, %2 FROM %3 ORDER BY %2 DESC").arg(KEYMAP[KEY::PLAYLIST], KEYMAP[KEY::ADD_DATE],TABLEMAP[TABLE::PLAYLISTS]);

    for(auto data : this->getDBData(queryTxt))
        playlists << data[KEY::PLAYLIST];

    return playlists;
}

bool CollectionDB::removeTrack(const QString &path)
{
    auto queryTxt = QString("DELETE FROM %1 WHERE %2 =  \"%3\"").arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::URL],path);
    auto query = this->getQuery(queryTxt);
    if(query.exec())
    {
        if(cleanAlbums()) cleanArtists();
        return true;
    }
    return false;
}

QSqlQuery CollectionDB::getQuery(const QString &queryTxt)
{
    QSqlQuery query(queryTxt, this->m_db);
    return query;
}

bool CollectionDB::removeSource(const QString &path)
{
    auto queryTxt = QString("DELETE FROM %1 WHERE %2 LIKE \"%3%\"").arg(TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::SOURCES_URL],path);
    qDebug() << queryTxt;
    auto query = this->getQuery(queryTxt);
    if(query.exec())
    {
        queryTxt = QString("DELETE FROM %1 WHERE %2 LIKE \"%3%\"").arg(TABLEMAP[TABLE::SOURCES],
                KEYMAP[KEY::URL],path);
        query.prepare(queryTxt);
        if(query.exec())
        {
            if(cleanAlbums()) cleanArtists();
            return true;
        }
    }
    return false;
}

sourceTypes CollectionDB::sourceType(const QString &url)
{
    /*for now*/
    Q_UNUSED(url);
    return sourceTypes::LOCAL;
}


/*******************OLD STUFF********************/

void CollectionDB::insertArtwork(const DB &track)
{
    auto artist = track[KEY::ARTIST];
    auto album =track[KEY::ALBUM];
    auto path = track[KEY::ARTWORK];

    switch(albumType(track))
    {
        case TABLE::ALBUMS :
        {
            auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\" AND %6 = \"%7\"").arg(TABLEMAP[TABLE::ALBUMS],
                    KEYMAP[KEY::ARTWORK],
                    path.isEmpty() ? SLANG[W::NONE] : path,
                                                      KEYMAP[KEY::ALBUM],
                                                      album,
                                                      KEYMAP[KEY::ARTIST],
                                                      artist);

            auto query = this->getQuery(queryStr);
            if(!query.exec())qDebug()<<"COULDNT Artwork[cover] inerted into DB"<<artist<<album;
            break;

        }
        case TABLE::ARTISTS:
        {
            auto queryStr = QString("UPDATE %1 SET %2 = \"%3\" WHERE %4 = \"%5\"").arg(TABLEMAP[TABLE::ARTISTS],
                    KEYMAP[KEY::ARTWORK],
                    path.isEmpty() ? SLANG[W::NONE] : path,
                                                      KEYMAP[KEY::ARTIST],
                                                      artist);
            auto query = this->getQuery(queryStr);
            if(!query.exec())qDebug()<<"COULDNT Artwork[head] inerted into DB"<<artist;

            break;

        }
        default: return;
    }

    emit artworkInserted(track);
}


bool CollectionDB::removePlaylistTrack(const QString &url, const QString &playlist)
{
    auto queryTxt = QString("DELETE FROM %1 WHERE %2 = \"%3\" AND %4 = \"%5\"").arg(TABLEMAP[TABLE::TRACKS_PLAYLISTS],
            KEYMAP[KEY::PLAYLIST],playlist,KEYMAP[KEY::URL],url);
    auto query = this->getQuery(queryTxt);
    return query.exec();
}

bool CollectionDB::removePlaylist(const QString &playlist)
{
    QString queryTxt;
    queryTxt = QString("DELETE FROM %1 WHERE %2 = \"%3\"").arg(TABLEMAP[TABLE::TRACKS_PLAYLISTS],
            KEYMAP[KEY::PLAYLIST],playlist);

    auto query = this->getQuery(queryTxt);
    if(!query.exec()) return false;

    queryTxt = QString("DELETE FROM %1 WHERE %2 = \"%3\"").arg(TABLEMAP[TABLE::PLAYLISTS],
            KEYMAP[KEY::PLAYLIST],playlist);

    query.prepare(queryTxt);
    return query.exec();
}

bool CollectionDB::removeArtist(const QString &artist)
{
    auto queryTxt = QString("DELETE FROM %1 WHERE %2 = \"%3\" ").arg(TABLEMAP[TABLE::ARTISTS],
            KEYMAP[KEY::ARTIST],artist);
    auto query = this->getQuery(queryTxt);

    return query.exec();
}

bool CollectionDB::cleanArtists()
{
    //    delete from artists where artist in (select artist from artists except select distinct artist from tracks);
    auto queryTxt=QString("DELETE FROM %1 WHERE %2 IN (SELECT %2 FROM %1 EXCEPT SELECT DISTINCT %2 FROM %3)").arg(
                TABLEMAP[TABLE::ARTISTS],
            KEYMAP[KEY::ARTIST],
            TABLEMAP[TABLE::TRACKS]
            );
    qDebug()<<queryTxt;

    auto query = this->getQuery(queryTxt);
    emit this->artistsCleaned(query.numRowsAffected());
    return query.exec();
}

bool CollectionDB::removeAlbum(const QString &album, const QString &artist)
{
    auto queryTxt = QString("DELETE FROM %1 WHERE %2 = \"%3\" AND %4 = \"%5\"").arg(TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::ALBUM],album,KEYMAP[KEY::ARTIST],artist);
    auto query = this->getQuery(queryTxt);
    return query.exec();
}

bool CollectionDB::cleanAlbums()
{
    //    delete from albums where (album, artist) in (select a.album, a.artist from albums a except select distinct album, artist from tracks);
    auto queryTxt=QString("DELETE FROM %1 WHERE (%2, %3) IN (SELECT %2, %3 FROM %1 EXCEPT SELECT DISTINCT %2, %3  FROM %4)").arg(
                TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::ALBUM],
            KEYMAP[KEY::ARTIST],
            TABLEMAP[TABLE::TRACKS]
            );
    qDebug()<<queryTxt;
    auto query = this->getQuery(queryTxt);
    emit albumsCleaned(query.numRowsAffected());
    return query.exec();
}


