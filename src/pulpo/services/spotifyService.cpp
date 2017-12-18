#include "spotifyService.h"


spotify::spotify(const BAE::DB &song)
{
    this->track = song;
    this->availableInfo.insert(ONTOLOGY::ALBUM, {INFO::ARTWORK});
    this->availableInfo.insert(ONTOLOGY::ARTIST, {INFO::ARTWORK, INFO::TAGS});
    this->availableInfo.insert(ONTOLOGY::TRACK, {INFO::TAGS, INFO::ARTWORK, INFO::METADATA});
}

bool spotify::setUpService(const ONTOLOGY &ontology, const PULPO::INFO &info)
{
    this->ontology = ontology;
    this->info= info;

    if(!this->availableInfo[this->ontology].contains(this->info))
        return false;

    auto url = this->API;

    QUrl encodedArtist(this->track[BAE::KEY::ARTIST]);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    switch(this->ontology)
    {
    case ONTOLOGY::ARTIST:
    {
        url.append("artist:");
        url.append(encodedArtist.toString());
        url.append("&type=artist&limit=5");
        break;
    }

    case ONTOLOGY::ALBUM:
    {
        QUrl encodedAlbum(this->track[BAE::KEY::ALBUM]);
        encodedAlbum.toEncoded(QUrl::FullyEncoded);

        url.append("album:");
        url.append(encodedAlbum.toString());
        url.append("%20artist:");
        url.append(encodedArtist.toString());
        url.append("&type=album");
        break;
    }

    case ONTOLOGY::TRACK:
    {
        QUrl encodedTrack(this->track[BAE::KEY::TITLE]);
        encodedTrack.toEncoded(QUrl::FullyEncoded);

        url.append("track:");
        url.append(encodedTrack.toString());

        url.append("&type=track&limit=5");
        break;
    }
    default: return false;
    }

    auto credentials = this->CLIENT_ID+":"+this->CLIENT_SECRET;
    auto auth = credentials.toLocal8Bit().toBase64();
    QString header = "Basic " + auth;

    auto request = QNetworkRequest(QUrl("https://accounts.spotify.com/api/token"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", header.toLocal8Bit());

    QNetworkAccessManager manager;
    QNetworkReply *reply =  manager.post(request,"grant_type=client_credentials");

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
            SLOT(quit()));

    loop.exec();

    reply->deleteLater();

    if(reply->error())
    {
        qDebug()<<reply->error();
        return false;
    }

    auto response  = reply->readAll();
    auto data = QJsonDocument::fromJson(response).object().toVariantMap();
    auto token = data["access_token"].toString();

    qDebug()<< "[spotify service]: "<< url;

    this->array = this->startConnection(url, {{"Authorization", "Bearer "+token}});
    if(this->array.isEmpty()) return false;

    return this->parseArray();
}

bool spotify::parseArtist()
{    
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    if (!jsonResponse.isObject()) return false;

    auto data = jsonResponse.object().toVariantMap();
    auto itemMap = data.value("artists").toMap().value("items");

    if(itemMap.isNull()) return false;
    QList<QVariant> items = itemMap.toList();

    if(items.isEmpty())  return false;

    auto root = items.first().toMap();

    if(this->info == INFO::TAGS || this->info == INFO::ALL)
    {
        VALUE tags;

        QStringList stats;
        stats << root.value("popularity").toString();
        stats << root.value("followers").toMap().value("total").toString();

        tags.insert(CONTEXT::ARTIST_STAT, stats);

        auto genres = root.value("genres").toStringList();
        tags.insert(CONTEXT::GENRE, genres);

        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::TAGS, tags));

        if(this->info == INFO::TAGS ) return true;
    }

    if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
    {
        auto artwork = root.value("images").toList().first().toMap().value("url").toString();
        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::ARTWORK, CONTEXT::IMAGE,this->startConnection(artwork)));

        if(this->info == INFO::ARTWORK && !artwork.isEmpty() ) return true;
    }

    return false;
}

bool spotify::parseAlbum()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    if (!jsonResponse.isObject()) return false;

    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemMap = data.value("albums").toMap().value("items");

    if(itemMap.isNull()) return false;
    QList<QVariant> items = itemMap.toList();

    if(items.isEmpty())  return false;

    if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
    {
        auto albumArt =items.first().toMap().value("images").toList().first().toMap().value("url").toString();
        emit this->infoReady(this->track, this->packResponse(ONTOLOGY::ALBUM, INFO::ARTWORK,CONTEXT::IMAGE, startConnection(albumArt)));
        qDebug()<<"parseSpotifyAlbum ["<< albumArt<<"]";

        if(this->info == INFO::ARTWORK && !albumArt.isEmpty() ) return true;
    }

    return false;
}

bool spotify::parseTrack()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(this->array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;
    if (!jsonResponse.isObject())
        return false;


    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemMap = data.value("tracks").toMap().value("items");

    if(itemMap.isNull()) return false;

    QList<QVariant> items = itemMap.toList();

    if(items.isEmpty()) return false;
    //get album title
    for(auto item : items )
    {
        auto album = item.toMap().value("album").toMap();
        auto trackArtist =  album.value("artists").toList().first().toMap().value("name").toString();

        if(trackArtist.contains(this->track[BAE::KEY::ARTIST]))
        {
            if(this->info == INFO::TAGS || this->info == INFO::ALL)
            {
                auto popularity = item.toMap().value("popularity").toString();
                emit this->infoReady(this->track,this->packResponse(ONTOLOGY::TRACK, INFO::TAGS, CONTEXT::TRACK_STAT,popularity));

                if(this->info == INFO::TAGS ) return true;
            }

            if(this->info == INFO::METADATA || this->info == INFO::ALL)
            {
                auto trackAlbum = album.value("name").toString();
                emit this->infoReady(this->track,this->packResponse(ONTOLOGY::TRACK, INFO::METADATA, CONTEXT::ALBUM_TITLE,trackAlbum));

                auto trackPosition = item.toMap().value("track_number").toString();
                emit this->infoReady(this->track,this->packResponse(ONTOLOGY::TRACK, INFO::METADATA, CONTEXT::TRACK_NUMBER,trackPosition));

                if(this->info == INFO::METADATA ) return true;
            }

            if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
            {
                auto artwork = album.value("images").toList().first().toMap().value("url").toString();
                emit this->infoReady(this->track,this->packResponse(ONTOLOGY::TRACK, INFO::ARTWORK,CONTEXT::IMAGE,this->startConnection(artwork)));
                if(!artwork.isEmpty() && this->info == INFO::ARTWORK ) return true;
            }

        }else continue;
    }

    return false;
}


