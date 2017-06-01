#include "spotifyService.h"

#include <QByteArray>
#include <QDebug>
#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>
#include <QUrl>

spotify::spotify(const QString &title_, const QString &artist_, const QString &album_) :
    artist(artist_),
    album(album_),
    title(title_)
{
}

QString spotify::setUpService(const spotify::Ontology &type)
{
    QString url = this->API;

    QUrl encodedArtist(this->artist);
    encodedArtist.toEncoded(QUrl::FullyEncoded);

    QUrl encodedAlbum(this->album);
    encodedAlbum.toEncoded(QUrl::FullyEncoded);

    QUrl encodedTrack(this->title);
    encodedTrack.toEncoded(QUrl::FullyEncoded);

    switch(type) {
    case ARTIST:
        url.append("artist:");
        url.append(encodedArtist.toString());
        url.append("&type=artist");
        break;
    case ALBUM:
        url.append("album:");
        url.append(encodedAlbum.toString());
        url.append("%20artist:");
        url.append(encodedArtist.toString());
        url.append("&type=album");
        break;
    case TRACK:
        url.append("track:");
        url.append(encodedTrack.toString());
        url.append("%20artist:");
        url.append(encodedArtist.toString());
        url.append("&type=track");
        break;
    }
    return url;
}

bool spotify::parseSpotifyArtist(const QByteArray &array, const Pulpo::ArtistInfo &infoType)
{
    Q_UNUSED(array)
    Q_UNUSED(infoType)
    return false;
}

bool spotify::parseSpotifyAlbum(const QByteArray &array, const AlbumInfo &infoType)
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError) {
        qDebug() << "Error happened:" << jsonParseError.errorString();
        return false;
    } else {
        if (!jsonResponse.isObject()) {
            qDebug() << "The json data is not an object";
            return false;
        } else {
            QJsonObject mainJsonObject(jsonResponse.object());
            auto data = mainJsonObject.toVariantMap();
            auto itemMap = data.value("albums").toMap().value("items");
            if (!itemMap.isNull()) {
                QList<QVariant> items = itemMap.toList();
                if (!items.isEmpty()) {
                    if (infoType == AlbumArt || infoType == AllAlbumInfo) {
                        auto albumArt = items.at(0).toMap().value("images").toList().at(0).toMap().value("url").toString();
                        emit albumArtReady(extractImg(albumArt));
                        qDebug() << "parseSpotifyAlbum [" << albumArt << "]";
                        if (infoType == AlbumArt )
                            return true;
                    }
                } else {
                    return false;
                }
            } else {
                qDebug() << "map is null";
                return false;
            }
        }
    }
    return true;
}

bool spotify::parseSpotifyTrack(const QByteArray &array, const TrackInfo &infoType)
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError) {
        qDebug() << "Error happened:" << jsonParseError.errorString();
        return false;
    } else {
        if (!jsonResponse.isObject()) {
            qDebug() << "The json data is not an object";
            return false;
        } else {
            QJsonObject mainJsonObject(jsonResponse.object());
            auto data = mainJsonObject.toVariantMap();
            auto itemMap = data.value("tracks").toMap().value("items");

            if (!itemMap.isNull()) {
                QList<QVariant> items = itemMap.toList();
                if (!items.isEmpty()) {
                    if (infoType == TrackAlbum || infoType == AllTrackInfo) {
                        auto trackAlbum =items.first().toMap().value("album").toMap().value("name").toString();
                        emit trackAlbumReady(trackAlbum);
                        if (infoType == TrackAlbum)
                            return true;
                    }
                    if (infoType == TrackPosition  || infoType == AllTrackInfo) {
                        auto trackPosition = items.first().toMap().value("track_number").toInt();
                        emit trackPositionReady(trackPosition);
                        if (infoType == TrackPosition)
                            return true;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    return true;
}

QVariant spotify::getTrackInfo(const QByteArray &array, const Pulpo::TrackInfo &infoType)
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(static_cast<QString>(array).toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError) {
        qDebug() << "Error happened:" << jsonParseError.errorString();
        return QVariant();
    } else {
        if (!jsonResponse.isObject()) {
            qDebug() << "The json data is not an object";
            return QVariant();
        } else {
            QJsonObject mainJsonObject(jsonResponse.object());
            auto data = mainJsonObject.toVariantMap();
            auto itemMap = data.value("tracks").toMap().value("items");

            if (!itemMap.isNull()) {
                QList<QVariant> items = itemMap.toList();

                if (!items.isEmpty()) {
                    if (infoType == TrackAlbum || infoType == AllTrackInfo) {
                        auto trackAlbum =items.first().toMap().value("album").toMap().value("name").toString();
                        return QVariant(trackAlbum);
                    }

                    if (infoType == TrackPosition  || infoType == AllTrackInfo) {
                        auto trackPosition = items.first().toMap().value("track_number").toInt();
                        return QVariant(trackPosition);
                    }
                } else {
                    return QVariant();
                }
            } else {
                return QVariant();
            }
        }
    }
   return QVariant();
}
