#ifndef COLLECTIONDB_H
#define COLLECTIONDB_H

#include <QDir>
#include <QList>
#include <QDebug>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <typeinfo>

#include "track.h"
#include "taginfo.h"
#include "database.h"

class CollectionDB : public QObject
{
    Q_OBJECT
public:
    explicit CollectionDB(QObject *parent = 0);
    ~CollectionDB(){}
    void openCollection(QString path);
    QSqlQuery getQuery(QString queryTxt);
    bool checkQuery(QString queryTxt);
    bool insertInto(QString tableName, QString column, QString location, int value);
    bool insertInto(QString tableName, QString column, QString location, QString value);
    void setTrackList(QList <Track>);
    void prepareCollectionDB();
    bool execQuery(QString queryTxt);
    bool check_existance(QString tableName, QString searchId, QString search);

    QList<QMap<int, QString>> getTrackData(QStringList urls);
    QList<QMap<int, QString>> getTrackData(const QVariantMap &filter, const QString &orderBy = "", const QString &whereOperator = "&");
    QString getArtistArt(QString artist);
    QString getAlbumArt(QString album, QString artist);
    QStringList getPlaylists(int limit = -1, int offset = 0, const QString &orderBy = "title");
    QStringList getPlaylistsMoods();
    QStringList albums;
    QStringList artists;

    enum colums
    {
        ID, TRACK, TITLE, ARTIST, ALBUM, GENRE, LOCATION, STARS, BABE, ART, PLAYED, PLAYLIST
    };

public slots:
    bool addTrack(QStringList paths, int babe = 0);
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

private:
    QSqlDatabase m_db;
    QList<Track> trackList;
    Database *m_database;
};


#endif // COLLECTION_H
