#ifndef LASTFM_H
#define LASTFM_H

#include <QObject>
#include "../pulpo.h"

class lastfm : public Pulpo
{
    Q_OBJECT

private:

    const QString API = "http://ws.audioscrobbler.com/2.0/";
    const QString KEY = "&api_key=ba6f0bd3c887da9101c10a50cf2af133";

    QByteArray array;

    Pulpo::ONTOLOGY ontology = Pulpo::ONTOLOGY::NONE;

    Bae::DB track;

    bool parseArtist(const Pulpo::INFO &infoType = Pulpo::INFO::ALL);
    bool parseAlbum(const Pulpo::INFO &infoType = Pulpo::INFO::ALL);
    bool parseTrack(const Pulpo::INFO &infoType = Pulpo::INFO::ALL);


public:

    explicit lastfm(const Bae::DB &song);

    bool setUpService(const Pulpo::ONTOLOGY &type);

    bool parseArray(const INFO &infoType = INFO::ALL);

    QVariant getTrackInfo(const QByteArray &array, const INFO &infoType);
};

#endif // LASTFM_H
