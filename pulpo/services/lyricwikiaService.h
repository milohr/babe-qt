#ifndef LYRICWIKIASERVICE_H
#define LYRICWIKIASERVICE_H
#include <QObject>
#include "../pulpo.h"

class lyricWikia : public Pulpo
{
    Q_OBJECT

private:
    QString API = "http://lyrics.wikia.com/api.php?action=lyrics";

    Bae::DB track;

public:

    explicit lyricWikia(const Bae::DB &song);


    QString setUpService();

    bool parseLyrics(const QByteArray &array);

    void extractLyrics(const QByteArray &array);

};

#endif // LYRICWIKIASERVICE_H
