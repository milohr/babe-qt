#ifndef COLLECTIONDB_H
#define COLLECTIONDB_H

#include <QList>
#include <QString>
#include <QStringList>
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
    QSqlQuery getQuery(const QString &queryTxt);
    bool checkQuery(QString queryTxt);
    bool updateTrack(const QString &column, const QString &location, const QVariant &value);
    void prepareCollectionDB();
    bool execQuery(QString queryTxt);
    bool hasTrack(const QString &searchId, const QString &searchTerm);

    QList<QMap<int, QString>> getTrackData(const QString &url);
    QList<QMap<int, QString>> getTrackData(const QVariantMap &filter, const QString &orderBy = "", const QString &whereOperator = "&", const QString &whereComparator = QStringLiteral("="));

    QString getArtistArt(QString artist);
    QString getAlbumArt(QString album, QString artist);
    QStringList getPlaylists(int limit = -1, int offset = 0, const QString &orderBy = "title");
    QStringList getPlaylistsMoods();
    QStringList albums;
    QStringList artists;

    enum colums
    {
        TRACK, TITLE, ARTIST, ALBUM, GENRE, LOCATION, STARS, BABE, ART, PLAYED, PLAYLIST
    };

public slots:
    void debugDatabaseMessage(const QString &message);
    bool addTrack(QStringList paths, int babe = 0);
    void insertPlaylist(const QString &name = "", const QString &color = "");
    void removePath(QString path);
    void setCollectionLists();
    void refreshArtistsTable();
    void cleanCollectionLists();
    void insertCoverArt(const QString &art, const QStringList &info);
    void insertHeadArt(const QString &art, const QStringList &info);

signals:
    void progress(int);
    void DBactionFinished(bool state);

private:
    Database *m_database;
};

#endif // COLLECTION_H
