/*
   Babe - tiny music player
   Copyright (C) 2017  Camilo Higuita
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

   */


#include "pulpo.h"
#include "../baeUtils.h"
#include "pulpo/services/lastfmService.h"
#include "pulpo/services/spotifyService.h"


Pulpo::Pulpo(QString title_, QString artist_, QString album_, QObject *parent)
    : QObject(parent), album(BaeUtils::fixString(album_)), artist(BaeUtils::fixString(artist_)), title(BaeUtils::fixString(title_)) {}

Pulpo::~Pulpo() {}

bool Pulpo::fetchArtistInfo(const ArtistInfo &infoType, const InfoServices &service, const bool &recursive)
{
    if(!this->artist.isEmpty())
    {
        QByteArray array;

        lastfm lastfm(this->title,this->artist,this->album);
        connect(&lastfm,&lastfm::artistArtReady,[this] (QByteArray array) { emit Pulpo::artistArtReady(array);});
        connect(&lastfm,&lastfm::artistWikiReady,[this] (QString wiki) { emit Pulpo::artistWikiReady(wiki);});
        connect(&lastfm,&lastfm::artistSimilarReady,[this] (QMap<QString,QByteArray> similar) { emit Pulpo::artistSimilarReady(similar);});
        connect(&lastfm,&lastfm::artistTagsReady,[this] (QStringList tags) { emit Pulpo::artistTagsReady(tags);});

        spotify spotify(this->title,this->artist,this->album);
        connect(&spotify,&spotify::artistArtReady,[this] (QByteArray array) { emit Pulpo::artistArtReady(array);});
        connect(&spotify,&spotify::artistWikiReady,[this] (QString wiki) { emit Pulpo::artistWikiReady(wiki);});
        connect(&spotify,&spotify::artistSimilarReady,[this] (QMap<QString,QByteArray> similar) { emit Pulpo::artistSimilarReady(similar);});
        connect(&spotify,&spotify::artistTagsReady,[this] (QStringList tags) { emit Pulpo::artistTagsReady(tags);});


        /**************************************/

        switch(service)
        {
        case LastFm:

            array = startConnection(lastfm.setUpService(lastfm::ARTIST));
            if(!array.isEmpty())
            {
                if(lastfm.parseLastFmArtist(array, infoType)) return true;
                else return false;

            }else return false;

        case Spotify:

            array = startConnection(spotify.setUpService(spotify::ARTIST));
            if(!array.isEmpty())
            {
                if(spotify.parseSpotifyArtist(array, infoType)) return true;
                else return false;

            }else return false;


        case iTunes: break;
        case infoCRAWL: break;
        case AllInfoServices: break;
        }

    }else return false;

    return false;
}



bool Pulpo::fetchAlbumInfo(const AlbumInfo &infoType, const InfoServices &service, const bool &recursive)
{
    //qDebug()<<"fetchAlbumInfo["<<this->title<<this->artist<<this->album<<"]";

    QByteArray array;
    lastfm lastfm(this->title,this->artist,this->album);

    connect(&lastfm,&lastfm::albumArtReady,[this,infoType,recursive] (QByteArray array)
    {
        if((array.isEmpty() || array.isNull()) && recursive == true)
        {
            this->fetchAlbumInfo(infoType,Spotify,false);
        }else
        {
            emit Pulpo::albumArtReady(array);
        }

    });

    connect(&lastfm,&lastfm::albumWikiReady,[this] (QString wiki) { emit Pulpo::albumWikiReady(wiki);});
    connect(&lastfm,&lastfm::albumTagsReady,[this] (QStringList tags) { emit Pulpo::albumTagsReady(tags);});

    spotify spotify(this->title,this->artist,this->album);
    connect(&spotify,&spotify::albumArtReady,[this] (QByteArray array) { emit Pulpo::albumArtReady(array);});
    connect(&spotify,&spotify::albumWikiReady,[this] (QString wiki) { emit Pulpo::albumWikiReady(wiki);});
    connect(&spotify,&spotify::albumTagsReady,[this] (QStringList tags) { emit Pulpo::albumTagsReady(tags);});

    if(!this->artist.isEmpty() && !this->album.isEmpty())
    {

        switch(service)
        {
        case LastFm:

            array = startConnection(lastfm.setUpService(lastfm::ALBUM));

            if(!array.isEmpty())
            {
                if(lastfm.parseLastFmAlbum(array, infoType)) return true;
                else return false;

            }else return false;

        case Spotify:

            array = startConnection(spotify.setUpService(spotify::ALBUM));
            if(!array.isEmpty())
            {
                if(spotify.parseSpotifyAlbum(array, infoType)) return true;
                else return false;

            }else return false;


        case iTunes: break;
        case infoCRAWL: break;
        case AllInfoServices: break;
        }


    }else return false;

    return false;
}

bool Pulpo::fetchTrackInfo(const Pulpo::TrackInfo &infoType, const Pulpo::LyricServices &lyricService, const Pulpo::InfoServices &service)
{
    QByteArray array;
    lastfm lastfm(this->title,this->artist,this->album);
    connect(&lastfm,&lastfm::trackLyricsReady,[this] (QString wiki) { emit Pulpo::trackLyricsReady(wiki);});
    connect(&lastfm,&lastfm::trackWikiReady,[this] (QString wiki) { emit Pulpo::trackWikiReady(wiki);});
    connect(&lastfm,&lastfm::trackTagsReady,[this] (QStringList tags) { emit Pulpo::trackTagsReady(tags);});
    connect(&lastfm,&lastfm::trackAlbumReady,[this] (QString album) { emit Pulpo::trackAlbumReady(album);});
    connect(&lastfm,&lastfm::trackPositionReady,[this] (int position) { emit Pulpo::trackPositionReady(position);});

    spotify spotify(this->title,this->artist,this->album);
    connect(&spotify,&spotify::trackWikiReady,[this] (QString wiki) { emit Pulpo::albumWikiReady(wiki);});
    connect(&spotify,&spotify::trackTagsReady,[this] (QStringList tags) { emit Pulpo::albumTagsReady(tags);});

    if(!this->artist.isEmpty() && !this->album.isEmpty())
    {

        switch(service)
        {
        case LastFm:

            array = startConnection(lastfm.setUpService(lastfm::TRACK));

            if(!array.isEmpty())
            {
                if(lastfm.parseLastFmTrack(array, infoType)) return true;
                else return false;

            }else return false;

        case Spotify:

            array = startConnection(spotify.setUpService(spotify::ARTIST));
            if(!array.isEmpty())
            {
                if(spotify.parseSpotifyTrack(array, infoType)) return true;
                else return false;

            }else return false;


        case iTunes: break;
        case infoCRAWL: break;
        case AllInfoServices: break;

        }


    }else if(!this->title.isEmpty() && !this->artist.isEmpty())
    {
        switch(lyricService)
        {
        case WikiLyrics: break;
        case LyricWikia: break;
        case Lyrics: break;
        case lyricCRAWL: break;
        case AllLyricServices: break;
        }
    }else return false;

    return false;
}


QVariant Pulpo::getStaticAlbumInfo(const AlbumInfo &infoType)
{
return QVariant ();
}

QVariant Pulpo::getStaticArtistInfo(const ArtistInfo &infoType)
{
    return QVariant ();

}

QVariant Pulpo::getStaticTrackInfo(const TrackInfo &infoType)
{
    QByteArray array;

    if(infoType == TrackAlbum)
    {
        lastfm lastfm(this->title, this->artist, this->album);
        array = startConnection(lastfm.setUpService(lastfm::TRACK));
        auto title =lastfm.getTrackInfo(array,infoType).toString();

        if(!title.isEmpty()) return title;
        else
        {
            spotify spotify(this->title, this->artist, this->album);
            array = startConnection(spotify.setUpService(spotify::TRACK));
            return title =spotify.getTrackInfo(array,infoType).toString();

        }
    }
    if(infoType ==TrackLyrics)
    {
    }

    if(infoType ==TrackPosition)
    {
        lastfm lastfm(this->title, this->artist, this->album);
        array = startConnection(lastfm.setUpService(lastfm::TRACK));
        auto position =lastfm.getTrackInfo(array,infoType).toInt();

        if(position>0) return position;
        else
        {
            spotify spotify(this->title, this->artist, this->album);
            array = startConnection(spotify.setUpService(spotify::TRACK));
            return position =spotify.getTrackInfo(array,infoType).toInt();

        }
    }
    if(infoType ==TrackWiki)
    {
    }

    return QVariant();
}

QByteArray Pulpo::startConnection(const QString &url)
{
    QNetworkAccessManager manager;

    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
            SLOT(quit()));

    loop.exec();

    QByteArray array(reply->readAll());

    delete reply;
    return array;
}


void Pulpo::dummy() { qDebug() << "QQQQQQQQQQQQQQQQQQQQ on DUMMYT"; }

void Pulpo::saveArt(const QByteArray &array, const QString &path)
{
    if(!array.isNull()&&!array.isEmpty())
    {
        QImage img;
        img.loadFromData(array);
        QString name = this->album.size() > 0 ? this->artist + "_" + this->album : this->artist;
        name.replace("/", "-");
        name.replace("&", "-");
        QString format = "JPEG";
        if (img.save(path + name + ".jpg", format.toLatin1(), 100))
        {
            if (this->album.isEmpty())
                emit artSaved(path + name + ".jpg", {this->artist});
            else
                emit artSaved(path + name + ".jpg", {this->album, this->artist});

            qDebug()<<path + name + ".jpg";
        } else {
            qDebug() << "couldn't save artwork";

            if (album.isEmpty())
                emit artSaved("", {this->artist});
            else
                emit artSaved("", {this->album, this->artist});
        }
    }else
    {
        if (album.isEmpty())
            emit artSaved("", {this->artist});
        else
            emit artSaved("", {this->album, this->artist});
    }
}

QByteArray Pulpo::extractImg(QString url)
{
    QNetworkAccessManager manager;

    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

    QEventLoop loop;

    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
                     SLOT(quit()));
    loop.exec();

    QByteArray img(reply->readAll());
    delete reply;

    return img;
}
