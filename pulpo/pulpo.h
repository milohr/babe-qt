#ifndef PULPO_H
#define PULPO_H

#include <QPixmap>
#include <QList>
#include <QDebug>
#include <QImage>
#include <QtCore>
#include <QtNetwork>
#include <QUrl>
#include <QWidget>
#include <QObject>
#include <QNetworkAccessManager>
#include <QDomDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QVariantMap>




class Pulpo : public QObject
{
    Q_OBJECT

public:

    enum Ontology
    {
        ARTIST,ALBUM,TRACK,GENRE
    };

    enum InfoServices
    {
        LastFm,Spotify,iTunes,AllInfoServices, infoCRAWL
    };

    enum LyricServices
    {
        LyricWikia,WikiLyrics,Lyrics,AllLyricServices, lyricCRAWL
    };

    enum ArtistInfo
    {
        ArtistArt,ArtistWiki,ArtistSimilar,ArtistTags,AllArtistInfo,NoneArtistInfo
    };

    enum AlbumInfo
    {
        AlbumArt,AlbumWiki,AlbumTracks,AlbumTags,AllAlbumInfo,NoneAlbumInfo
    };

    enum TrackInfo
    {
        TrackLyrics,TrackAlbum,TrackPosition, TrackWiki,TrackTags,AllTrackInfo,
    };

    enum ResponseType
    {
        XML,JSON
    };

    explicit Pulpo(QString title="", QString artist ="", QString album="", QObject *parent = 0);
    ~Pulpo();

    void feed(const QString &title, const QString &artist, const QString &album);
    QVariant getStaticAlbumInfo(const AlbumInfo &infoType);
    QVariant getStaticArtistInfo(const ArtistInfo &infoType);
    QVariant getStaticTrackInfo(const TrackInfo &infoType);


    bool fetchArtistInfo(const ArtistInfo &infoType = AllArtistInfo, const InfoServices &service = AllInfoServices, const bool &recursive = false);

    bool fetchAlbumInfo(const AlbumInfo &infoType = AllAlbumInfo, const InfoServices &service = AllInfoServices , const bool &recursive =false);

    bool fetchTrackInfo(const TrackInfo &infoType = AllTrackInfo, const LyricServices &lyricService = AllLyricServices, const InfoServices &services = AllInfoServices );

    QByteArray extractImg(QString url);

    QByteArray startConnection(const QString &url);


private:

    QString lyricWikiaAPI = "http://lyrics.wikia.com/api.php?action=lyrics";

    QPixmap art;
    QString album;
    QString artist;
    QString title;

public slots:

    void saveArt(const QByteArray &array, const QString &path);
    void dummy();

signals:

    void albumArtReady(QByteArray art);
    void albumWikiReady(QString wiki);
    void albumTracksReady(QStringList tracks);
    void albumTagsReady(QStringList tags);

    void artistArtReady(QByteArray art);
    void artistWikiReady(QString wiki);
    void artistSimilarReady(QMap<QString,QByteArray> similar);
    void artistTagsReady(QStringList tags);

    void trackLyricsReady(QString lyric);
    void trackWikiReady(QString wiki);
    void trackAlbumReady(QString album);
    void trackPositionReady(int position);
    void trackTagsReady(QStringList tags);

    void artSaved(QString path,QStringList info);

};

#endif // ARTWORK_H
