#ifndef COLLECTIONDB_H
#define COLLECTIONDB_H

#include "track.h"
#include "taginfo.h"

class QDir;
class QFileInfo;
class QWidget;

class AlbumsDB;
class ArtistsDB;
class TracksDB;
class PlaylistsDB;

class CollectionDB : public QObject
{
    Q_OBJECT
public:
    explicit CollectionDB(QObject *parent = 0);
    ~CollectionDB();

    void setTrackList(const QList<Track> &trackList);
    bool trackExists(const QVariantMap &data);
    void saveAll(const QVariantMap &trackData);
    void saveTrack(const QMap<int, QString> &trackData);
    void updateTrackPlayed(const QString &location);
    void updateTrackArt(const QString &path, const QString &artist, const QString &album);
    int updateTrack(const QString &column, const QString &location, const QVariant &value);
    QList<QMap<int, QString>> getTrackData(const QStringList &urls, int limit = -1, int offset = 0, const QString &orderBy = "album", bool descending = false);
    QList<QMap<int, QString>> getTrackData(const QVariantMap &where, int limit = -1, int offset = 0, const QString &orderBy = "album", bool descending = false, const QString &whereOperator = "AND", const QString &whereComparator = "=");
    QVariantList loadTracks(const QVariantMap &where, int limit = -1, int offset = 0, const QString &orderBy = "album", bool descending = false, const QString &whereOperator = "AND", const QString &whereComparator = "=");

    QList<QString> albums();
    bool albumExists(const QVariantMap &data);
    void saveAlbum(const QVariantMap &albumData, const QVariantMap &where = QVariantMap());
    QString getAlbumArt(const QString &title, const QString &artist);
    QVariantList getAlbumsData(const QVariantMap &where, int limit = -1, int offset = 0, const QString &orderBy = "title", bool descending = false, const QString &whereOperator = "AND", const QString &whereComparator = "=");

    QList<QString> artists();
    bool artistExists(const QVariantMap &data);
    void saveArtist(const QVariantMap &data, const QVariantMap &where = QVariantMap());
    QString getArtistArt(const QString &artist);
    QVariantList getArtistData(const QVariantMap &where, int limit = -1, int offset = 0, const QString &orderBy = "title", bool descending = false, const QString &whereOperator = "AND", const QString &whereComparator = "=");

    QStringList getPlaylists(const QString &property = "title");
    QStringList getPlaylistsMoods();

    enum colums
    {
        TRACK, TITLE, ARTIST, ALBUM, GENRE, LOCATION, STARS, BABE, ART, PLAYED, PLAYLIST
    };

private:
    QList<QString> m_albums;
    QList<QString> m_artists;
    QList<Track> m_trackList;
    AlbumsDB *m_albumsDB;
    ArtistsDB *m_artistsDB;
    PlaylistsDB *m_playlistsDB;
    TracksDB *m_tracksDB;

public slots:
    bool addTrack(const QStringList &paths, int babe = 0);
    void removeTrack(const QString &path);
    void insertPlaylist(const QString &name);
    void setCollectionLists();
    void refreshArtistsTable();
    void cleanCollectionLists();
    void insertCoverArt(const QString &path, const QStringList &info);
    void insertHeadArt(const QString &path, const QStringList &info);

signals:
    void progress(int);
    void dbActionFinished(bool state);
};

#endif // COLLECTION_H
