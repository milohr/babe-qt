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

    enum class SERVICES : uint8_t
    {
        LastFm = 0,
        Spotify = 1,
        iTunes = 2,
        MusicBrainz = 3,
        Genius = 4,
        LyricWikia = 5,
        Wikipedia = 6,
        WikiLyrics = 7,
        ALL = 8,
        NONE = 9
    };

    enum class ONTOLOGY : uint8_t
    {
        ARTIST,ALBUM,TRACK,GENRE,ALL,NONE
    };

    enum class INFO : uint8_t
    {
       ALBUM,ARTIST,ARTWORK,WIKI,ALBUM_TRACKS,TAGS,SIMILAR,LYRICS,TRACK,TITLE,ALL,NONE
    };

    typedef QMap<INFO, QVariant> RES; /* response type*/

    explicit Pulpo(const Bae::DB &song, QObject *parent = nullptr);
    explicit Pulpo(QObject *parent = nullptr);
    ~Pulpo();

    /* NEW INTRODUCED STUFF */

    void registerServices(const QList<SERVICES> &services);
    void setInfo(const ONTOLOGY ontology = ONTOLOGY::ALL, const INFO info = INFO::ALL);

    /* OLD STUFF TO REVIEW */

    enum ResponseType
    {
        XML,JSON
    };

    void feed(const Bae::DB &song);

    QVariant getStaticAlbumInfo(const INFO &infoType);
    QVariant getStaticArtistInfo(const INFO &infoType);
    QVariant getStaticTrackInfo(const INFO &infoType);

    QByteArray startConnection(const QString &url, const QString &auth="");


private:

    QPixmap art;
    Bae::DB track;
    webEngine *page;
    QList<SERVICES> registeredServices = {SERVICES::ALL};
    INFO info = INFO::ALL;
    ONTOLOGY ontology = ONTOLOGY::ALL;

    bool initServices();

public slots:

    void saveArt(const QByteArray &array, const QString &path);
    void dummy();

signals:

    void infoReady(const Bae::DB &track, const Pulpo::RES &response);

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
