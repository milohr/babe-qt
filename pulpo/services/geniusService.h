#ifndef GENIUSSERVICE_H
#define GENIUSSERVICE_H

#include <QObject>
#include "../pulpo.h"
#include "../htmlparser.h"

class genius : public Pulpo
{
    Q_OBJECT    
private:
    const QString KEY = "UARllo5N6CLQYVlqFwolyauSlYiyU_07YTg7HGHkWRbimN4GWPJehPP5fzu9lXeO";
    const QString API = "https://genius.com/search?q=";

public:
    explicit genius(const Bae::DB &song);
    virtual bool setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info);


protected:
    virtual bool parseArtist();
    virtual bool parseAlbum();
    virtual bool parseTrack();


};

#endif // GENIUSSERVICE_H
