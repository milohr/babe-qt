#include "spotifyService.h"

const QString spotify::auth = "Bearer BQDw6mqFaM-R9jGPH3fA-p7kPkaZ0QI7xl1HPTC59mr5_wzI_csQAwCbUtpgAfM0LeW7eukcaWMJ9kOHipSxMVkU7glO2H-fVLmT6VwKCfhfmMlW_tnqtUcha_8l9-5d8w5XKg";


spotify::spotify(const Bae::DB &song)
    : track(song) {}



bool spotify::setUpService(const ONTOLOGY &type)
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
    case ONTOLOGY::ARTIST:

        url.append("artist:");
        url.append(encodedArtist.toString());
        url.append("&type=artist");
        break;

    case ONTOLOGY::ALBUM:

        url.append("album:");
        url.append(encodedAlbum.toString());
        url.append("%20artist:");
        url.append(encodedArtist.toString());
        url.append("&type=album");
        break;

    case ONTOLOGY::TRACK:

        url.append("track:");
        url.append(encodedTrack.toString());

        url.append("%20artist:");
        url.append(encodedArtist.toString());

        url.append("&type=track");
        break;
    }
    qDebug()<<"setUpService Spotify["<<url<<"]";

    this->array = this->startConnection(url,this->auth);

    return this->array.isEmpty();

}

bool spotify::parseArtist(const Pulpo::INFO &infoType)
{
    return false;
}

bool spotify::parseAlbum(const Pulpo::INFO  &infoType)
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);
    qDebug()<<array;

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
                    if(infoType == INFO::ARTWORK || infoType == INFO::ALL)
                    {
                        auto albumArt =items.at(0).toMap().value("images").toList().at(0).toMap().value("url").toString();
                        emit albumArtReady(startConnection(albumArt));
                        qDebug()<<"parseSpotifyAlbum ["<< albumArt<<"]";
                        if(infoType == INFO::ARTWORK ) return true;

                    }


                } else return false;

            }else { qDebug()<<"map is null"; return false; }
        }
    }
    return true;
}

bool spotify::parseTrack(const Pulpo::INFO  &infoType)
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
                    if(infoType == INFO::ALBUM || infoType == INFO::ALL)
                    {
                        auto trackAlbum =items.first().toMap().value("album").toMap().value("name").toString();

                        emit trackAlbumReady(trackAlbum,this->track);
                        if(infoType == INFO::ALBUM ) return true;

                    }

                    if(infoType == INFO::TRACK  || infoType == INFO::ALL)
                    {
                        auto trackPosition = items.first().toMap().value("track_number").toInt();

                        emit trackPositionReady(trackPosition,this->track);
                        if(infoType == INFO::TRACK ) return true;
                    }

                } else return false;

            }else return false;
        }
    }
    return true;
}

QVariant spotify::getTrackInfo(const Pulpo::INFO &infoType)
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);


    if (jsonParseError.error != QJsonParseError::NoError)
    {
        qDebug() << "Error happened:" << jsonParseError.errorString();
        return QVariant();

    }else
    {
        if (!jsonResponse.isObject())
        {
            qDebug() << "The json data is not an object";
            return QVariant();

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
                    if(infoType == INFO::ALBUM || infoType == INFO::ALL)
                    {
                        auto trackAlbum =items.first().toMap().value("album").toMap().value("name").toString();
                        return QVariant(trackAlbum);
                    }

                    if(infoType == INFO::TRACK  || infoType == INFO::ALL)
                    {
                        auto trackPosition = items.first().toMap().value("track_number").toInt();
                        return QVariant(trackPosition);
                    }

                } else return QVariant();

            }else return QVariant();
        }
    }
   return QVariant();
}
