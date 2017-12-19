#ifndef MUSICBRAINZSERVICE_H
#define MUSICBRAINZSERVICE_H
#include <QObject>
#include "../pulpo.h"

using namespace BAE;

class musicBrainz : public Pulpo
{
    Q_OBJECT

private:
    const QString API = "http://musicbrainz.org/ws/2/";
    const QMap<QString,QString> header = { {"User-Agent", "Babe/1.0 ( babe.kde.org )"} };

public:
    explicit musicBrainz(const DB &song);
    virtual bool setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info);

protected:
    virtual bool parseArtist();
    virtual bool parseAlbum();
    virtual bool parseTrack();
};

#endif // MUSICBRAINZSERVICE_H
