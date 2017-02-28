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

void CollectionDB::openCollection(QString path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

void CollectionDB::removePath(QString path)
{

    qDebug()<<"trying to delete all from :"<< path;
    QSqlQuery queryTracks;
    queryTracks.prepare("DELETE FROM tracks  WHERE location LIKE \"%"+path+"%\"");
    bool success = queryTracks.exec();



    emit DBactionFinished(false);

    if(!success)
    {
        qDebug() << "removePerson error: ";

    }

}


QList<QStringList> CollectionDB::getTrackData(QStringList urls)
{
    QList<QStringList> list;
    for(auto url:urls)
    {
          QSqlQuery query("SELECT * FROM tracks WHERE location =\""+url+"\"");
          if(query.exec())
          {
              while(query.next())
              {
                  QStringList track;
                  track<< query.value(TRACK).toString();
                  track<< query.value(TITLE).toString();
                  track<< query.value(ARTIST).toString();
                  track<< query.value(ALBUM).toString();
                  track<< query.value(GENRE).toString();
                  track<< query.value(LOCATION).toString();
                  track<< query.value(STARS).toString();
                  track<< query.value(BABE).toString();
                  track<< query.value(ART).toString();
                  track<< query.value(PLAYED).toString();
                  track<< query.value(PLAYLIST).toString();
                  list<<track;
              }

          }

    }

    return list;
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
            {
                continue;
            }else
            {
                qDebug()<<"artists list does not longer contains"<<oldArtists;
                QSqlQuery queryArtist_delete;
                queryArtist_delete.prepare("DELETE FROM artists  WHERE title = \""+oldArtists+"\"");
                if(queryArtist_delete.exec()) qDebug()<<"deleted gone album";

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
            {
                continue;
            }else
            {
                qDebug()<<"albums list does not longer contains"<<oldAlbum;
                QSqlQuery queryAlbum_delete;
                queryAlbum_delete.prepare("DELETE FROM albums  WHERE title = \""+queryAlbums.value(0).toString()+"\"");
                if(queryAlbum_delete.exec()) qDebug()<<"deleted gone album";
            }
        }
    }


}

void CollectionDB::prepareCollectionDB()
{
    QSqlQuery query;
    query.exec("CREATE TABLE tracks(track integer, title text, artist text, album text, genre text, location text unique, stars integer, babe integer, art text, played integer, playlist text);");
    query.exec("CREATE TABLE albums(title text, artist text, art text, location text);");
    query.exec("CREATE TABLE playlists(title text, art text unique);");
    query.exec("CREATE TABLE artists(title text, art text, location text);");

    //query.exec("CREATE TABLE tracks(title text, album text, artist text, location text, stars integer, babe integer);");
}

QSqlQuery CollectionDB::getQuery(QString queryTxt)
{
    QSqlQuery query(queryTxt);
    return query;
}


bool CollectionDB::removeQuery(QString queryTxt)
{
    QSqlQuery query;
    query.prepare(queryTxt);

    bool success = query.exec();

    if(!success)
    {
        qDebug() << "removePerson error: "
                 << query.lastError();
        return false;
    }else return true;
}

bool CollectionDB::checkQuery(QString queryTxt)
{
    QSqlQuery query(queryTxt);

    qDebug()<<"The Query is: "<<queryTxt;

    if (query.exec())
    {
        if (query.next())
        {
            qDebug()<< "found the query";
            return true;
        }else
        {
            qDebug()<<"didn't find the query!";
            return false;
        }
    }else
    {
        qDebug()<<"the query failed!";

        return false;
    }


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

    if(paths.isEmpty()) return false;


    QSqlQuery query;


    if(query.exec("PRAGMA synchronous=OFF"))
    {
        success=true;

        int i=0;
        qDebug()<<"started wrrting to database...";
        for(auto file:paths)
        {
            qDebug()<<file;

            TagInfo info(file);
            int track;
            QString  title, artist, album, genre;
            // you should check if args are ok first...
            track=info.getTrack();
            title=info.getTitle();
            artist= info.getArtist();
            genre=info.getGenre();

            if(info.getAlbum().isEmpty())
            {
                qDebug()<<"the album has not title, so i'm going to try and get it for you";
                info.writeData();
                album=info.getAlbum();
            }else
            {
                qDebug()<<"the album has a title";
                album=info.getAlbum();
            }


            query.prepare("INSERT INTO tracks (track, title, artist, album, genre, location, stars, babe, art, played)" "VALUES (:track, :title, :artist, :album, :genre, :location, :stars, :babe, :art, :played) ");
            query.bindValue(":track", track);
            query.bindValue(":title", title);
            query.bindValue(":artist", artist);
            query.bindValue(":album", album);
            query.bindValue(":genre", genre);
            query.bindValue(":location", file);
            query.bindValue(":stars", 0);
            query.bindValue(":babe", babe);
            query.bindValue(":art", "");//here need to fecth the artwork
            query.bindValue(":played", 0);


            if(query.exec())
            {
                success = true;
                qDebug()<< "writting to db: "<<info.getTitle();
                if(!albums.contains(artist+" "+album))
                {
                    query.prepare("INSERT INTO albums (title, artist, art, location)" "VALUES (:title, :artist, :art, :location)");
                    query.bindValue(":title", album);
                    query.bindValue(":artist", artist);
                    query.bindValue(":art", "");
                    // query.bindValue(":location", QFileInfo(file).dir().path());
                    if(query.exec())
                    {
                        albums<<artist+" "+album;
                        success=true;
                    }else
                    {
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
            }
            else
            {
                qDebug() << "adding track error:  "
                         << query.lastError()
                         <<info.getTitle();

            }
        }

        qDebug()<<"finished wrrting to database";
        emit DBactionFinished(true);
    }
    else
    {
       return false;
    }


    return success;
}

void CollectionDB::insertCoverArt(QString path,QStringList info)
{
    //UPDATE albums SET art = "lalaltest" WHERE title = "Starboy" AND artist = "The Weeknd"

    qDebug()<<"the path:"<<path<<"the list:"<<info.at(0)<<info.at(1);
    if(info.size()==2)
    {

        QSqlQuery query;
        query.prepare("UPDATE albums SET art = (:art) WHERE title = (:title) AND artist = (:artist)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
        query.bindValue(":art",  path.isEmpty()?"NULL": path );
        query.bindValue(":title", info.at(0));
        query.bindValue(":artist", info.at(1));
        if(query.exec())
        {
            qDebug()<<"Artwork[cover] inserted into DB"<<info.at(0)<<info.at(1);
            if(!albums.contains(info.at(0))) albums<<info.at(1)+" "+info.at(0);
            //qDebug()<<"insertInto<<"<<"UPDATE "+tableName+" SET "+column+" = "+ value + " WHERE location = "+location;
        }else
        {
            qDebug()<<"COULDNT Artwork[cover] inerted into DB"<<info.at(0)<<info.at(1);
        }
    }

}

void CollectionDB::insertHeadArt(QString path, QStringList info)
{
    if(info.size()==1)
    {
        QSqlQuery query;
        query.prepare("UPDATE artists SET art = (:art) WHERE title = (:title)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
        query.bindValue(":art", path.isEmpty()?"NULL": path );
        query.bindValue(":title", info.at(0));
        if(query.exec())
        {
            qDebug()<<"Artwork[head] inerted into DB"<<info.at(0);
            if(!artists.contains(info.at(0)))artists<<info.at(0);
            //qDebug()<<"insertInto<<"<<"UPDATE "+tableName+" SET "+column+" = "+ value + " WHERE location = "+location;
        }
    }
}

void CollectionDB::addSong(QStringList paths, int babe=0)//deprecared
{
    //bool success = false;


    QSqlQuery query;


    qDebug()<<"started wrrting to database...";
    for(auto file:paths)
    {
        TagInfo info(file);

        // you should check if args are ok first...

        query.prepare("INSERT INTO tracks (track, title, artist, album, genre, location, stars, babe, art, played)" "VALUES (:track, :title, :artist, :album, :genre, :location, :stars, :babe, :art, :played) ");
        query.bindValue(":track", info.getTrack());
        query.bindValue(":title", info.getTitle());
        query.bindValue(":artist", info.getArtist());
        query.bindValue(":album", info.getAlbum());
        query.bindValue(":genre", info.getGenre());
        query.bindValue(":location", file);
        query.bindValue(":stars", 0);
        query.bindValue(":babe", babe);
        query.bindValue(":art", "");//here need to fecth the artwork
        query.bindValue(":played", 0);

        if(query.exec())
        {
            //success = true;
            qDebug()<< "writting to db: "<<info.getTitle();
        }
        else
        {
            qDebug() << "addPerson error:  "
                     << query.lastError();
        }


    }

    qDebug()<<"single song added to database";

}


void CollectionDB::setTrackList(QList <Track> trackList)
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
            qDebug()<< "it exists";
            return true;
        }else
        {
            qDebug()<<"currnt song doesn't exists in db";
            return false;
        }


    }else
    {


        return false;
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
    }else
    {
        return false;
    }

}

bool CollectionDB::insertInto(QString tableName, QString column, QString location, int value)
{



    QSqlQuery query;
    query.prepare("UPDATE "+tableName+" SET "+column+" = (:value) WHERE location = (:location)" );
    //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");

    query.bindValue(":value", value);
    query.bindValue(":location", location);
    if(query.exec())
    {
        qDebug()<<"insertInto<<"<<"UPDATE "+tableName+" SET "+column+" = "+ value + " WHERE location = "+location;

        return true;
    }else
    {
        return false;
    }

}

bool CollectionDB::insertInto(QString tableName, QString column, QString location, QString value)
{



    QSqlQuery query;
    query.prepare("UPDATE "+tableName+" SET "+column+" = (:value) WHERE location = (:location)" );
    //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");

    query.bindValue(":value", value);
    query.bindValue(":location", location);
    if(query.exec())
    {
        qDebug()<<"insertInto<<"<<"UPDATE "+tableName+" SET "+column+" = "+ value + " WHERE location = "+location;

        return true;
    }else
    {
        return false;
    }

}


void CollectionDB::createTable(QString tableName)
{

    QSqlQuery query;
    query.exec("CREATE TABLE "+tableName+"(track integer, title text, artist text, album text, genre text, location text unique, stars integer, babe integer, art text, played integer, playlist text);");

}

void CollectionDB::insertPlaylist(QString name, QString color)
{


    if(color.isEmpty())
    {
        QSqlQuery query;
        query.prepare("INSERT INTO playlists (title)" "VALUES (:title) ");

        query.bindValue(":title", name);

        if(query.exec())
        {
            //qDebug()<<"insertInto<<"<<"UPDATE playlists SET title = "+ name ;

        }
    }else if(name.isEmpty())
    {  QSqlQuery query;
        query.prepare("INSERT INTO playlists (title, art)" "VALUES (:title, :art) ");

        query.bindValue(":title", "mood");
        query.bindValue(":art", color);

        if(query.exec())
        {
            //qDebug()<<"insertInto<<"<<"UPDATE playlists SET title = "+ name ;

        }

    }else if(!name.isEmpty()&&!color.isEmpty())
    {

        QSqlQuery query;
        query.prepare("INSERT INTO playlists (title, art)" "VALUES (:title, :art) ");

        query.bindValue(":title", "mood");
        query.bindValue(":art", color);
        if(query.exec())
        {
            //qDebug()<<"insertInto<<"<<"UPDATE playlists SET title = "+ name ;

        }
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
