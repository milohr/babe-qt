#include "lastfmService.h"

lastfm::lastfm(const Bae::DB &song)
{    
    this->availableInfo.insert(ONTOLOGY::ALBUM, {INFO::ARTWORK, INFO::WIKI, INFO::TAGS});
    this->availableInfo.insert(ONTOLOGY::ARTIST, {INFO::ARTWORK, INFO::WIKI, INFO::TAGS});
    this->availableInfo.insert(ONTOLOGY::TRACK, {INFO::TAGS, INFO::WIKI, INFO::ARTWORK, INFO::METADATA});

    this->track = song;
}

bool lastfm::setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info)
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
        url.append("?method=artist.getinfo");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        break;
    }

    case PULPO::ONTOLOGY::ALBUM:
    {
        QUrl encodedAlbum(this->track[Bae::KEY::ALBUM]);
        encodedAlbum.toEncoded(QUrl::FullyEncoded);

        url.append("?method=album.getinfo");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        url.append("&album=" + encodedAlbum.toString());
        break;
    }

    case PULPO::ONTOLOGY::TRACK:
    {
        QUrl encodedTrack(this->track[Bae::KEY::TITLE]);
        encodedTrack.toEncoded(QUrl::FullyEncoded);

        url.append("?method=track.getinfo");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        url.append("&track=" + encodedTrack.toString());
        url.append("&format=json");

        break;
    }

    default: return false;
    }

    qDebug()<< "[lastfm service]: "<< url;

    this->array = this->startConnection(url);
    if(this->array.isEmpty()) return false;

    return this->parseArray();
}


bool lastfm::parseArtist()
{

    QString xmlData(array);
    QDomDocument doc;

    if (!doc.setContent(xmlData)) return false;



    QStringList artistTags;
    QByteArray artistSimilarArt;
    QStringList artistSimilar;
    QStringList artistStats;

    if (doc.documentElement().toElement().attributes().namedItem("status").nodeValue()!="ok")
        return false;


    const QDomNodeList nodeList = doc.documentElement().namedItem("artist").childNodes();

    for (int i = 0; i < nodeList.count(); i++)
    {
        QDomNode n = nodeList.item(i);

        if (n.isElement())
        {
            //Here retrieve the artist image
            if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
            {
                if(n.nodeName() == "image" && n.hasAttributes())
                {
                    auto imgSize = n.attributes().namedItem("size").nodeValue();

                    if (imgSize == "extralarge" && n.isElement())
                    {
                        auto artistArt_url = n.toElement().text();

                        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::ARTWORK,CONTEXT::IMAGE,startConnection(artistArt_url)));

                        if(this->info == INFO::ARTWORK) return true;
                        else continue;

                    }else if(this->info == INFO::ARTWORK) continue;
                }
            }

            //Here retrieve the artist wiki (bio)
            if(this->info == INFO::WIKI || this->info == INFO::ALL)
            {
                if (n.nodeName() == "bio")
                {
                    auto artistWiki = n.childNodes().item(2).toElement().text();
                    //qDebug()<<"Fetching ArtistWiki LastFm[]";

                    emit this->infoReady(this->track, this->packResponse(ONTOLOGY::ARTIST, INFO::WIKI,CONTEXT::WIKI,artistWiki));

                    if(this->info == INFO::WIKI) return true;
                    else continue;
                }else if(this->info == INFO::WIKI) continue;
            }


            //Here retrieve the artist similar artists
            if(this->info == INFO::TAGS || this->info == INFO::ALL)
            {
                if(n.nodeName() == "similar")
                {
                    auto similarList = n.toElement().childNodes();

                    for(int i=0; i<similarList.count(); i++)
                    {
                        QDomNode m = similarList.item(i);

                        auto artistSimilarName = m.childNodes().item(0).toElement().text();
                        artistSimilar<<artistSimilarName;
                    }

                    emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::TAGS,CONTEXT::ARTIST_SIMILAR,artistSimilar));

                }else if(n.nodeName() == "tags")
                {
                    auto tagsList = n.toElement().childNodes();
                    //qDebug()<<"Fetching ArtistTags LastFm[]";

                    for(int i=0; i<tagsList.count(); i++)
                    {
                        QDomNode m = tagsList.item(i);
                        artistTags<<m.childNodes().item(0).toElement().text();
                    }

                    emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::TAGS,CONTEXT::TAG,artistTags));


                }else if(n.nodeName() == "stats")
                {
                    QVariant stat;
                    auto stats = n.toElement().childNodes();
                    //qDebug()<<"Fetching ArtistTags LastFm[]";

                    for(int i=0; i<stats.count(); i++)
                    {
                        QDomNode m = stats.item(i);
                        artistStats<<m.toElement().text();
                    }

                    emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ARTIST, INFO::TAGS, CONTEXT::ARTIST_STAT,artistStats));

                }else if(this->info == INFO::TAGS) continue;
            }

        }
    }


    /*********NOW WE WANT TO PARSE SIMILAR ARTISTS***********/
    if(this->info == INFO::TAGS || this->info == INFO::ALL)
    {
        auto url = this->API;
        QUrl encodedTrack(this->track[Bae::KEY::TITLE]);
        encodedTrack.toEncoded(QUrl::FullyEncoded);
        QUrl encodedArtist(this->track[Bae::KEY::ARTIST]);
        encodedArtist.toEncoded(QUrl::FullyEncoded);
        url.append("?method=artist.getSimilar");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        url.append("&format=json");


        qDebug()<< "[lastfm service]: "<< url;

        this->array = this->startConnection(url);

        if(!this->array.isEmpty())
            this->parseSimilar();
    }

    return true;
}

bool lastfm::parseAlbum()
{
    QString xmlData(this->array);
    QDomDocument doc;

    if (!doc.setContent(xmlData))
        return false;

    if (doc.documentElement().toElement().attributes().namedItem("status").nodeValue()!="ok")
        return false;

    const auto nodeList = doc.documentElement().namedItem("album").childNodes();

    for (int i = 0; i < nodeList.count(); i++)
    {
        QDomNode n = nodeList.item(i);

        if (n.isElement())
        {
            //Here retrieve the artist image
            if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
            {
                if(n.nodeName() == "image" && n.hasAttributes())
                {
                    auto imgSize = n.attributes().namedItem("size").nodeValue();

                    if (imgSize == "extralarge" && n.isElement())
                    {
                        auto albumArt_url = n.toElement().text();

                        emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ALBUM, INFO::ARTWORK,CONTEXT::IMAGE,startConnection(albumArt_url)));

                        if(this->info == INFO::ARTWORK) return true;
                        else continue;

                    }else continue;

                }else if(this->info == INFO::ARTWORK) continue;
            }

            if(this->info == INFO::WIKI || this->info == INFO::ALL)
            {
                if (n.nodeName() == "wiki")
                {
                    auto albumWiki = n.childNodes().item(1).toElement().text();
                    //qDebug()<<"Fetching AlbumWiki LastFm[]";

                    emit this->infoReady(this->track,this->packResponse(ONTOLOGY::ALBUM, INFO::WIKI,CONTEXT::WIKI,albumWiki));

                    if(this->info == INFO::WIKI) return true;
                    else continue;

                }else if(this->info == INFO::WIKI) continue;
            }

            if(this->info == INFO::TAGS || this->info == INFO::ALL)
            {
                ////qDebug()<<"lastfm[AlbumTags]";

                if (n.nodeName() == "tags")
                {
                    auto tagsList = n.toElement().childNodes();
                    QStringList albumTags;
                    for(int i=0; i<tagsList.count(); i++)
                    {
                        QDomNode m = tagsList.item(i);
                        albumTags<<m.childNodes().item(0).toElement().text();
                    }
                    emit this->infoReady(this->track, this->packResponse(ONTOLOGY::ALBUM, INFO::TAGS,CONTEXT::TAG,albumTags));

                    if(this->info == INFO::TAGS) return true;
                    else continue;

                }else if(this->info == INFO::TAGS) continue;
            }
        }
    }

    return true;
}

bool lastfm::parseTrack()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(this->array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    if (!jsonResponse.isObject())
        return false;

    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemMap = data.value("track").toMap();

    if(itemMap.isEmpty()) return false;

    if(this->info == INFO::TAGS || this->info == INFO::ALL)
    {

        auto listeners = itemMap.value("listeners").toString();
        auto playcount = itemMap.value("playcount").toString();
        QStringList stats = {listeners,playcount};


        QStringList tags;
        for(auto tag : itemMap.value("toptags").toMap().value("tag").toList())
            tags<<tag.toMap().value("name").toString();

        PULPO::VALUE contexts = {{ CONTEXT::TRACK_STAT,stats},{ CONTEXT::TAG,tags}};

        emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::TAGS, contexts));

        if(this->info == INFO::TAGS ) return true;
    }

    if(this->info == INFO::METADATA || this->info == INFO::ALL)
    {
        auto albumTitle = itemMap.value("album").toMap().value("title").toString();
        auto trackNumber = itemMap.value("album").toMap().value("@attr").toMap().value("position").toString();

        emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::METADATA, {{CONTEXT::TRACK_NUMBER,trackNumber},{CONTEXT::ALBUM_TITLE,albumTitle}}));

        if(this->info == INFO::METADATA ) return true;
    }


    if(this->info == INFO::WIKI || this->info == INFO::ALL)
    {
        auto wiki = itemMap.value("wiki").toMap().value("content").toString();
        emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::WIKI, CONTEXT::WIKI,wiki));
        if(!wiki.isEmpty() && this->info == INFO::WIKI) return true;
    }

    if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
    {
        auto images = itemMap.value("album").toMap().value("image").toList();

        for(auto image : images)
            if(image.toMap().value("size").toString()=="extralarge")
            {
                auto artwork = image.toMap().value("#text").toString();
                emit this->infoReady(this->track, this->packResponse(ONTOLOGY::TRACK, INFO::ARTWORK, CONTEXT::IMAGE,this->startConnection(artwork)));
                if(this->info == INFO::ARTWORK) return true;
            }

        if(this->info == INFO::ARTWORK) return false;
    }

    return false;
}

bool lastfm::parseSimilar()
{

    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(this->array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    if (!jsonResponse.isObject())
        return false;

    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();
    auto itemMap = data.value("similarartists").toMap().value("artist");

    if(itemMap.isNull()) return false;

    QList<QVariant> items = itemMap.toList();

    if(items.isEmpty()) return false;


    if(this->info == INFO::TAGS || this->info == INFO::ALL)
    {
        QStringList artistSimilar;

        for(auto item : items)
            artistSimilar<<item.toMap().value("name").toString();

        emit this->infoReady(this->track, this->packResponse(ONTOLOGY::ARTIST, INFO::TAGS, CONTEXT::ARTIST_SIMILAR,artistSimilar));

        if(this->info == INFO::TAGS && !artistSimilar.isEmpty() ) return true;
    }

    return false;
}

bool lastfm::parseTags()
{
    return false;
}
