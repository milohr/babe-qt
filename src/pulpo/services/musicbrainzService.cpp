#include "musicbrainzService.h"


/*
 * http://musicbrainz.org/ws/2/recording/?query=artist:lana%20del%20rey+recording:ultraviolence
 * http://google.com/complete/search?output=toolbar&q=lana
 * */

musicBrainz::musicBrainz(const DB &song)
{
    this->availableInfo.insert(ONTOLOGY::ALBUM, {INFO::TAGS});
    this->availableInfo.insert(ONTOLOGY::ARTIST, {INFO::TAGS});
    this->availableInfo.insert(ONTOLOGY::TRACK, {INFO::TAGS});

    this->track = song;
}

bool musicBrainz::setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info)
{
    this->ontology = ontology;
    this->info = info;

    if(!this->availableInfo[this->ontology].contains(this->info))
        return false;

    auto url = this->API;
    QUrl encodedArtist(this->track[Bae::KEY::ARTIST]);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    switch(this->ontology)
    {
    case PULPO::ONTOLOGY::ARTIST:
    {
        url.append(QString("artist/?query=artist:\"" + encodedArtist.toString()+ "\"")) ;
        url.append("&fmt=json");
        url.append("&limit=5");
        break;
    }

    case PULPO::ONTOLOGY::ALBUM:
    {
        QUrl encodedAlbum(this->track[Bae::KEY::ALBUM]);
        encodedAlbum.toEncoded(QUrl::FullyEncoded);

        url.append(QString("release/?query=release:\"" + encodedAlbum.toString()+ "\"")) ;
        url.append(QString("+artist:\"" + encodedArtist.toString()+ "\"")) ;
        url.append(QString("+primarytype:\"album\"&fmt=json&limit=5"));
        break;
    }

    case PULPO::ONTOLOGY::TRACK:
    {
        QUrl encodedTrack(this->track[Bae::KEY::TITLE]);
        encodedTrack.toEncoded(QUrl::FullyEncoded);
        url.append(QString("recording/?query=artist:\"" + encodedArtist.toString() + "\""));
        url.append(QString("+recording:\"" + encodedTrack.toString()+ "\""));
        url.append("&fmt=json");
        url.append("&limit=5");

        break;
    }

    default: return false;
    }

    qDebug()<< "[musicbrainz service]: "<< url;

    this->array = this->startConnection(url, this->header);
    if(this->array.isEmpty()) return false;

    return this->parseArray();
}

bool musicBrainz::parseArtist()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    if (!jsonResponse.isObject()) return false;

    auto data = jsonResponse.object().toVariantMap();

    if(data.isEmpty()) return false;

    QList<QVariant> items = data.value("artists").toList();

    if(items.isEmpty())  return false;

    for(auto item : items)
    {
        auto itemMap = item.toMap();

        if(this->info == INFO::TAGS || this->info == INFO::ALL)
        {
            PULPO::VALUE contexts;

            auto score = itemMap.value("score").toString();
            contexts.insert(CONTEXT::STAT,score);

            auto alias = itemMap.value("sort-name").toString();
            contexts.insert(CONTEXT::ARTIST_ALIAS,alias);

            auto gender = itemMap.value("gender").toString();
            contexts.insert(CONTEXT::ARTIST_GENDER,gender);

            auto country = itemMap.value("country").toString();
            contexts.insert(CONTEXT::ARTIST_PLACE,country);

            auto area = itemMap.value("area").toMap().value("name").toString();
            contexts.insert(CONTEXT::ARTIST_PLACE,area);

            auto subarea = itemMap.value("begin-area").toMap().value("name").toString();
            contexts.insert(CONTEXT::ARTIST_PLACE,subarea);

            auto lifeSpan_begin = itemMap.value("life-span").toMap().value("begin").toString();
            auto lifeSpan_end = itemMap.value("life-span").toMap().value("ended").toString();
            contexts.insert(CONTEXT::ARTIST_DATE, QStringList{lifeSpan_begin,lifeSpan_end});

            QStringList aliases;
            for(auto alias : itemMap.value("aliases").toList())
                aliases << alias.toMap().value("name").toString();

            contexts.insert(CONTEXT::ARTIST_ALIAS,aliases);

            QStringList tags;
            for(auto tag : itemMap.value("tags").toList())
                tags << tag.toMap().value("name").toString();

            contexts.insert(CONTEXT::TAG,tags);

            emit this->infoReady(this->track, this->packResponse(ONTOLOGY::ARTIST, INFO::TAGS,contexts));
            if(this->info == INFO::TAGS  && !contexts.isEmpty()) return true;
        }

    }

    return false;
}

bool musicBrainz::parseAlbum()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    if (!jsonResponse.isObject()) return false;

    auto data = jsonResponse.object().toVariantMap();

    if(data.isEmpty()) return false;

    QList<QVariant> items = data.value("releases").toList();

    if(items.isEmpty())  return false;

    for(auto item : items)
    {
        auto itemMap = item.toMap();

        if(this->info == INFO::TAGS || this->info == INFO::ALL)
        {
            PULPO::VALUE contexts;

            auto score = itemMap.value("score").toString();
            contexts.insert(CONTEXT::STAT,score);

            auto date = itemMap.value("date").toString();
            contexts.insert(CONTEXT::ALBUM_DATE,date);

            QStringList labels;
            for(auto label : itemMap.value("label-info").toList())
                labels << label.toMap().value("label").toMap().value("name").toString();

            contexts.insert(CONTEXT::ALBUM_LABEL,labels);

            emit this->infoReady(this->track, this->packResponse(ONTOLOGY::ALBUM, INFO::TAGS,contexts));
            if(this->info == INFO::TAGS  && !contexts.isEmpty()) return true;
        }

    }

    return false;
}

bool musicBrainz::parseTrack()
{

    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    if (!jsonResponse.isObject()) return false;

    auto data = jsonResponse.object().toVariantMap();

    if(data.isEmpty()) return false;
    QList<QVariant> items = data.value("recordings").toList();

    if(items.isEmpty())  return false;

    auto id = items.first().toMap().value("id").toString();
    auto url =QString("http://musicbrainz.org/ws/2/recording/%1?inc=artist-rels+recording-rels&fmt=json&limit=5").arg(id);
    qDebug()<<"NEW TRACK INFO URL MUSICBRAINZ"<<url;
    auto rel = this->startConnection(url, this->header);

    if(rel.isEmpty()) return false;

    jsonResponse = QJsonDocument::fromJson(static_cast<QString>(rel).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    if (!jsonResponse.isObject()) return false;

    data = jsonResponse.object().toVariantMap();

    if(data.isEmpty()) return false;


    if(this->info == INFO::TAGS || this->info == INFO::ALL)
    {
        items = data.value("relations").toList();

        if(items.isEmpty())  return false;

        QStringList people;

        for(auto item : items)
        {
            auto team = item.toMap().value("artist").toMap().value("name").toString();
            people<<team;
        }

        emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::TAGS,CONTEXT::TRACK_TEAM, people));
        if(this->info == INFO::TAGS  && !people.isEmpty()) return true;
    }


    return false;
}
