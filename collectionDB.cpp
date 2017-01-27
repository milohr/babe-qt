#include "collectionDB.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <track.h>
#include <QSqlDriver>
#include <taginfo.h>


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
    QSqlQuery query;
    qDebug()<<"trying to delete all from :"<< path;
       query.prepare("DELETE FROM tracks WHERE location LIKE \"%"+path+"%\"");

       bool success = query.exec();

       if(!success)
       {
           qDebug() << "removePerson error: "
                    << query.lastError();

       }else
       {

       }

}

void CollectionDB::prepareCollectionDB()
{


    QSqlQuery query;
    query.exec("CREATE TABLE tracks(track integer, title text, artist text, album text, genre text, location text unique, stars integer, babe integer, art text, played integer, playlist text);");

    query.exec("CREATE TABLE playlists(title text);");
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
           qDebug()<<"didn't ind the query!";
           return false;
       }



    }else
    {
    qDebug()<<"the query failed!";

        return false;
    }


}

void CollectionDB::addTrack(QStringList paths)
{
    //bool success = false;


        QSqlQuery query;

        query.exec("PRAGMA synchronous=OFF");
int i=0;
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
         query.bindValue(":babe", 0);
         query.bindValue(":art", "");//here need to fecth the artwork
         query.bindValue(":played", 0);


         if(query.exec())
         {
             //success = true;
             qDebug()<< "writting to db: "<<info.getTitle();
             emit progress((i++)+1);
         }
         else
         {
              qDebug() << "addPerson error:  "
                       << query.lastError()
                        <<info.getTitle();
         }


      }

     qDebug()<<"finished wrrting to database";

emit DBactionFinished(true);

}


void CollectionDB::addSong(QStringList paths, int babe=0)
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
    query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
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



   // qDebug()<< QString::fromStdString(typeid(query).name());
   // return true;

}


void CollectionDB::createTable(QString tableName)
{

    QSqlQuery query;
    query.exec("CREATE TABLE "+tableName+"(track integer, title text, artist text, album text, genre text, location text unique, stars integer, babe integer, art text, played integer, playlist text);");

}

void CollectionDB::insertPlaylist(QString name)
{



   QSqlQuery query;
   query.prepare("INSERT INTO playlists (title)" "VALUES (:title) ");

   query.bindValue(":title", name);

   if(query.exec())
   {
       //qDebug()<<"insertInto<<"<<"UPDATE playlists SET title = "+ name ;
      // createTable(name);


   }else
   {

   }

}

QStringList CollectionDB::getPlaylists()
{



   QSqlQuery query;
   QStringList files;
   query.prepare("SELECT * FROM playlists");

   if (query.exec())   while (query.next()) files << query.value(0).toString();

   return files;


}
