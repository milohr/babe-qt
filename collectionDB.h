#ifndef COLLECTIONDB_H
#define COLLECTIONDB_H

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QWidget>
#include <typeinfo>

#include "track.h"
#include "taginfo.h"

class CollectionDB : public QObject
{
    Q_OBJECT
public:
    explicit CollectionDB(QObject *parent = 0);
    ~CollectionDB(){}
    void openCollection(const QString &path);
    QSqlQuery getQuery(const QString &queryTxt);
    bool checkQuery(const QString &queryTxt);
    bool insertInto(const QString &tableName, const QString &column, const QString &location, int value);
    bool insertInto(const QString &tableName, const QString &column, const QString &location, const QString &value);
    void setTrackList(const QList<Track> &trackList);
    void prepareCollectionDB();
    bool removeQuery(const QString &queryTxt);
    bool execQuery(const QString &queryTxt);
    bool check_existance(const QString &tableName, const QString &searchId, const QString &search);
    void createTable(const QString &tableName);

    QList<QMap<int, QString>> getTrackData(const QStringList &urls);
    QList<QMap<int, QString>> getTrackData(const QString &queryText);
    QString getArtistArt(const QString &artist);
    QString getAlbumArt(const QString &album, const QString &artist);
    QStringList getPlaylists();
    QStringList getPlaylistsMoods();
    QStringList albums;
    QStringList artists;

    enum colums
    {
        TRACK, TITLE, ARTIST, ALBUM, GENRE, LOCATION, STARS, BABE, ART, PLAYED, PLAYLIST
    };

private:
    QSqlDatabase m_db;
    QList <Track> trackList;

public slots:
    bool addTrack(const QStringList &paths, int babe = 0);
    void closeConnection();
    void insertPlaylist(const QString &name);
    void removePath(const QString &path);
    void setCollectionLists();
    void refreshArtistsTable();
    void cleanCollectionLists();
    void insertCoverArt(const QString &path, const QStringList &info);
    void insertHeadArt(const QString &path, const QStringList &info);

signals:
    void progress(int);
    void DBactionFinished(bool state);
};

#endif // COLLECTION_H
