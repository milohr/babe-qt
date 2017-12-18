#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <QObject>
#include "../pulpo.h"


class spotify : public Pulpo
{
    Q_OBJECT

private:
    const QString API = "https://api.spotify.com/v1/search?q=";
    const QString CLIENT_ID = "a49552c9276745f5b4752250c2d84367";
    const QString CLIENT_SECRET = "b3f1562559f3405dbcde4a435f50089a";

public:
    explicit spotify(const BAE::DB &song);
    virtual bool setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &infoType);

protected:
    virtual bool parseArtist();
    virtual bool parseAlbum();
    virtual bool parseTrack();
};

#endif // SPOTIFY_H
