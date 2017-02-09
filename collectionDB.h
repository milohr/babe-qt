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
#include <QSqlDriver>
#include <QFileInfo>

class CollectionDB : public QObject
{
     Q_OBJECT
public:

    CollectionDB();
    //CollectionDB(bool connect);
    virtual ~CollectionDB(){}
    void openCollection(QString path);
    QSqlQuery getQuery(QString queryTxt);
    bool checkQuery(QString queryTxt);
    bool insertInto(QString tableName, QString column, QString location, int value);
        bool insertInto(QString tableName, QString column, QString location, QString value);
    void setTrackList(QList <Track>);
    void prepareCollectionDB();
    bool removeQuery(QString queryTxt);
    bool check_existance(QString tableName, QString searchId, QString search);
    void addSong(QStringList paths, int babe);
    void createTable(QString tableName);
    QStringList getPlaylists();
    QStringList getPlaylistsMoods();
    QStringList albums;
    QStringList artists;
    enum colums
    {
        TRACK,TITLE,ARTIST,ALBUM,GENRE,LOCATION,STARS,BABE,ART,PLAYED,PLAYLIST
    };

private:

    QSqlDatabase m_db;
    QList <Track> trackList;


public slots:
    void addTrack(QStringList paths, int babe=0);
    void closeConnection();
    void insertPlaylist(QString name, QString color);

    void removePath(QString path);
    void setCollectionLists();
    void insertCoverArt(QString path, QStringList info);
    void insertHeadArt(QString path, QStringList info);


signals:
    void progress(int);
    void DBactionFinished(bool state);

};


#endif // COLLECTION_H
