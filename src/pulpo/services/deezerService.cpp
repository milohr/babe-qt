#include "deezerService.h"

deezer::deezer(const BAE::DB &song)
{
    this->availableInfo.insert(ONTOLOGY::ARTIST, {INFO::ARTWORK});
    this->availableInfo.insert(ONTOLOGY::ALBUM, {INFO::ARTWORK});
    this->availableInfo.insert(ONTOLOGY::TRACK, {INFO::TAGS, INFO::ARTWORK, INFO::METADATA});

    this->track = song;
}

bool deezer::setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info)
{

    qDebug()<<"setting up genius service";
    this->ontology = ontology;
    this->info = info;

    if(!this->availableInfo[this->ontology].contains(this->info))
        return false;

    auto url = this->API;

    QUrl encodedArtist(this->track[BAE::KEY::ARTIST]);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    QUrl encodedTrack(this->track[BAE::KEY::TITLE]);
    encodedTrack.toEncoded(QUrl::FullyEncoded);

    QUrl encodedAlbum(this->track[BAE::KEY::ALBUM]);
    encodedAlbum.toEncoded(QUrl::FullyEncoded);

    switch(this->ontology)
    {
        case PULPO::ONTOLOGY::ARTIST:
        {
            url.append("artist:\"");
            url.append(encodedArtist.toString());
            url.append("\"");

            break;
        }

        case PULPO::ONTOLOGY::TRACK:
        {
            url.append("artist:\"");
            url.append(encodedArtist.toString());
            url.append("\" ");

            url.append("track:\"");
            url.append(encodedTrack.toString());
            url.append("\"");
            break;
        }

        case PULPO::ONTOLOGY::ALBUM:
        {
            url.append("artist:\"");
            url.append(encodedArtist.toString());
            url.append("\" ");

            url.append("album:\"");
            url.append(encodedAlbum.toString());
            url.append("\"");
            break;
        }
        default: return false;
    }

    url.append("&limit=10");
    qDebug()<<"DEEZER API"<< url;
    this->array = this->startConnection(url);
    if(this->array.isEmpty()) return false;

    return this->parseArray();
}

bool deezer::parseArtist()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(this->array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;
    if (!jsonResponse.isObject())
        return false;

    qDebug()<<"parsing artist deezer";

    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemList = data.value("data").toList();

    if(itemList.isEmpty()) return false;

    for(auto item : itemList)
    {
        auto artistMap = item.toMap().value("artist").toMap();

        if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
        {
            if(artistMap.value("name").toString() == this->track[BAE::KEY::ARTIST])
            {
                auto artwork = artistMap.value("picture_big").toString();
                emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::ARTWORK, CONTEXT::IMAGE, this->startConnection(artwork)));
                if(artwork.isEmpty() && this->info == INFO::ARTWORK ) return false;

                return true;

            }else continue;
        }
    }
    return false;
}


bool deezer::parseTrack()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(this->array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;
    if (!jsonResponse.isObject())
        return false;

    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemList = data.value("data").toList();

    if(itemList.isEmpty()) return false;

    for(auto item : itemList)
    {
        if(item.toMap().value("artist").toMap().value("name").toString() == this->track[BAE::KEY::ARTIST]
                && item.toMap().value("title").toString() == this->track[BAE::KEY::TITLE])
        {
            auto albumMap = item.toMap().value("album").toMap();

            if(this->info == INFO::TAGS || this->info == INFO::ALL)
            {
                auto rank = item.toMap().value("rank").toString();
                emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::TAGS, CONTEXT::TRACK_STAT, rank));

                if(this->info == INFO::TAGS) return true;
            }

            if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
            {
                auto image = albumMap.value("cover_big").toString();

                emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::ARTWORK, CONTEXT::IMAGE, this->startConnection(image)));
                if(image.isEmpty() && this->info == INFO::ARTWORK) return false;
            }

            if(this->info == INFO::METADATA || this->info == INFO::ALL)
            {
                auto albumMap = item.toMap().value("album").toMap();
                auto albumTitle = albumMap.value("title").toString();

                emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::METADATA, CONTEXT::ALBUM_TITLE, albumTitle));

                if(albumTitle.isEmpty() && this->info == INFO::ARTWORK) return false;
            }

            return true;

        }else continue;
    }

    return false;
}

bool deezer::parseAlbum()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    if (!jsonResponse.isObject()) return false;

    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemList = data.value("data").toList();

    if(itemList.isEmpty()) return false;

    for(auto item : itemList)
    {
        if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
        {
            auto albumMap = item.toMap().value("album").toMap();
            auto artistMap = item.toMap().value("artist").toMap();
            auto album = albumMap.value("title").toString();
            auto artist = artistMap.value("name").toString();

            if(album == this->track[BAE::KEY::ALBUM] && artist == this->track[BAE::KEY::ARTIST])
            {
                auto albumArt = albumMap.value("cover_big").toString();
                emit this->infoReady(this->track, this->packResponse(ONTOLOGY::ALBUM, INFO::ARTWORK,CONTEXT::IMAGE, startConnection(albumArt)));

                if(this->info == INFO::ARTWORK && albumArt.isEmpty() ) return false;

                return true;

            }else continue;
        }
    }

    return false;
}
