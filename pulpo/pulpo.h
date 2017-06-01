#ifndef PULPO_H
#define PULPO_H

#include <QObject>
#include <QPixmap>
#include <QMap>

class QByteArray;
class QDomDocument;
class QImage;
class QJsonDocument;
class QtNetwork;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QtCore;
class QWidget;
class QUrl;

class Pulpo : public QObject
{
    Q_OBJECT
public:
    explicit Pulpo(const QString &title = "", const QString &artist = "", const QString &album = "", QObject *parent = 0);
    ~Pulpo();

    enum Ontology
    {
        ARTIST, ALBUM, TRACK, GENRE
    };
    enum InfoServices
    {
        LastFm, Spotify, iTunes, AllInfoServices, infoCRAWL
    };
    enum LyricServices
    {
        LyricWikia, WikiLyrics, Lyrics, AllLyricServices, lyricCRAWL
    };
    enum ArtistInfo
    {
        ArtistArt, ArtistWiki, ArtistSimilar, ArtistTags, AllArtistInfo, NoneArtistInfo
    };
    enum AlbumInfo
    {
        AlbumArt, AlbumWiki, AlbumTracks, AlbumTags, AllAlbumInfo, NoneAlbumInfo
    };
    enum TrackInfo
    {
        TrackLyrics, TrackAlbum, TrackPosition, TrackWiki, TrackTags, AllTrackInfo,
    };
    enum ResponseType
    {
        XML, JSON
    };

    QByteArray extractImg(QString url);
    QByteArray startConnection(const QString &url);
    QVariant getStaticAlbumInfo(const AlbumInfo &infoType);
    QVariant getStaticArtistInfo(const ArtistInfo &infoType);
    QVariant getStaticTrackInfo(const TrackInfo &infoType);

    bool fetchArtistInfo(const ArtistInfo &infoType = AllArtistInfo, const InfoServices &service = AllInfoServices, const bool &recursive = false);
    bool fetchAlbumInfo(const AlbumInfo &infoType = AllAlbumInfo, const InfoServices &service = AllInfoServices , const bool &recursive =false);
    bool fetchTrackInfo(const TrackInfo &infoType = AllTrackInfo, const LyricServices &lyricService = AllLyricServices, const InfoServices &services = AllInfoServices );

private:
    QPixmap art;
    QString album;
    QString artist;
    QString title;

public slots:
    void saveArt(const QByteArray &array, const QString &path);
    void dummy();

signals:
    void albumArtReady(const QByteArray &art);
    void albumWikiReady(const QString &wiki);
    void albumTracksReady(const QStringList &tracks);
    void albumTagsReady(const QStringList &tags);

    void artistArtReady(const QByteArray &art);
    void artistWikiReady(const QString &wiki);
    void artistSimilarReady(const QMap<QString, QByteArray> &similar);
    void artistTagsReady(const QStringList &tags);

    void trackLyricsReady(const QString &lyric);
    void trackWikiReady(const QString &wiki);
    void trackAlbumReady(const QString &album);
    void trackPositionReady(int position);
    void trackTagsReady(const QStringList &tags);

    void artSaved(QString path,QStringList info);
};

#endif // ARTWORK_H
