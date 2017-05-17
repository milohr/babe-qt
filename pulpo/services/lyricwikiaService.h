#ifndef LYRICWIKIASERVICE_H
#define LYRICWIKIASERVICE_H
#include <QObject>
#include "../pulpo.h"

class lyricWikia : public Pulpo
{
    Q_OBJECT

private:
    QString API = "http://lyrics.wikia.com/api.php?action=lyrics";

    QString artist;
    QString album;
    QString title;

public:

    explicit lyricWikia(const QString &title_, const QString &artist_, const QString &album_);


    QString setUpService();

    bool parseLyrics(const QByteArray &array);

    void extractLyrics(const QByteArray &array);

};

#endif // LYRICWIKIASERVICE_H
