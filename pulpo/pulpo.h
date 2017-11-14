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



namespace PULPO
{

enum class SERVICES : uint8_t
{
    LastFm,
    Spotify,
    iTunes,
    MusicBrainz,
    Genius,
    LyricWikia,
    Wikipedia,
    WikiLyrics,
    ALL,
    NONE
};

enum class ONTOLOGY : uint8_t
{
    ARTIST,
    ALBUM,
    TRACK,
    NONE
};

enum class INFO : uint8_t
{
    ARTWORK,
    WIKI,
    TAGS,
    METADATA,
    LYRICS,
    ALL,
    NONE
};

/*Generic context names. It's encouraged to use these instead of a unkown string*/
enum class CONTEXT : uint8_t
{
    STAT,
    TRACK_NUMBER,
    TRACK_TITLE,
    TRACK_DATE,
    TRACK_TEAM,
    TRACK_AUTHOR,
    TRACK_LANGUAGE,
    TRACK_SIMILAR,

    ALBUM_TITLE,
    ALBUM_DATE,
    ALBUM_LANGUAGE,
    ALBUM_SIMILAR,
    ALBUM_LABEL,

    ARTIST_TITLE,
    ARTIST_DATE,
    ARTIST_LANGUAGE,
    ARTIST_PLACE,
    ARTIST_SIMILAR,
    ARTIST_TEAM,
    ARTIST_ALIAS,
    ARTIST_GENDER,

    GENRE,
    TAG,
    WIKI,
    IMAGE,
    LYRIC,

};

static const QMap<CONTEXT,QString> CONTEXT_MAP =
{
    {CONTEXT::STAT, "stat"},

    {CONTEXT::ALBUM_TITLE, "album_title"},
    {CONTEXT::ALBUM_DATE, "album_date"},
    {CONTEXT::ALBUM_LANGUAGE, "album_language"},
    {CONTEXT::ALBUM_SIMILAR, "album_similar"},
    {CONTEXT::ALBUM_LABEL, "album_label"},

    {CONTEXT::ARTIST_TITLE, "artist_title"},
    {CONTEXT::ARTIST_DATE, "artist_date"},
    {CONTEXT::ARTIST_LANGUAGE, "artist_language"},
    {CONTEXT::ARTIST_PLACE, "artist_place"},
    {CONTEXT::ARTIST_SIMILAR, "artist_similar"},
    {CONTEXT::ARTIST_ALIAS, "artist_alias"},
    {CONTEXT::ARTIST_GENDER, "artist_gender"},

    {CONTEXT::TRACK_DATE, "track_date"},
    {CONTEXT::TRACK_TITLE, "track_title"},
    {CONTEXT::TRACK_NUMBER, "track_number"},
    {CONTEXT::TRACK_TEAM, "track_team"},
    {CONTEXT::TRACK_AUTHOR, "track_author"},
    {CONTEXT::TRACK_LANGUAGE, "track_language"},
    {CONTEXT::TRACK_SIMILAR, "track_similar"},

    {CONTEXT::GENRE, "genre"},
    {CONTEXT::TAG, "tag"},
    {CONTEXT::WIKI, "wiki"},
    {CONTEXT::IMAGE, "image"},
    {CONTEXT::LYRIC, "lyric"},

};

enum class RECURSIVE : bool
{
    ON = true,
    OFF = false
};

typedef QMap<CONTEXT, QVariant> CONTEXT_K;
typedef QMap<INFO, CONTEXT_K> RESPONSE;
typedef QMap<ONTOLOGY, QList<INFO>> AVAILABLE;

}

using namespace PULPO;

class Pulpo : public QObject
{
    Q_OBJECT

public:



    explicit Pulpo(const Bae::DB &song, QObject *parent = nullptr);
    explicit Pulpo(QObject *parent = nullptr);
    ~Pulpo();

    void feed(const Bae::DB &song, const RECURSIVE &recursive = RECURSIVE::ON );
    QByteArray startConnection(const QString &url, const QMap<QString, QString> &headers = {});

    void registerServices(const QList<SERVICES> &services);
    void setInfo(const INFO &info);
    void setOntology(const ONTOLOGY &ontology);
    void setRecursive(const RECURSIVE &state);

private:
    bool initServices();
    RECURSIVE recursive = RECURSIVE::ON;
    QList<SERVICES> registeredServices = {SERVICES::ALL};

    void passSignal(const Bae::DB &track, const PULPO::RESPONSE &response);

protected:
    QByteArray array;
    Bae::DB track;
    INFO info = INFO::NONE;
    ONTOLOGY ontology = ONTOLOGY::NONE;
    AVAILABLE availableInfo;

    RESPONSE packResponse(const INFO &infoKey, const CONTEXT &contextName, const QVariant &value);
    RESPONSE packResponse(const PULPO::INFO &infoKey, const CONTEXT_K &map);
    /* expected methods to be overrided by services */
    bool setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info);
    bool parseArray();
    virtual bool parseArtist() {return false;}
    virtual bool parseAlbum() {return false;}
    virtual bool parseTrack() {return false;}

public slots:    
    void saveArt(const QByteArray &array, const QString &path);

signals:
    void infoReady(const Bae::DB &track, const PULPO::RESPONSE &response);
    void artSaved(const Bae::DB &track);
};

#endif // ARTWORK_H
