#include "collectionDB.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <track.h>


CollectionDB::CollectionDB()
{

}

void CollectionDB::setCollectionDB(QString path)
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
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    m_db.open();
    QSqlQuery query;
    query.exec("CREATE TABLE tracks(title text, album text, artist text, location text, stars integer, babe integer);");


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
      m_db.close();

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


