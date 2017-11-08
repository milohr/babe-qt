#include "spotifyService.h"


spotify::spotify(const Bae::DB &song)
{
    this->track = song;
}

bool spotify::setUpService(const ONTOLOGY &ontology, const Pulpo::INFO &info)
{
    this->ontology = ontology;
    this->info= info;

    auto url = this->API;

    QUrl encodedArtist(this->track[Bae::DBCols::ARTIST]);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    switch(this->ontology)
    {
    case ONTOLOGY::ARTIST:
    {
        url.append("artist:");
        url.append(encodedArtist.toString());
        url.append("&type=artist");
        break;
    }

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

    qDebug()<<"setUpService Spotify["<<url<<"]";

    auto credentials = this->CLIENT_ID+":"+this->CLIENT_SECRET;
    auto auth = credentials.toLocal8Bit().toBase64();
    QString header = "Basic " + auth;


    auto request = QNetworkRequest(QUrl("https://accounts.spotify.com/api/token"));
    request.setRawHeader("Authorization", header.toLocal8Bit());

    QNetworkAccessManager manager;
    QNetworkReply *reply =  manager.post(request,"grant_type=client_credentials");

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
            SLOT(quit()));

    loop.exec();

    reply->deleteLater();

    if(reply->error()) qDebug()<<reply->error();
    auto response  = reply->readAll();
    auto data = QJsonDocument::fromJson(response).object().toVariantMap();
    auto token = data["access_token"].toString();

    this->array = this->startConnection(url, "Bearer "+token);

    if(this->array.isEmpty()) return false;

    return this->parseArray();
}

bool spotify::parseArray()
{
    if(this->ontology != Pulpo::ONTOLOGY::NONE)
    {
        switch(this->ontology)
        {
        case Pulpo::ONTOLOGY::ALBUM: this->parseAlbum(); break;
        case Pulpo::ONTOLOGY::ARTIST: this->parseArtist(); break;
        case Pulpo::ONTOLOGY::TRACK: this->parseTrack(); break;
        default: return false;
        }
    }

    return true;
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
            auto itemMap = data.value("albums").toMap().value("items");

            if(!itemMap.isNull())
            {

                QList<QVariant> items = itemMap.toList();

                if(!items.isEmpty())
                {
                    if(this->info == INFO::ARTWORK || this->info == INFO::ALL)
                    {
                        auto albumArt =items.first().toMap().value("images").toList().first().toMap().value("url").toString();
                        emit this->infoReady(this->track,{{INFO::ARTWORK, startConnection(albumArt)}} );
                        qDebug()<<"parseSpotifyAlbum ["<< albumArt<<"]";
                        if(this->info == INFO::ARTWORK ) return true;
                    }

                } else return false;

            }else { qDebug()<<"map is null"; return false; }
        }
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
