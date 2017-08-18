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


    enum sourceTypes
    {
        LOCAL, ONLINE, DEVICE
    };


    explicit CollectionDB();
    //CollectionDB(bool connect);
    ~CollectionDB(){}
    void openCollection(const QString &path);
    QSqlQuery getQuery(QString queryTxt);
    bool checkQuery(QString queryTxt);
    bool insertInto(const QString &tableName, const QString &column, const QString &location, const QVariant &value);
    void setTrackList(QList <Track>);
    void prepareCollectionDB();
    bool removeQuery(QString queryTxt);
    bool execQuery(QString queryTxt);
    bool check_existance(QString tableName, QString searchId, QString search);

    QList<QMap<int, QString>> getTrackData(const QStringList &urls);
    QList<QMap<int, QString>> getTrackData(const QString &queryText);
    QString getArtistArt(QString artist);
    QString getAlbumArt(QString album, QString artist);
    QStringList getPlaylists();
    QStringList getPlaylistsMoods();
    QStringList albums;
    QStringList artists;


    /* usefull actions */
    void addTrack(const QStringList &paths, const int &babe=0);

    /*useful tools*/
    sourceTypes sourceType(const QString &url);


private:

    QSqlDatabase m_db;
    QSqlQuery sqlQuery;
    QList <Track> trackList;

    /*basic actions*/
    bool insert(const QString &tableName, const QVariantMap &insertData);
    bool update();
    bool remove();
    QSqlQuery select();



public slots:
    void closeConnection();
    void insertPlaylist(const QString &name);
    bool removePath(const QString &path);
    void setCollectionLists();
    void refreshArtistsTable();
    void cleanCollectionLists();

    /*useful*/
    void insertCoverArt(QString path, QStringList info);
    void insertHeadArt(QString path, QStringList info);

signals:
    void progress(int);
    void DBactionFinished();

};


#endif // COLLECTION_H
