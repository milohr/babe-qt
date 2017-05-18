#include "geniusService.h"
#include <QObject>


genius::genius(const QString &title_, const QString &artist_, const QString &album_) :
    artist(artist_),  album(album_), title(title_)  {}

QString genius::setUpService()
{
    QString url = this->API;

    QUrl encodedArtist(this->artist);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    QUrl encodedTrack(this->title);
    encodedTrack.toEncoded(QUrl::FullyEncoded);

    url.append(encodedArtist.toString()+" "+encodedTrack.toString());

    qDebug()<<"setUpService genius["<<url<<"]";
    return url;
}

void genius::parseLyrics(const QByteArray &array)
{
    htmlParser parser;
    parser.setHtml(array);

    connect(&parser, &htmlParser::finishedParsingTags,[&parser, this] (const QStringList &tags)
    {
        if(!tags.isEmpty())
            extractLyrics(parser.extractProp(tags.first(),"href="));
    });

    parser.parseTag("li", "class=\"search_result\"");

}

void genius::parseAlbumArt(const QByteArray &array)
{
    htmlParser parser;
    parser.setHtml(array);

    connect(&parser, &htmlParser::finishedParsingTags,[&parser, this] (const QStringList &tags)
    {
        if(!tags.isEmpty())
        {
            extractAlbumArt(parser.extractProp(tags.first(),"href="));
        }
    });

    parser.parseTag("li", "class=\"search_result\"");
}


void genius::extractAlbumArt(const QString &url)
{

    QNetworkAccessManager manager;
    QNetworkRequest request ((QUrl(url)));
    QNetworkReply *reply =  manager.get(request);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
            SLOT(quit()));

    loop.exec();

    QByteArray array(reply->readAll());
    delete reply;

    htmlParser parser;
    parser.setHtml(array);
    qDebug()<<"Artwork now";

    connect(&parser, &htmlParser::finishedParsingTags,[&parser, this] (const QStringList &list)
    {
        if(!list.isEmpty())
        {
            emit albumArtReady(Pulpo::extractImg(parser.extractProp(list.first(),"src=")));
        }
    });
 parser.parseTag("img", "class=\"cover_art-image\"");

}


void genius::extractLyrics(const QString &url)
{

    QNetworkAccessManager manager;
    QNetworkRequest request ((QUrl(url)));
    QNetworkReply *reply =  manager.get(request);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
            SLOT(quit()));

    loop.exec();

    QByteArray array(reply->readAll());
    delete reply;

    htmlParser parser;
    parser.setHtml(array);
    qDebug()<<"Lyricsssss now";

    connect(&parser, &htmlParser::finishedParsingTags,[&parser, this] (const QStringList &list)
    {
        if(!list.isEmpty())
        {
            QString text = "<h2 align='center' >" + this->title + "</h2>";
            auto lyrics = list.first();

            lyrics=lyrics.trimmed();
            lyrics.replace("\n", "<br>");
            if(lyrics.isEmpty())
            {
                qDebug("Not found");
                text+="\n<h3 align='center'>:( Nothing Here</h3>";
            }
            else text += lyrics;

            text= "<div id='geniusLyrics' align='center'>"+text+"</div>";
            emit trackLyricsReady(text);

        }
    });
 parser.parseTag("lyrics", "class=\"lyrics\"");

}
