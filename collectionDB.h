#ifndef COLLECTIONDB_H
#define COLLECTIONDB_H
#include <QString>
#include <QStringList>
#include <QList>
#include <QSqlDatabase>
#include <QWidget>
#include <typeinfo>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QFileInfo>
#include <QDir>
#include "track.h"
#include "taginfo.h"

class CollectionDB : public QObject
{
    Q_OBJECT
public:

    explicit CollectionDB();
    //CollectionDB(bool connect);
    ~CollectionDB(){}
    void openCollection(QString path);
    QSqlQuery getQuery(QString queryTxt);
    bool checkQuery(QString queryTxt);
    bool insertInto(QString tableName, QString column, QString location, int value);
    bool insertInto(QString tableName, QString column, QString location, QString value);
    void setTrackList(QList <Track>);
    void prepareCollectionDB();
    bool removeQuery(QString queryTxt);
    bool execQuery(QString queryTxt);
    bool check_existance(QString tableName, QString searchId, QString search);
    void createTable(QString tableName);

    QList<QMap<int, QString>> getTrackData(QStringList urls);
    QList<QMap<int, QString>> getTrackData(QString queryText);
    QString getArtistArt(QString artist);
    QString getAlbumArt(QString album, QString artist);
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
    bool addTrack(QStringList paths, int babe=0);
    void closeConnection();
    void insertPlaylist(QString name, QString color);

    void removePath(QString path);
    void setCollectionLists();
    void refreshArtistsTable();
    void cleanCollectionLists();
    void insertCoverArt(QString path, QStringList info);
    void insertHeadArt(QString path, QStringList info);


signals:
    void progress(int);
    void DBactionFinished(bool state);

};


#endif // COLLECTION_H
