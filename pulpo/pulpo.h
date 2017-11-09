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


    typedef QMap<INFO, QVariant> RES;
    typedef QMap<ONTOLOGY, QList<INFO>> AVAILABLE;


    explicit Pulpo(const Bae::DB &song, QObject *parent = nullptr);
    explicit Pulpo(QObject *parent = nullptr);
    ~Pulpo();

    void feed(const Bae::DB &song, const bool &recursive=true);
    QByteArray startConnection(const QString &url, const QString &auth = "");

    void registerServices(const QList<SERVICES> &services);
    void setInfo(const INFO &info = INFO::ALL);
    void setOntology(const ONTOLOGY &ontology = ONTOLOGY::ALL);
    void setRecursive(const bool &state);

private:
    bool initServices();
    bool recursive = true;
    QList<SERVICES> registeredServices = {SERVICES::ALL};

    void passSignal(const Bae::DB &track, const Pulpo::RES &response);
    //    webEngine *page;

protected:
    QByteArray array;
    Bae::DB track;
    INFO info = INFO::ALL;
    ONTOLOGY ontology = ONTOLOGY::ALL;    
    AVAILABLE availableInfo;
    /* expected methods to be overrided by services */
    bool setUpService(const Pulpo::ONTOLOGY &ontology, const Pulpo::INFO &info);
    bool parseArray();
    bool parseArtist();
    bool parseAlbum();
    bool parseTrack();

public slots:    
    void saveArt(const QByteArray &array, const QString &path);

signals:
    void infoReady(const Bae::DB &track, const Pulpo::RES &response);
    void artSaved(const Bae::DB &track);
};

#endif // ARTWORK_H
