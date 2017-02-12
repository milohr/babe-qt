#include "lyrics.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QDomDocument>
#include <QRegExp>

Lyrics::Lyrics(QObject *parent) : QObject(parent)
{
    url = "http://lyrics.wikia.com/api.php?action=lyrics";
    QString genius_key = "pkeVfwpHfXNX1ulZfZlxjSAs48T_l99NM4j2DOa71DWxnmDP21iz0dM48Y_NKjrZ";
}


void Lyrics::setData(QString artist, QString song)
{



    this->artist=artist;
    this->song = song;
    QUrl q_artist (artist);
    q_artist.toEncoded(QUrl::FullyEncoded);
    QUrl q_song (song);
    q_song.toEncoded(QUrl::FullyEncoded);

    if (!q_artist.isEmpty()) url.append("&artist=" + q_artist.toString());
    if (!q_song.isEmpty()) url.append("&song=" + q_song.toString());
    url.append("&fmt=xml");
    startConnection();
}

void Lyrics::startConnection()
{


    QNetworkAccessManager manager;

    QNetworkReply *reply  = manager.get(QNetworkRequest(QUrl(url)));

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    QObject::connect(&manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(xmlInfo(QNetworkReply*)));
    loop.exec();


    //qDebug()<<url;
    delete reply;

}


void Lyrics::xmlInfo(QNetworkReply *reply)
{

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bts = reply->readAll();
        QString xmlData(bts);
        //qDebug()<<xmlData;
        //QString info;
        //qDebug()<<xmlData;
        QDomDocument doc;

        if (!doc.setContent(xmlData))
        {
            qDebug()<<"The XML obtained from last.fm "
                      "is invalid.";
        }else
        {



            QString temp = doc.documentElement().namedItem("url").toElement().text().toLatin1();
            QUrl temp_u (temp);
            temp_u.toEncoded(QUrl::FullyEncoded);
            //qDebug("LyricsWindow: received url = %s", qPrintable(temp));


            temp =temp_u.toString();

            temp.replace("http://lyrics.wikia.com/","http://lyrics.wikia.com/index.php?title=");
            temp.append("&action=edit");
            //qDebug() << temp;
            QRegExp url_regexp("<url>(.*)</url>");
            url_regexp.setMinimal(true);
            QUrl url = QUrl::fromEncoded(temp.toLatin1());
            QString referer = url_regexp.cap(1);
            //  qDebug("LyricsWindow: request url = %s", url.toEncoded().constData());
            QNetworkRequest request;
            request.setUrl(url);
            request.setRawHeader("Referer", referer.toLatin1());
            qDebug("Receiving lyrics");


            QNetworkAccessManager m_http;

            QNetworkReply *reply  = m_http.get(request);

            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
            QObject::connect(&m_http,SIGNAL(finished(QNetworkReply*)),this,SLOT(getLyrics(QNetworkReply*)));
            loop.exec();


            //qDebug()<<url;
            delete reply;
        }

    }
}

void Lyrics::getLyrics(QNetworkReply *reply)
{
    QString content = QString::fromUtf8(reply->readAll().constData());

    //QString text;
    content.replace("&lt;", "<");
    QRegExp lyrics_regexp("<lyrics>(.*)</lyrics>");
    lyrics_regexp.indexIn(content);

    QString text = "<h2 align='center' >" + song + "</h2>";
    QString lyrics = lyrics_regexp.cap(1);
    lyrics = lyrics.trimmed();
    lyrics.replace("\n", "<br>");
    if(lyrics.isEmpty())
    {
        qDebug("Not found");
        text+="\n<h3 align='center'>:( Nothing Here</h3>";
    }
    else
    {
        text += lyrics;
        // qDebug()<<text;
    }
    reply->deleteLater();

    //lyric = doc.documentElement().namedItem("lyrics").toElement().text();

    if(text.isEmpty())
    {
        qDebug()<<"Could not find " <<
                  " lyrics ";


    }else
    {
        // qDebug()<<"the lyrics are"<< lyric;
        text= "<p align='center'>"+text+"<p>";

 emit lyricsReady(text);

        //disconnect(this, SIGNAL(lyricsReady(QString)), 0, 0);


    }

}


