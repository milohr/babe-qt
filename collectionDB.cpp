#include "collectionDB.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <track.h>
#include <typeinfo>

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



void CollectionDB::prepareCollectionDB(QString path)
{


    QSqlQuery query;
    query.exec("CREATE TABLE tracks(title text, artist text, album text, location text, stars integer, babe integer);");


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
    {
       if (query.next())
       {
          qDebug()<< "found the query";
          return true;
       }else
       {
           qDebug()<<"the query failed!";
           return false;
       }



    }else
    {
    qDebug()<<"the query failed!";

        return false;
    }


}

void CollectionDB::addTrack()
{
    //bool success = false;


        QSqlQuery query;

        query.exec("PRAGMA synchronous=OFF");

     qDebug()<<"started wrrting to database...";
      for(int i = 0; i < trackList.size(); i++)
      {

         // you should check if args are ok first...

         query.prepare("INSERT INTO tracks (title, artist, album, location, stars, babe)" "VALUES (:title, :artist, :album, :location, :stars, :babe ) ");
         query.bindValue(":title", QString::fromStdString(trackList[i].getTitle()));
         query.bindValue(":artist", QString::fromStdString(trackList[i].getArtist()));
         query.bindValue(":album", QString::fromStdString(trackList[i].getAlbum()));
         query.bindValue(":location", QString::fromStdString(trackList[i].getLocation()));
         query.bindValue(":stars", 0);
         query.bindValue(":babe", 0);

         if(query.exec())
         {
             //success = true;
             qDebug()<< "writting to db: "<<QString::fromStdString(trackList[i].getTitle());
emit progress(i+1);
         }
         else
         {
              qDebug() << "addPerson error:  "
                       << query.lastError();
         }


      }

     qDebug()<<"finished wrrting to database";

emit DBactionFinished(true);
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


