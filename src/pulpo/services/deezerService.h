#ifndef DEEZERSERVICE_H
#define DEEZERSERVICE_H

#include <QObject>
#include "../pulpo.h"

class deezer : public Pulpo
{
    Q_OBJECT

private:
    const QString API = "https://api.deezer.com/search?q=";

    QString getID(const QString &url);
    bool getAlbumInfo(const QByteArray &array);
    bool extractLyrics(const QByteArray &array);

public:
    explicit deezer(const BAE::DB &song);
    virtual bool setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info);

protected:
    virtual bool parseArtist();
    virtual bool parseAlbum();
    virtual bool parseTrack();
};

#endif // DEEZERSERVICE_H
