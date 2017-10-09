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
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QSqlQuery>
#include "../baeUtils.h"
#include "pulpo/webengine.h"




class Pulpo : public QObject
{
    Q_OBJECT

public:

    explicit Pulpo(const Bae::DB &song, QObject *parent = 0);
    explicit Pulpo(QObject *parent = 0);
    ~Pulpo();

    enum Ontology
    {
        ARTIST,ALBUM,TRACK,GENRE
    };

    enum InfoServices
    {
        LastFm,Spotify,iTunes,GeniusInfo,AllInfoServices, infoCRAWL, NoneInfoService
    };

    enum LyricServices
    {
        LyricWikia,WikiLyrics,Lyrics, Genius,AllLyricServices, lyricCRAWL, NoneLyricService
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
        TrackLyrics,TrackAlbum,TrackPosition, TrackWiki,TrackTags,AllTrackInfo,NoneTrackInfo
    };

    enum ResponseType
    {
        XML,JSON
    };



    void feed(const Bae::DB &song);
    QVariant getStaticAlbumInfo(const AlbumInfo &infoType);
    QVariant getStaticArtistInfo(const ArtistInfo &infoType);
    QVariant getStaticTrackInfo(const TrackInfo &infoType);


    bool fetchArtistInfo(const ArtistInfo &infoType = AllArtistInfo, const InfoServices &service = AllInfoServices);

    bool fetchAlbumInfo(const AlbumInfo &infoType = AllAlbumInfo, const InfoServices &service = AllInfoServices );

    bool fetchTrackInfo(const TrackInfo &infoType = AllTrackInfo, const LyricServices &lyricService = AllLyricServices, const InfoServices &services = AllInfoServices );

    QByteArray startConnection(const QString &url);


private:

    QPixmap art;
    Bae::DB track;
    webEngine *page;

public slots:

    void saveArt(const QByteArray &array, const QString &path);
    void dummy();

signals:

    void albumArtReady(const QByteArray &art);
    void albumWikiReady(const QString &wiki,const Bae::DB &track);
    void albumTracksReady(const QStringList &tracks,const Bae::DB &track);
    void albumTagsReady(const QStringList &tags,const Bae::DB &track);

    void artistArtReady(const QByteArray &art);
    void artistWikiReady(const QString &wiki,const Bae::DB &track);
    void artistSimilarReady(const QMap<QString,QByteArray> &similar,const Bae::DB &track);
    void artistTagsReady(const QStringList &tags,const Bae::DB &track);

    void trackLyricsReady(const QString &lyric,const Bae::DB &track);
    void trackLyricsUrlReady(const QUrl &url,const Bae::DB &track);
    void trackWikiReady(const QString &wiki,const Bae::DB &track);
    void trackAlbumReady(const QString &album,const Bae::DB &track);
    void trackPositionReady(const int &position,const Bae::DB &track);
    void trackTagsReady(const QStringList &tags,const Bae::DB &track);

    void artSaved(const Bae::DB &track);
};

#endif // ARTWORK_H
