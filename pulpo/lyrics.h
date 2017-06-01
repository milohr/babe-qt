#ifndef LYRICS_H
#define LYRICS_H

#include <QObject>

class QtNetwork;
class QUrl;
class QWidget;
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;
class QDomDocument;
class QRegExp;

class Lyrics : public QObject
{
    Q_OBJECT
public:
    explicit Lyrics(QObject *parent = 0);
    QString lyric;
    QString url;
    QString artist;
    QString song;
    void startConnection();
    void setData(QString artist,QString song);
    QNetworkAccessManager *m_http;

signals:
    void lyricsReady(QString lyrics);

public slots:
    void xmlInfo(QNetworkReply *reply);
    void getLyrics(QNetworkReply *reply);
};

#endif // LYRICS_H
