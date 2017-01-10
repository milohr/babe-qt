#ifndef COLLECTIONDB_H
#define COLLECTIONDB_H
#include <QString>
#include <QStringList>
#include <QList>
#include <QSqlDatabase>
#include <track.h>
#include <QThread>
#include <QWidget>
#include <typeinfo>
#include<QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

class CollectionDB : public QObject
{
     Q_OBJECT
public:

    CollectionDB();
    //CollectionDB(bool connect);
    virtual ~CollectionDB(){}
    void openCollection(QString path);

QSqlQuery getQuery(QString queryTxt);
    bool insertInto(QString tableName, QString column, QString location, int value);




    void setTrackList(QList <Track>);
    void prepareCollectionDB(QString path);
    bool check_existance(QString tableName, QString searchId, QString search);
private:
QSqlDatabase m_db;
    QList <Track> trackList;

public slots:
    void addTrack();
 void closeConnection();

signals:
    void progress(int);
    void DBactionFinished(bool);

};


#endif // COLLECTION_H
