#include "collectionDB.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <track.h>


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

CollectionDB::CollectionDB()
{

}

void CollectionDB::prepareCollectionDB(QString path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    m_db.open();
    QSqlQuery query;
    query.exec("CREATE TABLE tracks(id integer primary key, title text, album text, artist text, location text, stars integer, babe integer);");


}

bool CollectionDB::addTrack( QList<Track> tracks)
{
  bool success = false;
    for(int i = 0; i < tracks.size(); i++)
    {

   // you should check if args are ok first...
   QSqlQuery query;
   query.prepare("INSERT INTO people (name) VALUES (:name)");
   query.bindValue(":name", QString::fromStdString(tracks[1].getTitle()));
   if(query.exec())
   {
       success = true;
   }
   else
   {
        qDebug() << "addPerson error:  "
                 << query.lastError();
   }
    }
   return success;
}
