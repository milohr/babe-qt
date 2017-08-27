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
    QSqlQuery getQuery(QString queryTxt);
    bool checkQuery(QString queryTxt);
    bool insertInto(const QString &tableName, const QString &column, const QString &location, const QVariant &value);
    void setTrackList(QList <Track>);
    bool removeQuery(QString queryTxt);
    bool execQuery(QString queryTxt);

    QStringList albums;
    QStringList artists;

    /*basic public actions*/
    void prepareCollectionDB();
    void setUpCollection(const QString &path);
    bool check_existance(const QString &tableName, const QString &searchId, const QString &search);

    /* usefull actions */
    void addTrack(const QStringList &paths, const int &babe=0);
    bool rateTrack(const QString &path, const int &value);
    bool babeTrack(const QString &path, const bool &value);
    bool moodTrack(const QString &path, const QString &value);
    bool artTrack(const QString &path, const QString &value);
    bool lyricsTrack(const QString &path, const QString &value);
    bool wikiArtist(const QString &artist, const QString &value);
    bool wikiAlbum(const QString &album, const QString &artist, QString value);

    bool addPlaylist(const QString &title);
    bool trackPlaylist(const QString &url, const QString &playlist);

    Bae::TRACKMAP_LIST getTrackData(const QStringList &urls);
    Bae::TRACKMAP_LIST getTrackData(const QString &queryText);
    QString getTrackLyrics(const QString &url);
    QString getTrackArt(const QString &path);
    QString getArtistArt(const QString &artist);
    QString getArtistWiki(const QString &artist);
    QString getAlbumArt(const QString &album, const QString &artist);
    QString getAlbumWiki(const QString &album, const QString &artist);

    QStringList getPlaylists();



    QStringList getPlaylistsMoods();
    /*useful tools*/
    sourceTypes sourceType(const QString &url);


private:

    QSqlDatabase m_db;
    QList <Track> trackList;

    /*basic actions*/
    bool insert(const QString &tableName, const QVariantMap &insertData);
    bool update(const QString &table, const QString &column, const QVariant &newValue, const QVariant &op, const QString &id);
    bool remove();
    bool openDB();
    QSqlQuery select();



public slots:
    void closeConnection();
    bool removePath(const QString &path);
    void setCollectionLists();
    void refreshArtistsTable();
    void cleanCollectionLists();

    /*useful*/
    void insertCoverArt(const QString &path, const Bae::TRACKMAP &track);
    void insertHeadArt(const QString &path, const Bae::TRACKMAP &track);

signals:
    void progress(int);
    void DBactionFinished();

};


#endif // COLLECTION_H
