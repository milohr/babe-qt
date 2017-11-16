#include "lyricwikiaService.h"

lyricWikia::lyricWikia(const Bae::DB &song)
{
    this->availableInfo.insert(ONTOLOGY::TRACK, {INFO::LYRICS});
    this->track = song;
}

bool lyricWikia::setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info)
{
    this->ontology = ontology;
    this->info = info;

    if(!this->availableInfo[this->ontology].contains(this->info))
        return false;

    auto url = this->API;

    switch(this->ontology)
    {
    case PULPO::ONTOLOGY::TRACK:
    {
        QUrl encodedArtist(this->track[Bae::KEY::ARTIST]);
        encodedArtist.toEncoded(QUrl::FullyEncoded);

        QUrl encodedTrack(this->track[Bae::KEY::TITLE]);
        encodedTrack.toEncoded(QUrl::FullyEncoded);

        url.append("&artist=" + encodedArtist.toString());
        url.append("&song=" + encodedTrack.toString());
        url.append("&fmt=xml");

        break;
    }
    default: return false;

    }

    qDebug()<< "[lyricwikia service]: "<< url;

    this->array = this->startConnection(url);
    if(this->array.isEmpty()) return false;

    return this->parseArray();
}



bool lyricWikia::parseTrack()
{    
    QString xmlData(this->array);

    QDomDocument doc;

    if (!doc.setContent(xmlData)) return false;

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

    this->extractLyrics(array);
    return true;
}

void lyricWikia::extractLyrics(const QByteArray &array)
{
    QString content = QString::fromUtf8(array.constData());
    content.replace("&lt;", "<");
    QRegExp lyrics_regexp("<lyrics>(.*)</lyrics>");
    lyrics_regexp.indexIn(content);
    QString text;
    QString lyrics = lyrics_regexp.cap(1);
    lyrics = lyrics.trimmed();
    lyrics.replace("\n", "<br>");
    if(!lyrics.contains("PUT LYRICS HERE")&&!lyrics.isEmpty())
    {
        text = "<h2 align='center' >" + this->track[Bae::KEY::TITLE] + "</h2>";
        text += lyrics;

        text= "<div align='center'>"+text+"</div>";
    }

    emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::LYRICS,CONTEXT::LYRIC,text));

}
