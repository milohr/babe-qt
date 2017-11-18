#include "geniusService.h"
#include <QObject>
#include "../htmlparser.h"

genius::genius(const Bae::DB &song)
{
    this->availableInfo.insert(ONTOLOGY::ARTIST, {INFO::ARTWORK, INFO::WIKI, INFO::TAGS});
    this->availableInfo.insert(ONTOLOGY::TRACK, {INFO::TAGS, INFO::WIKI, INFO::ARTWORK, INFO::LYRICS});

    this->track = song;
}

bool genius::setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info)
{

    qDebug()<<"setting up genius service";
    this->ontology = ontology;
    this->info = info;

    if(!this->availableInfo[this->ontology].contains(this->info))
        return false;

    auto url = this->API;

    QUrl encodedArtist(this->track[Bae::KEY::ARTIST]);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    QUrl encodedTrack(this->track[Bae::KEY::TITLE]);
    encodedTrack.toEncoded(QUrl::FullyEncoded);

    switch(this->ontology)
    {
    case PULPO::ONTOLOGY::ARTIST:
    {
        url.append("/search?q=");
        url.append(encodedArtist.toString());
        break;
    }

    case PULPO::ONTOLOGY::TRACK:
    {
        url.append("/search?q=");
        url.append(encodedTrack.toString());
        url.append(" " + encodedArtist.toString());
        break;
    }

    default: return false;
    }

    qDebug()<< "[genius service]: "<< url;

    auto newUrl = this->getID(url);

    qDebug()<< "[genius service]: "<< newUrl;

    this->array = this->startConnection( newUrl,{{"Authorization", this->KEY}} );
    if(this->array.isEmpty()) return false;

    return this->parseArray();
}

QString genius::getID(const QString &url)
{
    QString id;

    auto new_array =  this->startConnection(url, {{"Authorization", this->KEY}} );

    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(new_array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return id;

    if (!jsonResponse.isObject())
        return id;

    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto hits = data.value("response").toMap().value("hits").toList();

    if(hits.isEmpty()) return id;

    switch(this->ontology)
    {

    case ONTOLOGY::ARTIST:
    {
        id = hits.first().toMap().value("result").toMap().value("primary_artist").toMap().value("api_path").toString();
        break;
    }
    case ONTOLOGY::TRACK:
    {
        id = hits.first().toMap().value("result").toMap().value("api_path").toString();
        break;
    }
    default: break;
    }

    return !id.isEmpty()? this->API+id :  id;

}


bool genius::parseArtist()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(this->array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;
    if (!jsonResponse.isObject())
        return false;

    qDebug()<<"parsing artist info1";

    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemMap = data.value("response").toMap().value("artist").toMap();
    qDebug()<<"parsing artist info2";

    if(itemMap.isEmpty()) return false;
    VALUE contexts;
    qDebug()<<"parsing artist info3";

    if(this->info == INFO::TAGS || this->info == INFO::ALL)
    {
        auto alias = itemMap.value("alternate_names").toStringList();
        contexts.insert(CONTEXT::ARTIST_ALIAS, alias);

        auto followers = itemMap.value("followers_count").toString();
        contexts.insert(CONTEXT::ARTIST_STAT, followers );

        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::TAGS, contexts));

        if(this->info == INFO::TAGS ) return true;
    }

    if(this->info == INFO::WIKI || this->info == INFO::WIKI)
    {
        QString wikiData;
        for(auto wiki : itemMap.value("description").toMap().value("dom").toMap().value("children").toList())
        {
            for(auto child : wiki.toMap().value("children").toStringList() )
                wikiData = wikiData + child;
        }

        contexts.insert( CONTEXT::WIKI, wikiData);
        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::WIKI, contexts));

        if(!wikiData.isEmpty() && this->info == INFO::WIKI) return true;
    }

    if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
    {
        auto artwork = itemMap.value("image_url").toString();
        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::ARTWORK,CONTEXT::IMAGE,this->startConnection(artwork)));
        if(!artwork.isEmpty() && this->info == INFO::ARTWORK ) return true;
    }


    return false;
}

bool genius::parseTrack()
{

    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(this->array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;
    if (!jsonResponse.isObject())
        return false;


    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemMap = data.value("response").toMap().value("song").toMap();

    if(itemMap.isEmpty()) return false;

    auto albumMap = itemMap.value("album").toMap();

    if(!albumMap.isEmpty())
    {
        auto id = albumMap.value("api_path").toString();
        qDebug()<<"TRACK ALBUM"<<this->API+id;
        auto album_array = this->startConnection(this->API+id, {{"Authorization", this->KEY}});
        this->getAlbumInfo(album_array);
    }

    if(this->info == INFO::TAGS || this->info == INFO::ALL)
    {
        VALUE contexts;
        QStringList team;

        auto performances = itemMap.value("custom_performances").toList();
        for(auto performance : performances)
            for(auto artist : performance.toMap().value("artists").toList())
                team<< artist.toMap().value("name").toString();

        qDebug()<<this->track[Bae::KEY::TITLE]<<"CUSTOM PERFORMANCES:"<<team;

        for(auto feature : itemMap.value("featured_artists").toList())
            team<<feature.toMap().value("name").toString();

        for(auto producer : itemMap.value("producer_artists").toList())
            team<<producer.toMap().value("name").toString();

        for(auto producer : itemMap.value("writer_artists").toList())
            team<<producer.toMap().value("name").toString();

        contexts.insert(CONTEXT::TRACK_TEAM, team);

        emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::TAGS, contexts));

        if(this->info == INFO::TAGS) return true;
    }

    if(this->info == INFO::WIKI || this->info == INFO::WIKI)
    {
        QString wikiData;
        for(auto wiki : itemMap.value("description").toMap().value("dom").toMap().value("children").toList())
            for(auto child : wiki.toMap().value("children").toStringList() )
                wikiData = wikiData + child;


        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::TRACK, INFO::WIKI, CONTEXT::WIKI, wikiData));

        if(!wikiData.isEmpty() && this->info == INFO::WIKI) return true;
    }

    if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
    {
        auto image = itemMap.value("header_image_url").toString();

        emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::ARTWORK, CONTEXT::IMAGE,this->startConnection(image)));

        if(this->info == INFO::ARTWORK) return false;
    }

    if(this->info == INFO::LYRICS || this->info == INFO::ALL)
    {
        auto lyricsPath = itemMap.value("path").toString();

        auto path = "https://genius.com"+lyricsPath;
        qDebug()<<"LYRICS PATH"<<path;
        auto lyricsArray = this->startConnection(path);

        if(!lyricsArray.isEmpty()) this->extractLyrics(lyricsArray);

        if(this->info == INFO::LYRICS ) return true;
    }

    return false;
}

bool genius::getAlbumInfo(const QByteArray &array)
{

    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;
    if (!jsonResponse.isObject())
        return false;


    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemMap = data.value("response").toMap().value("album").toMap();

    if(itemMap.isEmpty()) return false;


    if(this->info == INFO::TAGS || this->info == INFO::ALL)
    {
        VALUE tags;

        auto date = itemMap.value("release_date").toString();
        tags.insert(CONTEXT::ALBUM_DATE, date);

        auto views = itemMap.value("song_pageviews").toString();
        tags.insert(CONTEXT::ALBUM_STAT, views );

        QStringList team;
        for(auto name : itemMap.value("song_performances").toList())
            for(auto artist : name.toMap().value("artists").toList())
                team<<artist.toMap().value("name").toString();

        tags.insert(CONTEXT::ALBUM_TEAM, team);

        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ALBUM, INFO::TAGS, tags));

        if(this->info == INFO::TAGS ) return true;
    }


    if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
    {
        auto artwork = itemMap.value("cover_art_url").toString();
        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::TRACK, INFO::ARTWORK, CONTEXT::IMAGE,this->startConnection(artwork)));
        if(!artwork.isEmpty() && this->info == INFO::ARTWORK ) return true;
    }



    return false;
}

void genius::extractLyrics(const QByteArray &array)
{
    QString lyrics;
    htmlParser parser;
    parser.setHtml(array);

    auto lyricsList = parser.parseTag("div", "class=\"lyrics\"");
    qDebug()<<lyricsList;

    if(!lyricsList.isEmpty())
        lyrics=lyricsList.first();

    //    QString content = QString::fromUtf8(array.constData());
    //    //    content.replace("&lt;", "<");
    //    QRegExp lyrics_regexp("<div class=\"lyrics\">(.*)</div>");
    //    lyrics_regexp.indexIn(content);
    QString text;
    //    QString lyrics = lyrics_regexp.cap(1);
    //    qDebug()<<lyrics;
    //    lyrics = lyrics.trimmed();
    //    lyrics.replace("\n", "<br>");

    if(!lyrics.isEmpty())
    {
        text = "<h2 align='center' >" + this->track[Bae::KEY::TITLE] + "</h2>";
        text += lyrics;

        text= "<div align='center'>"+text+"</div>";
    }

    emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::LYRICS,CONTEXT::LYRIC,text));
}


