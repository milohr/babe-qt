#include "geniusService.h"
#include <QObject>



QString genius::API =  "https://genius.com/search?q=";

genius::genius(const Bae::TRACKMAP &song) :
    track(song) {}

 QString genius::setUpService(const Bae::TRACKMAP &song)
{
    QString url = genius::API;

    QUrl encodedArtist(song[Bae::TracksCols::ARTIST]);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    QUrl encodedTrack(song[Bae::TracksCols::TITLE]);
    encodedTrack.toEncoded(QUrl::FullyEncoded);

    url.append(encodedArtist.toString()+" "+encodedTrack.toString());

    qDebug()<<"setUpService genius["<<url<<"]";
    return url;
}

void genius::parseLyrics(const QByteArray &array)
{
    htmlParser parser;
    parser.setHtml(array);

    connect(&parser, &htmlParser::finishedParsingTags,[this] (const QStringList &tags)
    {
        qDebug()<<"GENIUS RESULT TAGS:"<<tags;
        htmlParser parser;
        if(!tags.isEmpty())
            extractLyrics(parser.extractProp(tags.first(),"href="));
    });

    parser.parseTag("a", "class=\"mini_card\"");
}

void genius::parseAlbumArt(const QByteArray &array)
{
    htmlParser parser;
    parser.setHtml(array);

    connect(&parser, &htmlParser::finishedParsingTags,[&parser, this] (const QStringList &tags)
    {
        if(!tags.isEmpty())
        {
            extractAlbumArt(parser.extractProp(tags.first(),"ng-href="));
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

    connect(&parser, &htmlParser::finishedParsingTags,[this] (const QStringList &list)
    {
        if(!list.isEmpty())
        {
            htmlParser parser;
            emit albumArtReady(Pulpo::startConnection(parser.extractProp(list.first(),"src=")));
        }
    });
 parser.parseTag("img", "class=\"cover_art-image\"");

}


void genius::extractLyrics(const QString &url)
{

    qDebug()<<"extractLyrics"<<url;
    emit trackLyricsReady(url,this->track);

//    QNetworkAccessManager manager;
//    QNetworkRequest request ((QUrl(url)));
//    QNetworkReply *reply =  manager.get(request);
//    QEventLoop loop;
//    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
//    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
//            SLOT(quit()));

//    loop.exec();

//    QByteArray array(reply->readAll());
//    delete reply;

//    htmlParser parser;

////    qDebug()<<"LYRICS ARRAY"<<array;

//    parser.setHtml(array);
//    qDebug()<<"Lyricsssss now";

//    connect(&parser, &htmlParser::finishedParsingTags,[this] (const QStringList &list)
//    {
//        qDebug()<<"got lyrics"<<list;
//        if(!list.isEmpty())
//        {
//            QString text = "<h2 align='center' >" + this->track[Bae::TracksCols::TITLE] + "</h2>";
//            auto lyrics = list.first();

//            lyrics=lyrics.trimmed();
//            lyrics.replace("\n", "<br>");
//            if(lyrics.isEmpty())
//            {
//                qDebug("Not found");
//                text+="\n<h3 align='center'>:( Nothing Here</h3>";
//            }
//            else text += lyrics;

//            text= "<div id='geniusLyrics' align='center'>"+text+"</div>";
//            emit trackLyricsReady(text,this->track);

//        }
//    });
// parser.parseTag("div", "class=\"lyrics\"");

}
