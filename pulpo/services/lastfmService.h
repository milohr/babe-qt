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

public:
    explicit lastfm(const Bae::DB &song);
    virtual bool setUpService(const Pulpo::ONTOLOGY &ontology, const Pulpo::INFO &info);
    virtual bool parseArray();

protected:
    virtual bool parseArtist();
    virtual bool parseAlbum();
    virtual bool parseTrack();
};

#endif // LASTFM_H
