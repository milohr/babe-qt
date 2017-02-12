#ifndef ARTWORK_H
#define ARTWORK_H
#include <QPixmap>
#include <QDebug>
#include <QImage>
#include <QtCore>
#include <QtNetwork>
#include <QUrl>
#include <QWidget>
#include <QObject>
#include <QNetworkAccessManager>

class ArtWork : public QObject
{
    Q_OBJECT

public:
    explicit ArtWork(QObject *parent = 0);
    void setDataCover(QString artist, QString album,QString title, QString path="");
    void setDataHead(QString artist, QString path="");
     void setDataHead_asCover(QString artist, QString path="");
    void setDataCoverInfo(QString artist, QString album);
    void setDataHeadInfo(QString artist);
    void setDataCover_title(QString artist, QString title);
    QByteArray getCover();
    QString getInfo();
    QByteArray selectCover(QString url);
    void selectHead(QString url);
    void selectInfo(QString info);
    QString info;
    QString bio;
    QString fixTitle(QString title, QString s, QString e);
    QString removeFeat(QString newTitle);
    QString removeOfficial(QString newTitle);

    void startConnection();

    enum ART
    {
        ALBUM,ARTIST,ALBUM_INFO,ARTIST_INFO,ALBUM_by_TITLE,ARTIST_COVER
    };

private:
    QString url ;
    //QImage cover;
    QPixmap art;
    QString album;
    QString artist;
    QString title;
    QString xmlData;
    QByteArray coverArray;
    QString path;
    int type;

    //QNetworkReply reply;

private slots:

    void xmlInfo(QNetworkReply* reply);
    void saveArt(QByteArray array);
    void dummy();
signals:
    void pixmapReady(QImage *pix);
    void coverReady(QByteArray array);
    void headReady(QByteArray array);
    void bioReady(QString bio);
    void infoReady(QString info);
    void artSaved(QString path,QStringList info);

};

#endif // ARTWORK_H
