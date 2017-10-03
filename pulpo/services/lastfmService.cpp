#include "lastfmService.h"

lastfm::lastfm(const Bae::DB &song):track(song){}


QString lastfm::setUpService(const Ontology &type)
{
    QString url = this->API;

    QUrl encodedArtist(this->track[Bae::DBCols::ARTIST]);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    QUrl encodedAlbum(this->track[Bae::DBCols::ALBUM]);
    encodedAlbum.toEncoded(QUrl::FullyEncoded);

    QUrl encodedTrack(this->track[Bae::DBCols::TITLE]);
    encodedTrack.toEncoded(QUrl::FullyEncoded);

    switch(type)
    {
    case ARTIST:

        url.append("?method=artist.getinfo");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        break;

    case ALBUM:

        url.append("?method=album.getinfo");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        url.append("&album=" + encodedAlbum.toString());
        break;

    case TRACK:

        url.append("?method=track.getinfo");
        url.append(KEY);
        url.append("&artist=" + encodedArtist.toString());
        url.append("&track=" + encodedTrack.toString());
        break;
    }
//qDebug()<<"setUpService LastFm["<<url<<"]";
    return url;
}



bool lastfm::parseLastFmArtist(const QByteArray &array, const ArtistInfo &infoType)
{
    QString xmlData(array);
    QDomDocument doc;

    if (!doc.setContent(xmlData))
    {
        //qDebug() << "The XML obtained from last.fm is invalid.";
        return false;

    }else
    {
        QStringList artistTags;
        QByteArray artistSimilarArt;
        QMap<QString,QByteArray> artistSimilar;

        if (doc.documentElement().toElement().attributes().namedItem("status").nodeValue()!="ok")
            return false;

        const QDomNodeList nodeList = doc.documentElement().namedItem("artist").childNodes();

        for (int i = 0; i < nodeList.count(); i++)
        {
            QDomNode n = nodeList.item(i);

            if (n.isElement())
            {
                //Here retrieve the artist image
                if(infoType == ArtistArt || infoType == AllArtistInfo)
                {
                    if(n.nodeName() == "image" && n.hasAttributes())
                    {
                        QString imgSize = n.attributes().namedItem("size").nodeValue();

                        if (imgSize == "extralarge" && n.isElement())
                        {
                            auto artistArt_url = n.toElement().text();
                            //qDebug()<<"Fetching ArtistArt LastFm["<<artistArt_url<<"]";
                            emit artistArtReady(startConnection(artistArt_url));
                            if(infoType == ArtistArt) return true;
                            else continue;

                        }else if(infoType == ArtistArt) continue;
                    }
                }

                //Here retrieve the artist wiki (bio)
                if(infoType == ArtistWiki || infoType == AllArtistInfo)
                {
                    if (n.nodeName() == "bio")
                    {
                        auto artistWiki = n.childNodes().item(2).toElement().text();
                        //qDebug()<<"Fetching ArtistWiki LastFm[]";
                        emit artistWikiReady(artistWiki,this->track);
                        if(infoType == ArtistWiki) return true;
                        else continue;
                    }else if(infoType == ArtistWiki) continue;
                }


                //Here retrieve the artist similar artists
                if(infoType == ArtistSimilar || infoType == AllArtistInfo)
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

                        emit artistSimilarReady(artistSimilar,this->track);
                        if(infoType == ArtistSimilar) return true;
                        else continue;
                    }else if(infoType == ArtistSimilar) continue;
                }

                if(infoType == ArtistTags || infoType == AllArtistInfo)
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

                        emit artistTagsReady(artistTags,this->track);
                        if(infoType == ArtistTags) return true;
                    }else if(infoType == ArtistTags) continue;

                }
            }
        }
    }

    return true;
}

bool lastfm::parseLastFmAlbum(const QByteArray &array, const AlbumInfo &infoType)
{
    QString xmlData(array);
    QDomDocument doc;

    if (!doc.setContent(xmlData))
    {
        //qDebug() << "The XML obtained from last.fm is invalid.";
        return false;

    }else
    {
        if (doc.documentElement().toElement().attributes().namedItem("status").nodeValue()!="ok")
        {
            //qDebug() << "lfm status=failed.";

            return false;
        }

        const QDomNodeList nodeList = doc.documentElement().namedItem("album").childNodes();

        for (int i = 0; i < nodeList.count(); i++)
        {
            QDomNode n = nodeList.item(i);

            if (n.isElement())
            {
                //Here retrieve the artist image
                if(infoType == AlbumArt || infoType == AllAlbumInfo)
                {
                    ////qDebug()<<"lastfm[AlbumArt]";
                    if(n.nodeName() == "image" && n.hasAttributes())
                    {
                        QString imgSize = n.attributes().namedItem("size").nodeValue();

                        if (imgSize == "extralarge" && n.isElement())
                        {
                            auto albumArt_url = n.toElement().text();
                            //qDebug()<<"Fetching AlbumArt LastFm[]";

                            emit albumArtReady(startConnection(albumArt_url));

                            if(infoType == AlbumArt) return true;
                            else continue;

                        }else continue;

                    }else if(infoType == AlbumArt) continue;
                }

                if(infoType == AlbumWiki || infoType == AllAlbumInfo)
                {
                    if (n.nodeName() == "wiki")
                    {
                        auto albumWiki = n.childNodes().item(1).toElement().text();
                        //qDebug()<<"Fetching AlbumWiki LastFm[]";

                        emit albumWikiReady(albumWiki,this->track);
                        if(infoType == AlbumWiki) return true;
                        else continue;

                    }else if(infoType == AlbumWiki) continue;
                }

                if(infoType == AlbumTags || infoType == AllAlbumInfo)
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
                        //qDebug()<<"Fetching AlbumTags LastFm["<<albumTags<<"]";

                        emit albumTagsReady(albumTags,this->track);
                        if(infoType == AlbumTags) return true;
                        else continue;

                    }else if(infoType == AlbumTags) continue;
                }

            }
        }
    }

    return true;
}

bool lastfm::parseLastFmTrack(const QByteArray &array, const TrackInfo &infoType)
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

                if(infoType == TrackPosition || infoType == AllTrackInfo)
                {
                    if(n.nodeName() == "album" && n.hasAttributes())
                    {
                        //qDebug()<<"Fetching TrackPosition LastFm[]";

                        int position = n.attributes().namedItem("position").toElement().text().toInt();
                        emit trackPositionReady(position,this->track);
                        if(infoType == TrackPosition) return true;

                    }else if(infoType == TrackPosition) continue;

                }

                if(infoType == TrackAlbum || infoType == AllTrackInfo)
                {
                    ////qDebug()<<"lastfm[AlbumArt]";
                    if(n.nodeName() == "album")
                    {
                        //qDebug()<<"Fetching TrackAlbum LastFm[]";

                        auto trackAlbum = n.namedItem("title").toElement().text();
                        emit trackAlbumReady(trackAlbum,this->track);
                        if(infoType == TrackAlbum) return true;
                        else continue;

                    }else if(infoType == TrackAlbum) continue;
                }

                if(infoType == TrackWiki || infoType == AllTrackInfo)
                {

                    if (n.nodeName() == "wiki")
                    {
                        //qDebug()<<"Fetching TrackWiki LastFm[]";

                        auto trackWiki = n.namedItem("summary").toElement().text();
                        //qDebug()<<trackWiki;
                        emit trackWikiReady(trackWiki,this->track);
                        if(infoType == TrackWiki) return true;
                        else continue;

                    }else if(infoType == TrackWiki) continue;

                }

                if(infoType == TrackTags || infoType == AllTrackInfo)
                {
                    if (n.nodeName() == "toptags")
                    {

                        //qDebug()<<"Fetching TrackTags LastFm[]";
                        auto tagsList = n.toElement().childNodes();
                        QStringList trackTags;
                        for(int i=0; i<tagsList.count(); i++)
                            trackTags<<tagsList.item(i).namedItem("name").toElement().text();


                        emit trackTagsReady(trackTags,this->track);
                        if(infoType == TrackTags) return true;
                        else continue;
                    }else if(infoType == TrackTags) continue;
                }

            }
        }
    }

    return true;
}

QVariant lastfm::getTrackInfo(const QByteArray &array, const TrackInfo &infoType)
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

                if(infoType == TrackPosition || infoType == AllTrackInfo)
                {
                    if(n.nodeName() == "album" && n.hasAttributes())
                    {
                        int position = n.attributes().namedItem("position").toElement().text().toInt();
                        return QVariant(position);

                    }else return QVariant(0);

                }

                if(infoType == TrackAlbum || infoType == AllTrackInfo)
                {
                    ////qDebug()<<"lastfm[AlbumArt]";
                    if(n.nodeName() == "album")
                    {
                        auto trackAlbum = n.namedItem("title").toElement().text();
                        return QVariant(trackAlbum);

                    }else if(infoType == TrackAlbum) continue;
                }

                if(infoType == TrackWiki || infoType == AllTrackInfo)
                {

                    if (n.nodeName() == "wiki")
                    {
                        auto trackWiki = n.namedItem("summary").toElement().text();
                        //qDebug()<<trackWiki;
                        return QVariant(trackWiki);

                    }else if(infoType == TrackWiki) continue;

                }

                if(infoType == TrackTags || infoType == AllTrackInfo)
                {
                    if (n.nodeName() == "toptags")
                    {
                        //qDebug()<<"found toptags";
                        auto tagsList = n.toElement().childNodes();
                        QStringList trackTags;
                        for(int i=0; i<tagsList.count(); i++)
                            trackTags<<tagsList.item(i).namedItem("name").toElement().text();


                        return QVariant(trackTags);

                    }else if(infoType == TrackTags) continue;
                }

            }
        }
    }

    return QVariant();

}
