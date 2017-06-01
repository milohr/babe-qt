#ifndef LASTFM_H
#define LASTFM_H

#include "../pulpo.h"

class QByteArray;
class QDomDocument;
class QDomNode;
class QDomNodeList;
class QJsonDocument;
class QJsonObject;
class QJsonParseError;
class QString;

class lastfm : public Pulpo
{
    Q_OBJECT
private:
    QString API = "http://ws.audioscrobbler.com/2.0/";
    QString KEY = "&api_key=ba6f0bd3c887da9101c10a50cf2af133";
    QString artist;
    QString album;
    QString title;

public:
    enum Ontology
    {
        ARTIST,ALBUM,TRACK
    };
    explicit lastfm(const QString &title_, const QString &artist_, const QString &album_);
    QString setUpService(const lastfm::Ontology &type);
    bool parseLastFmArtist(const QByteArray &array, const Pulpo::ArtistInfo &infoType);
    bool parseLastFmAlbum(const QByteArray &array,  const Pulpo::AlbumInfo &infoType);
    bool parseLastFmTrack(const QByteArray &array, const Pulpo::TrackInfo &infoType);
    QVariant getTrackInfo(const QByteArray &array, const TrackInfo &infoType);
};

#endif // LASTFM_H
