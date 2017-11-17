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
#include <QVariantMap>

#include "../utils/bae.h"

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
    bool execQuery(QSqlQuery &query);
    bool execQuery(const QString &queryTxt);

    /*basic public actions*/
    void prepareCollectionDB();
    void setUpCollection(const QString &path);
    bool check_existance(const QString &tableName, const QString &searchId, const QString &search);

    /* usefull actions */

    void insertArtwork(const Bae::DB &track);

    void addTrack(const Bae::DB &track);

    bool rateTrack(const QString &path, const int &value);
    bool babeTrack(const QString &path, const bool &value);
    bool moodTrack(const QString &path, const QString &value);
    bool artTrack(const QString &path, const QString &value);
    bool lyricsTrack(const Bae::DB &track, const QString &value);
    bool playedTrack(const QString &url, const int &increment=1);

    bool wikiTrack(const Bae::DB &track, const QString &value);
    bool tagsTrack(const Bae::DB &track, const QString &value, const QString &context);
    bool albumTrack(const Bae::DB &track, const QString &value);
    bool trackTrack(const Bae::DB &track, const QString &value);

    bool wikiArtist(const Bae::DB &track, const QString &value);
    bool tagsArtist(const Bae::DB &track, const QString &value, const QString &context = "");

    bool wikiAlbum(const Bae::DB &track, QString value);
    bool tagsAlbum(const Bae::DB &track, const QString &value, const QString &context = "");

    bool addPlaylist(const QString &title);
    bool trackPlaylist(const QString &url, const QString &playlist);

    Bae::DB_LIST getDBData(const QStringList &urls);
    Bae::DB_LIST getDBData(QSqlQuery &query);

    Bae::DB_LIST getAlbumTracks(const QString &album, const QString &artist, const Bae::KEY &orderBy = Bae::KEY::TRACK, const Bae::W &order = Bae::W::ASC);
    Bae::DB_LIST getArtistTracks(const QString &artist, const Bae::KEY &orderBy = Bae::KEY::ALBUM, const Bae::W &order = Bae::W::ASC);
    Bae::DB_LIST getBabedTracks(const Bae::KEY &orderBy = Bae::KEY::PLAYED, const Bae::W &order = Bae::W::DESC);
    Bae::DB_LIST getSearchedTracks(const Bae::KEY &where, const QString &search);
    Bae::DB_LIST getPlaylistTracks(const QString &playlist, const Bae::KEY &orderBy = Bae::KEY::ADD_DATE, const Bae::W &order = Bae::W::DESC);
    Bae::DB_LIST getMostPlayedTracks(const int &greaterThan = 1,const int &limit = 50, const Bae::KEY &orderBy = Bae::KEY::PLAYED, const Bae::W &order = Bae::W::DESC);
    Bae::DB_LIST getFavTracks(const int &stars = 1,const int &limit = 50, const Bae::KEY &orderBy = Bae::KEY::STARS, const Bae::W &order = Bae::W::DESC);
    Bae::DB_LIST getRecentTracks(const int &limit = 50, const Bae::KEY &orderBy = Bae::KEY::ADD_DATE, const Bae::W &order = Bae::W::DESC);
    Bae::DB_LIST getOnlineTracks(const Bae::KEY &orderBy = Bae::KEY::ADD_DATE, const Bae::W &order = Bae::W::DESC);


    QString getTrackLyrics(const QString &url);
    QString getTrackArt(const QString &path);
    QStringList getTrackTags(const QString &path);
    int getTrackStars(const QString &path);
    int getTrackBabe(const QString &path);
    QString getArtistArt(const QString &artist);
    QString getArtistWiki(const QString &artist);
    QStringList getArtistTags(const QString &artist);
    QString getAlbumArt(const QString &album, const QString &artist);
    QString getAlbumWiki(const QString &album, const QString &artist);
    QStringList getAlbumTags(const QString &album, const QString &artist);
    QStringList getArtistAlbums(const QString &artist);

    QStringList getPlaylists();
    QStringList getPlaylistsMoods();


    bool removePlaylistTrack(const QString &url, const QString &playlist);
    bool removePlaylist(const QString &playlist);
    bool removeArtist(const QString &artist);
    bool cleanArtists();
    bool removeAlbum(const QString &album, const QString &artist);
    bool cleanAlbums();
    bool removeSource(const QString &path);
    bool removeTrack(const QString &path);
    /*useful tools*/
    sourceTypes sourceType(const QString &url);


private:

    QSqlDatabase m_db;
    /*basic actions*/
    bool insert(const QString &tableName, const QVariantMap &insertData);
    bool update(const QString &table, const QString &column, const QVariant &newValue, const QVariant &op, const QString &id);
    bool remove();
    bool openDB();

public slots:
    void closeConnection();
    /*useful*/

signals:
    void trackInserted();
    void artworkInserted(const Bae::DB &albumMap);
    void DBactionFinished();

};


#endif // COLLECTION_H
