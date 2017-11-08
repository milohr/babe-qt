#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <QObject>
#include "../pulpo.h"


class spotify : public Pulpo
{
    Q_OBJECT




private:
    QString API = "https://api.spotify.com/v1/search?q=";
    Bae::DB track;
    QByteArray array;

public:

    static const QString auth;

    explicit spotify(const Bae::DB &song);

    bool setUpService(const Pulpo::ONTOLOGY &type);

    bool parseArtist(const Pulpo::INFO &infoType);
    bool parseAlbum(const Pulpo::INFO &infoType);
    bool parseTrack(const  Pulpo::INFO &infoType);

    QVariant getTrackInfo(const INFO &infoType);

};

#endif // SPOTIFY_H
