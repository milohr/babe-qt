#include "lyricwikiaService.h"

lyricWikia::lyricWikia(const QString &title_, const QString &artist_, const QString &album_) :
     artist(artist_),  album(album_), title(title_)  {}

QString lyricWikia::setUpService()
{
    QString url = this->API;

    QUrl encodedArtist(this->artist);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    QUrl encodedTrack(this->title);
    encodedTrack.toEncoded(QUrl::FullyEncoded);

    url.append("&artist=" + encodedArtist.toString());
    url.append("&song=" + encodedTrack.toString());
    url.append("&fmt=xml");

    qDebug()<<"setUpService lyricWikia["<<url<<"]";
    return url;
}



bool lyricWikia::parseLyrics(const QByteArray &array)
{

    QString xmlData(array);

    QDomDocument doc;

    if (!doc.setContent(xmlData))
    {
        qDebug()<<"The XML obtained from lyricWikia is invalid.";
        return false;
    }else
    {

        QString temp = doc.documentElement().namedItem("url").toElement().text().toLatin1();
        QUrl temp_u (temp);
        temp_u.toEncoded(QUrl::FullyEncoded);

        temp =temp_u.toString();

        temp.replace("http://lyrics.wikia.com/","http://lyrics.wikia.com/index.php?title=");
        temp.append("&action=edit");
        QRegExp url_regexp("<url>(.*)</url>");
        url_regexp.setMinimal(true);
        QUrl url = QUrl::fromEncoded(temp.toLatin1());
        QString referer = url_regexp.cap(1);

        QNetworkRequest request;
        request.setUrl(url);
        request.setRawHeader("Referer", referer.toLatin1());
        qDebug("Receiving lyrics");

        QNetworkAccessManager m_http;

        QNetworkReply *reply  = m_http.get(request);

        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
        loop.exec();

        QByteArray array(reply->readAll());
        delete reply;

        if(array.isEmpty()) return false;
        else
        {
            this->extractLyrics(array);
            return true;

        }
    }

}

void lyricWikia::extractLyrics(const QByteArray &array)
{
    QString content = QString::fromUtf8(array.constData());

    //QString text;
    content.replace("&lt;", "<");
    QRegExp lyrics_regexp("<lyrics>(.*)</lyrics>");
    lyrics_regexp.indexIn(content);

    QString text = "<h2 align='center' >" + this->title + "</h2>";
    QString lyrics = lyrics_regexp.cap(1);
    lyrics = lyrics.trimmed();
    lyrics.replace("\n", "<br>");
    if(lyrics.isEmpty())
    {
        qDebug("Not found");
        text+="\n<h3 align='center'>:( Nothing Here</h3>";
    }
    else text += lyrics;

    text= "<p align='center'>"+text+"<p>";
    emit trackLyricsReady(text);

}
