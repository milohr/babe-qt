#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <QObject>
#include "../pulpo.h"


class spotify : public Pulpo
{
    Q_OBJECT

private:

    QString API = "https://api.spotify.com/v1/search?q=";

    QString artist;
    QString album;
    QString title;

public:

    enum Ontology
    {
        ARTIST,ALBUM,TRACK
    };

    explicit spotify(const QString &title_, const QString &artist_, const QString &album_);

    QString setUpService(const spotify::Ontology &type);

    bool parseSpotifyArtist(const QByteArray &array, const Pulpo::ArtistInfo &infoType);
    bool parseSpotifyAlbum(const QByteArray &array, const Pulpo::AlbumInfo &infoType);
    bool parseSpotifyTrack(const QByteArray &array,const  Pulpo::TrackInfo &infoType);

    QVariant getTrackInfo(const QByteArray &array, const Pulpo::TrackInfo &infoType);

};

#endif // SPOTIFY_H
