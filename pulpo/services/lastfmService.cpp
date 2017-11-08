#include "lastfmService.h"

lastfm::lastfm(const Bae::DB &song):track(song){}


bool lastfm::setUpService(const Pulpo::ONTOLOGY &type)
{
    //    qDebug()<<"Setting up lastfm service";
    this->ontology = type;

    auto url = this->API;

    QUrl encodedArtist(this->track[Bae::DBCols::ARTIST]);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    switch(this->ontology)
    {
    case Pulpo::ONTOLOGY::ARTIST:
    {
        url.append("?method=artist.getinfo");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        break;
    }

    case Pulpo::ONTOLOGY::ALBUM:
    {
        QUrl encodedAlbum(this->track[Bae::DBCols::ALBUM]);
        encodedAlbum.toEncoded(QUrl::FullyEncoded);

        url.append("?method=album.getinfo");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        url.append("&album=" + encodedAlbum.toString());
        break;
    }

    case Pulpo::ONTOLOGY::TRACK:
    {
        QUrl encodedTrack(this->track[Bae::DBCols::TITLE]);
        encodedTrack.toEncoded(QUrl::FullyEncoded);

        url.append("?method=track.getinfo");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        url.append("&track=" + encodedTrack.toString());
        break;
    }
    default: return false;
    }

    //    qDebug()<< "lastfm url: "<< url;

    this->array = this->startConnection(url);
    return !this->array.isEmpty();
}

bool lastfm::parseArray(const INFO &infoType)
{

    if(this->ontology != Pulpo::ONTOLOGY::NONE)
    {
        switch(this->ontology)
        {
        case Pulpo::ONTOLOGY::ALBUM: this->parseAlbum(infoType); break;
        case Pulpo::ONTOLOGY::ARTIST: this->parseArtist(infoType); break;
        case Pulpo::ONTOLOGY::TRACK: this->parseTrack(infoType); break;
        default: return false;
        }
    }

    return false;
}



bool lastfm::parseArtist(const INFO &infoType)
{
    QString xmlData(array);
    QDomDocument doc;

    if (!doc.setContent(xmlData)) return false;


    QStringList artistTags;
    QByteArray artistSimilarArt;
    QMap<QString,QVariant> artistSimilar;

    if (doc.documentElement().toElement().attributes().namedItem("status").nodeValue()!="ok")
        return false;

    const QDomNodeList nodeList = doc.documentElement().namedItem("artist").childNodes();

    for (int i = 0; i < nodeList.count(); i++)
    {
        QDomNode n = nodeList.item(i);

        if (n.isElement())
        {
            //Here retrieve the artist image
            if(infoType == INFO::ARTWORK || infoType == INFO::ALL)
            {
                if(n.nodeName() == "image" && n.hasAttributes())
                {
                    auto imgSize = n.attributes().namedItem("size").nodeValue();

                    if (imgSize == "extralarge" && n.isElement())
                    {
                        auto artistArt_url = n.toElement().text();
                        //qDebug()<<"Fetching ArtistArt LastFm["<<artistArt_url<<"]";
                        emit this->infoReady(this->track,{{INFO::ARTWORK, startConnection(artistArt_url)}});

                        if(infoType == INFO::ARTWORK) return true;
                        else continue;

                    }else if(infoType == INFO::ARTWORK) continue;
                }
            }

            //Here retrieve the artist wiki (bio)
            if(infoType == INFO::WIKI || infoType == INFO::ALL)
            {
                if (n.nodeName() == "bio")
                {
                    auto artistWiki = n.childNodes().item(2).toElement().text();
                    //qDebug()<<"Fetching ArtistWiki LastFm[]";

                    emit this->infoReady(this->track, {{INFO::WIKI,artistWiki}});

                    if(infoType == INFO::WIKI) return true;
                    else continue;
                }else if(infoType == INFO::WIKI) continue;
            }


            //Here retrieve the artist similar artists
            if(infoType == INFO::SIMILAR || infoType == INFO::ALL)
            {
                if(n.nodeName() == "similar")
                {
                    auto similarList = n.toElement().childNodes();
                    //qDebug()<<"Fetching ArtistSimilar LastFm[]";

                    for(int i=0; i<similarList.count(); i++)
                    {
                        QDomNode m = similarList.item(i);

                        auto artistSimilarName = m.childNodes().item(0).toElement().text();
                        auto artist = m.toElement().childNodes();

                        for(int j=0; j<artist.count(); j++)
                        {
                            QDomNode k = artist.item(j);

                            if(k.nodeName() == "image" && k.hasAttributes())
                            {
                                QString imgSize = k.attributes().namedItem("size").nodeValue();

                                if (imgSize == "extralarge")
                                    if (k.isElement())
                                    {
                                        auto artistSimilarArt_url = k.toElement().text();
                                        artistSimilarArt = startConnection(artistSimilarArt_url);
                                    }
                            }
                        }
                        artistSimilar.insert(artistSimilarName,artistSimilarArt);
                    }

                    emit this->infoReady(this->track,{{INFO::SIMILAR, artistSimilar}});

                    if(infoType == INFO::SIMILAR) return true;
                    else continue;
                }else if(infoType == INFO::SIMILAR) continue;
            }

            if(infoType == INFO::TAGS || infoType == INFO::ALL)
            {
                if(n.nodeName() == "tags")
                {
                    auto tagsList = n.toElement().childNodes();
                    //qDebug()<<"Fetching ArtistTags LastFm[]";

                    for(int i=0; i<tagsList.count(); i++)
                    {
                        QDomNode m = tagsList.item(i);
                        artistTags<<m.childNodes().item(0).toElement().text();
                    }

                    emit this->infoReady(this->track,{{INFO::TAGS, artistTags}});
                    if(infoType == INFO::TAGS) return true;
                }else if(infoType == INFO::TAGS) continue;

            }
        }
    }


    return true;
}

bool lastfm::parseAlbum(const INFO &infoType)
{
    QString xmlData(this->array);
    QDomDocument doc;

    if (!doc.setContent(xmlData))
    {
        qDebug() << "The XML obtained from last.fm is invalid.";
        return false;
    }


    if (doc.documentElement().toElement().attributes().namedItem("status").nodeValue()!="ok") return false;


    const QDomNodeList nodeList = doc.documentElement().namedItem("album").childNodes();

    for (int i = 0; i < nodeList.count(); i++)
    {
        QDomNode n = nodeList.item(i);

        if (n.isElement())
        {
            //Here retrieve the artist image
            if(infoType == INFO::ARTWORK || infoType == INFO::ALL)
            {
                ////qDebug()<<"lastfm[AlbumArt]";
                if(n.nodeName() == "image" && n.hasAttributes())
                {
                    auto imgSize = n.attributes().namedItem("size").nodeValue();

                    if (imgSize == "extralarge" && n.isElement())
                    {
                        auto albumArt_url = n.toElement().text();
                        //qDebug()<<"Fetching AlbumArt LastFm[]";

                        emit this->infoReady(this->track,{{INFO::ARTWORK,startConnection(albumArt_url)}});

                        if(infoType == INFO::ARTWORK) return true;
                        else continue;

                    }else continue;

                }else if(infoType == INFO::ARTWORK) continue;
            }

            if(infoType == INFO::WIKI || infoType == INFO::ALL)
            {
                if (n.nodeName() == "wiki")
                {
                    auto albumWiki = n.childNodes().item(1).toElement().text();
                    //qDebug()<<"Fetching AlbumWiki LastFm[]";

                    emit this->infoReady(this->track,{{INFO::WIKI, albumWiki}});

                    if(infoType == INFO::WIKI) return true;
                    else continue;

                }else if(infoType == INFO::WIKI) continue;
            }

            if(infoType == INFO::TAGS || infoType == INFO::ALL)
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
                    emit this->infoReady(this->track,{{INFO::TAGS, albumTags}});
                    if(infoType == INFO::TAGS) return true;
                    else continue;

                }else if(infoType == INFO::TAGS) continue;
            }
        }
    }

    return true;
}

bool lastfm::parseTrack(const INFO &infoType)
{
    //qDebug()<< "parseLastFmTrack";
    QString xmlData(array);
    QDomDocument doc;

    if (!doc.setContent(xmlData))
    {
        //qDebug() << "The XML obtained from last.fm is invalid.";
        return false;

    }else
    {
        if (doc.documentElement().toElement().attributes().namedItem("status").nodeValue()!="ok")
            return false;

        const QDomNodeList nodeList = doc.documentElement().namedItem("track").childNodes();

        for (int i = 0; i < nodeList.count(); i++)
        {
            QDomNode n = nodeList.item(i);

            if (n.isElement())
            {

                if(infoType == INFO::TRACK || infoType == INFO::ALL)
                {
                    if(n.nodeName() == "album" && n.hasAttributes())
                    {
                        //qDebug()<<"Fetching TrackPosition LastFm[]";

                        int position = n.attributes().namedItem("position").toElement().text().toInt();
                        emit trackPositionReady(position,this->track);
                        if(infoType == INFO::TRACK) return true;

                    }else if(infoType == INFO::TRACK) continue;

                }

                if(infoType == INFO::ALBUM || infoType == INFO::ALL)
                {
                    ////qDebug()<<"lastfm[AlbumArt]";
                    if(n.nodeName() == "album")
                    {
                        //qDebug()<<"Fetching TrackAlbum LastFm[]";

                        auto trackAlbum = n.namedItem("title").toElement().text();
                        emit trackAlbumReady(trackAlbum,this->track);
                        if(infoType == INFO::ALBUM) return true;
                        else continue;

                    }else if(infoType == INFO::ALBUM) continue;
                }

                if(infoType == INFO::WIKI || infoType == INFO::ALL)
                {

                    if (n.nodeName() == "wiki")
                    {
                        //qDebug()<<"Fetching TrackWiki LastFm[]";

                        auto trackWiki = n.namedItem("summary").toElement().text();
                        //qDebug()<<trackWiki;
                        emit trackWikiReady(trackWiki,this->track);
                        if(infoType == INFO::WIKI) return true;
                        else continue;

                    }else if(infoType == INFO::WIKI) continue;

                }

                if(infoType == INFO::TAGS || infoType == INFO::ALL)
                {
                    if (n.nodeName() == "toptags")
                    {

                        //qDebug()<<"Fetching TrackTags LastFm[]";
                        auto tagsList = n.toElement().childNodes();
                        QStringList trackTags;
                        for(int i=0; i<tagsList.count(); i++)
                            trackTags<<tagsList.item(i).namedItem("name").toElement().text();


                        emit trackTagsReady(trackTags,this->track);
                        if(infoType == INFO::TAGS) return true;
                        else continue;
                    }else if(infoType == INFO::TAGS) continue;
                }

            }
        }
    }

    return true;
}

QVariant lastfm::getTrackInfo(const QByteArray &array, const INFO &infoType)
{
    QString xmlData(array);
    QDomDocument doc;

    if (!doc.setContent(xmlData))
    {
        //qDebug() << "The XML obtained from last.fm is invalid.";
        return QVariant();

    }else
    {
        if (doc.documentElement().toElement().attributes().namedItem("status").nodeValue()!="ok")
            return QVariant();

        const QDomNodeList nodeList = doc.documentElement().namedItem("track").childNodes();

        for (int i = 0; i < nodeList.count(); i++)
        {
            QDomNode n = nodeList.item(i);

            if (n.isElement())
            {

                if(infoType == INFO::TRACK || infoType == INFO::ALL)
                {
                    if(n.nodeName() == "album" && n.hasAttributes())
                    {
                        int position = n.attributes().namedItem("position").toElement().text().toInt();
                        return QVariant(position);

                    }else return QVariant(0);

                }

                if(infoType == INFO::ALBUM || infoType == INFO::ALL)
                {
                    ////qDebug()<<"lastfm[AlbumArt]";
                    if(n.nodeName() == "album")
                    {
                        auto trackAlbum = n.namedItem("title").toElement().text();
                        return QVariant(trackAlbum);

                    }else if(infoType == INFO::ALBUM) continue;
                }

                if(infoType == INFO::WIKI || infoType == INFO::ALL)
                {

                    if (n.nodeName() == "wiki")
                    {
                        auto trackWiki = n.namedItem("summary").toElement().text();
                        //qDebug()<<trackWiki;
                        return QVariant(trackWiki);

                    }else if(infoType == INFO::WIKI) continue;

                }

                if(infoType == INFO::TAGS || infoType == INFO::ALL)
                {
                    if (n.nodeName() == "toptags")
                    {
                        //qDebug()<<"found toptags";
                        auto tagsList = n.toElement().childNodes();
                        QStringList trackTags;
                        for(int i=0; i<tagsList.count(); i++)
                            trackTags<<tagsList.item(i).namedItem("name").toElement().text();


                        return QVariant(trackTags);

                    }else if(infoType == INFO::TAGS) continue;
                }

            }
        }
    }

    return QVariant();

}
