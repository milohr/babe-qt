#ifndef LYRICWIKIASERVICE_H
#define LYRICWIKIASERVICE_H
#include <QObject>
#include "../pulpo.h"

class lyricWikia : public Pulpo
{
    Q_OBJECT

private:
    const QString API = "http://lyrics.wikia.com/api.php?action=lyrics";

    bool extractLyrics(const QByteArray &array);

public:
    explicit lyricWikia(const BAE::DB &song);
    virtual bool setUpService(const ONTOLOGY &ontology, const INFO &info);

protected:
    virtual bool parseTrack();

};

#endif // LYRICWIKIASERVICE_H
