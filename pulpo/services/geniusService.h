#ifndef GENIUSSERVICE_H
#define GENIUSSERVICE_H

#include <QObject>
#include "../pulpo.h"
#include "../htmlparser.h"

class genius : public Pulpo
{
    Q_OBJECT
public:
    explicit genius(const QString &title_, const QString &artist_, const QString &album_);

    QString setUpService();

    void parseLyrics(const QByteArray &array);
    void parseAlbumArt(const QByteArray &array);
    void extractLyrics(const QByteArray &array);
    void extractAlbumArt(const QString &url);
    bool neededTag(const QString &txt, const QString &tag);

    void extractLyrics(const QString &url);


private:
    QString KEY = "UARllo5N6CLQYVlqFwolyauSlYiyU_07YTg7HGHkWRbimN4GWPJehPP5fzu9lXeO";
    QString API = "https://genius.com/search?q=";
    QString artist;
    QString album;
    QString title;
};

#endif // GENIUSSERVICE_H
