#include "lastfmService.h"

lastfm::lastfm(const Bae::DB &song)
{
    this->track = song;
    this->availableInfo.insert(ONTOLOGY::ALBUM, {INFO::ARTWORK, INFO::WIKI, INFO::TAGS});
    this->availableInfo.insert(ONTOLOGY::ARTIST, {INFO::ARTWORK, INFO::WIKI, INFO::TAGS, INFO::SIMILAR});
    this->availableInfo.insert(ONTOLOGY::TRACK, {});

}

bool lastfm::setUpService(const Pulpo::ONTOLOGY &ontology, const Pulpo::INFO &info)
{
    this->ontology = ontology;
    this->info= info;

    if(!this->availableInfo[this->ontology].contains(this->info))
        return false;

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

    qDebug()<< "[lastfm service]: "<< url;

    this->array = this->startConnection(url);
    if(this->array.isEmpty()) return false;

    return this->parseArray();
}

bool lastfm::parseArray()
{
    if(this->ontology != Pulpo::ONTOLOGY::NONE)
        switch(this->ontology)
        {
        case Pulpo::ONTOLOGY::ALBUM: return this->parseAlbum();
        case Pulpo::ONTOLOGY::ARTIST: return this->parseArtist();
        case Pulpo::ONTOLOGY::TRACK: return this->parseTrack();
        default: return false;
        }

    return false;
}

bool lastfm::parseArtist()
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
            if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
            {
                if(n.nodeName() == "image" && n.hasAttributes())
                {
                    auto imgSize = n.attributes().namedItem("size").nodeValue();

                    if (imgSize == "extralarge" && n.isElement())
                    {
                        auto artistArt_url = n.toElement().text();
                        qDebug()<<"Fetching ArtistArt LastFm["<<artistArt_url<<"]";
                        emit this->infoReady(this->track,{{INFO::ARTWORK, startConnection(artistArt_url)}});

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

                    emit this->infoReady(this->track, {{INFO::WIKI,artistWiki}});

                    if(this->info == INFO::WIKI) return true;
                    else continue;
                }else if(this->info == INFO::WIKI) continue;
            }


            //Here retrieve the artist similar artists
            if(this->info == INFO::SIMILAR || this->info == INFO::ALL)
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

                    if(this->info == INFO::SIMILAR) return true;
                    else continue;
                }else if(this->info == INFO::SIMILAR) continue;
            }

            if(this->info == INFO::TAGS || this->info == INFO::ALL)
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
                    if(this->info == INFO::TAGS) return true;
                }else if(this->info == INFO::TAGS) continue;

            }
        }
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
                        emit this->infoReady(this->track,{{INFO::ARTWORK,startConnection(albumArt_url)}});

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

                    emit this->infoReady(this->track,{{INFO::WIKI, albumWiki}});

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
                    emit this->infoReady(this->track,{{INFO::TAGS, albumTags}});
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

                if(this->info == INFO::TRACK || this->info == INFO::ALL)
                {
                    if(n.nodeName() == "album" && n.hasAttributes())
                    {
                        //qDebug()<<"Fetching TrackPosition LastFm[]";

                        int position = n.attributes().namedItem("position").toElement().text().toInt();
                        emit this->infoReady(this->track, {{INFO::TRACK, position}});
                        if(this->info == INFO::TRACK) return true;

                    }else if(this->info == INFO::TRACK) continue;

                }

                if(this->info == INFO::ALBUM || this->info == INFO::ALL)
                {
                    ////qDebug()<<"lastfm[AlbumArt]";
                    if(n.nodeName() == "album")
                    {
                        //qDebug()<<"Fetching TrackAlbum LastFm[]";

                        auto trackAlbum = n.namedItem("title").toElement().text();

                        emit this->infoReady(this->track,{{INFO::ALBUM, trackAlbum}});
                        if(this->info == INFO::ALBUM) return true;
                        else continue;

                    }else if(this->info == INFO::ALBUM) continue;
                }

                if(this->info == INFO::WIKI || this->info == INFO::ALL)
                {

                    if (n.nodeName() == "wiki")
                    {
                        //qDebug()<<"Fetching TrackWiki LastFm[]";

                        auto trackWiki = n.namedItem("summary").toElement().text();
                        //qDebug()<<trackWiki;
                        emit this->infoReady(this->track, {{INFO::WIKI, trackWiki}});
                        if(this->info == INFO::WIKI) return true;
                        else continue;

                    }else if(this->info == INFO::WIKI) continue;

                }

                if(this->info == INFO::TAGS || this->info == INFO::ALL)
                {
                    if (n.nodeName() == "toptags")
                    {

                        //qDebug()<<"Fetching TrackTags LastFm[]";
                        auto tagsList = n.toElement().childNodes();
                        QStringList trackTags;
                        for(int i=0; i<tagsList.count(); i++)
                            trackTags<<tagsList.item(i).namedItem("name").toElement().text();

                        emit this->infoReady(this->track, {{INFO::TAGS, trackTags}});
                        if(this->info == INFO::TAGS) return true;
                        else continue;
                    }else if(this->info == INFO::TAGS) continue;
                }

            }
        }
    }

    return true;
}

//QVariant lastfm::getTrackInfo(const QByteArray &array, const INFO &this->info)
//{
//    QString xmlData(array);
//    QDomDocument doc;

//    if (!doc.setContent(xmlData))
//    {
//        //qDebug() << "The XML obtained from last.fm is invalid.";
//        return QVariant();

//    }else
//    {
//        if (doc.documentElement().toElement().attributes().namedItem("status").nodeValue()!="ok")
//            return QVariant();

//        const QDomNodeList nodeList = doc.documentElement().namedItem("track").childNodes();

//        for (int i = 0; i < nodeList.count(); i++)
//        {
//            QDomNode n = nodeList.item(i);

//            if (n.isElement())
//            {

//                if(this->info == INFO::TRACK || this->info == INFO::ALL)
//                {
//                    if(n.nodeName() == "album" && n.hasAttributes())
//                    {
//                        int position = n.attributes().namedItem("position").toElement().text().toInt();
//                        return QVariant(position);

//                    }else return QVariant(0);

//                }

//                if(this->info == INFO::ALBUM || this->info == INFO::ALL)
//                {
//                    ////qDebug()<<"lastfm[AlbumArt]";
//                    if(n.nodeName() == "album")
//                    {
//                        auto trackAlbum = n.namedItem("title").toElement().text();
//                        return QVariant(trackAlbum);

//                    }else if(this->info == INFO::ALBUM) continue;
//                }

//                if(this->info == INFO::WIKI || this->info == INFO::ALL)
//                {

//                    if (n.nodeName() == "wiki")
//                    {
//                        auto trackWiki = n.namedItem("summary").toElement().text();
//                        //qDebug()<<trackWiki;
//                        return QVariant(trackWiki);

//                    }else if(this->info == INFO::WIKI) continue;

//                }

//                if(this->info == INFO::TAGS || this->info == INFO::ALL)
//                {
//                    if (n.nodeName() == "toptags")
//                    {
//                        //qDebug()<<"found toptags";
//                        auto tagsList = n.toElement().childNodes();
//                        QStringList trackTags;
//                        for(int i=0; i<tagsList.count(); i++)
//                            trackTags<<tagsList.item(i).namedItem("name").toElement().text();


//                        return QVariant(trackTags);

//                    }else if(this->info == INFO::TAGS) continue;
//                }

//            }
//        }
//    }

//    return QVariant();

//}
