#include "spotifyService.h"


spotify::spotify(const Bae::DB &song)
{
    this->track = song;
    this->availableInfo.insert(ONTOLOGY::ALBUM, {INFO::ARTWORK});
    this->availableInfo.insert(ONTOLOGY::ARTIST, {});
    this->availableInfo.insert(ONTOLOGY::TRACK, {});
}

bool spotify::setUpService(const ONTOLOGY &ontology, const Pulpo::INFO &info)
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
    //    case ONTOLOGY::ARTIST:
    //    {
    //        url.append("artist:");
    //        url.append(encodedArtist.toString());
    //        url.append("&type=artist");
    //        break;
    //    }

    case ONTOLOGY::ALBUM:
    {
        QUrl encodedAlbum(this->track[Bae::DBCols::ALBUM]);
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
        QUrl encodedTrack(this->track[Bae::DBCols::TITLE]);
        encodedTrack.toEncoded(QUrl::FullyEncoded);

        url.append("track:");
        url.append(encodedTrack.toString());

        url.append("%20artist:");
        url.append(encodedArtist.toString());

        url.append("&type=track");
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

    this->array = this->startConnection(url, "Bearer "+token);
    if(this->array.isEmpty()) return false;

    return this->parseArray();
}

bool spotify::parseArray()
{
    if(this->ontology != Pulpo::ONTOLOGY::NONE)
        switch(this->ontology)
        {
        case Pulpo::ONTOLOGY::ALBUM: return this->parseAlbum();
            //        case Pulpo::ONTOLOGY::ARTIST: return this->parseArtist();
        case Pulpo::ONTOLOGY::TRACK:return this->parseTrack();
        default: return false;
        }

    return false;
}

bool spotify::parseArtist()
{
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
        emit this->infoReady(this->track,{{INFO::ARTWORK, startConnection(albumArt)}} );
        qDebug()<<"parseSpotifyAlbum ["<< albumArt<<"]";
        if(this->info == INFO::ARTWORK ) return true;
    }



    return true;
}

bool spotify::parseTrack()
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError)
    {
        qDebug() << "Error happened:" << jsonParseError.errorString();
        return false;

    }else
    {
        if (!jsonResponse.isObject())
        {
            qDebug() << "The json data is not an object";
            return false;

        }else
        {
            QJsonObject mainJsonObject(jsonResponse.object());
            auto data = mainJsonObject.toVariantMap();
            auto itemMap = data.value("tracks").toMap().value("items");

            if(!itemMap.isNull())
            {
                QList<QVariant> items = itemMap.toList();

                if(!items.isEmpty())
                {
                    if(this->info == INFO::ALBUM || this->info == INFO::ALL)
                    {
                        auto trackAlbum =items.first().toMap().value("album").toMap().value("name").toString();

                        emit this->infoReady(this->track,{{INFO::ALBUM, trackAlbum}});
                        if(this->info == INFO::ALBUM ) return true;
                    }

                    if(this->info == INFO::TRACK  || this->info == INFO::ALL)
                    {
                        auto trackPosition = items.first().toMap().value("track_number").toInt();

                        emit this->infoReady(this->track, {{INFO::TRACK, trackPosition}});
                        if(this->info == INFO::TRACK ) return true;
                    }

                } else return false;

            }else return false;
        }
    }
    return true;
}

//QVariant spotify::getTrackInfo(const Pulpo::INFO &infoType)
//{
//    QJsonParseError jsonParseError;
//    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);


//    if (jsonParseError.error != QJsonParseError::NoError)
//    {
//        qDebug() << "Error happened:" << jsonParseError.errorString();
//        return QVariant();

//    }else
//    {
//        if (!jsonResponse.isObject())
//        {
//            qDebug() << "The json data is not an object";
//            return QVariant();

//        }else
//        {
//            QJsonObject mainJsonObject(jsonResponse.object());
//            auto data = mainJsonObject.toVariantMap();
//            auto itemMap = data.value("tracks").toMap().value("items");

//            if(!itemMap.isNull())
//            {
//                QList<QVariant> items = itemMap.toList();

//                if(!items.isEmpty())
//                {
//                    if(infoType == INFO::ALBUM || infoType == INFO::ALL)
//                    {
//                        auto trackAlbum =items.first().toMap().value("album").toMap().value("name").toString();
//                        return QVariant(trackAlbum);
//                    }

//                    if(infoType == INFO::TRACK  || infoType == INFO::ALL)
//                    {
//                        auto trackPosition = items.first().toMap().value("track_number").toInt();
//                        return QVariant(trackPosition);
//                    }

//                } else return QVariant();

//            }else return QVariant();
//        }
//    }
//   return QVariant();
//}
