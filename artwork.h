#pragma once

#include <QPixmap>
#include <QDebug>
#include <QImage>
#include <QtCore>
#include <QtNetwork>
#include <QUrl>
#include <QWidget>
#include <QObject>
#include <QNetworkAccessManager>
#include <QDomDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QVariantMap>


class ArtWork : public QObject
{
    Q_OBJECT

public:
    explicit ArtWork(QObject *parent = 0);
    ~ArtWork();
    void setDataCover(QString artist, QString album,QString title, QString path="");
    void setDataHead(QString artist, QString path="");
     void setDataHead_asCover(QString artist);
    void setDataCoverInfo(QString artist, QString album);
    void setDataHeadInfo(QString _artist);
    void setDataCover_title(QString artist, QString title);
    void setDataCover_spotify(QString artist, QString album,QString title);
    void setDataCover_itunes(QString artist, QString album,QString title);
    QByteArray getCover();
    QByteArray selectCover(QString url);
    void selectHead(QString url);

    QString getAlbumTitle_Spotify(QString artist, QString title);
    QString getAlbumTitle(QString artist, QString title);


    void startConnection(bool json=false);

    enum ART
    {
        ALBUM,ARTIST,ALBUM_INFO,ARTIST_INFO,ALBUM_by_TITLE,ARTIST_COVER,ALBUM_TITLE,ALBUM_by_SPOTIFY,ALBUM_by_ITUNES
    };

private:
    QString url;
    QPixmap art;
    QString album;
    QString artist;
    QString title;
    QString xmlData;
    QByteArray coverArray;
    QString path;
    int type;

public slots:

    void xmlInfo(QNetworkReply* reply);
    void jsonInfo(QNetworkReply *reply);
    void saveArt(QByteArray array);
    void dummy();
signals:
    void pixmapReady(QImage *pix);
    void coverReady(QByteArray array);
    void headReady(QByteArray array);
    void bioReady(QString bio);
    void tagsReady(QStringList tags);
    void similarArtistsReady(QMap<QString,QByteArray> info);
    void infoReady(QString info);

    void artSaved(QString path,QStringList info);
    void albumTitleReady(QString title);

};

