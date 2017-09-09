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
#include "pulpo/services/lastfmService.h"
#include "pulpo/services/spotifyService.h"
#include "pulpo/services/lyricwikiaService.h"
#include "pulpo/services/geniusService.h"

Pulpo::Pulpo(const Bae::TRACKMAP &song,QObject *parent)
    : QObject(parent), track(song) {

    page = new webEngine();
}


Pulpo::Pulpo(QObject *parent){}

Pulpo::~Pulpo() {}

void Pulpo::feed(const Bae::TRACKMAP &song)
{
    this->track=song;
}

bool Pulpo::fetchArtistInfo(const ArtistInfo &infoType, const InfoServices &service)
{
    if(!this->track[Bae::TracksCols::ARTIST].isEmpty())
    {
        QByteArray array;

        lastfm lastfm(this->track);
        connect(&lastfm,&lastfm::artistArtReady,[this] (const QByteArray &array) { emit Pulpo::artistArtReady(array);});
        connect(&lastfm,&lastfm::artistWikiReady,[this] (const QString &wiki,const Bae::TRACKMAP track)
        {
            qDebug() <<"got the artist wiki";
            emit Pulpo::artistWikiReady(wiki,track);
        });

        connect(&lastfm,&lastfm::artistSimilarReady,[this] (const QMap<QString,QByteArray> &similar,const Bae::TRACKMAP track)
        {
            emit Pulpo::artistSimilarReady(similar,track);
        });

        connect(&lastfm,&lastfm::artistTagsReady,[this] (const QStringList &tags,const Bae::TRACKMAP track)
        {
            emit Pulpo::artistTagsReady(tags,track);
        });

        spotify spotify(this->track);
        connect(&spotify,&spotify::artistArtReady,[this] (QByteArray array) { emit Pulpo::artistArtReady(array);});
        connect(&spotify,&spotify::artistWikiReady,[this] (const QString &wiki,const Bae::TRACKMAP &track)
        {
            emit Pulpo::artistWikiReady(wiki,track);
        });
        connect(&spotify,&spotify::artistSimilarReady,[this] (const QMap<QString,QByteArray> &similar,const Bae::TRACKMAP &track)
        {
            emit Pulpo::artistSimilarReady(similar,track);
        });
        connect(&spotify,&spotify::artistTagsReady,[this] (const QStringList &tags,const Bae::TRACKMAP &track)
        {
            emit Pulpo::artistTagsReady(tags,track);
        });


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
        case GeniusInfo: break;
        case NoneInfoService: break;
        }

    }else return false;

    return false;
}



bool Pulpo::fetchAlbumInfo(const AlbumInfo &infoType, const InfoServices &service)
{
    //qDebug()<<"fetchAlbumInfo["<<this->title<<this->artist<<this->album<<"]";

    QByteArray array;
    lastfm lastfm(this->track);

    connect(&lastfm,&lastfm::albumArtReady,[this, infoType] (QByteArray array)
    {
        if((array.isEmpty() || array.isNull()))
        {
            this->fetchAlbumInfo(infoType,Spotify);
        }else
        {
            emit Pulpo::albumArtReady(array);
        }

    });

    connect(&lastfm,&lastfm::albumWikiReady,[this] (const QString &wiki, const Bae::TRACKMAP track) { emit Pulpo::albumWikiReady(wiki,track);});
    connect(&lastfm,&lastfm::albumTagsReady,[this] (const QStringList &tags,const Bae::TRACKMAP track) { emit Pulpo::albumTagsReady(tags,track);});

    spotify spotify(this->track);
    connect(&spotify,&spotify::albumArtReady,[this,infoType] (QByteArray array)
    {
        if((array.isEmpty() || array.isNull()))
        {
            this->fetchAlbumInfo(infoType,GeniusInfo);
        }else
        {
            emit Pulpo::albumArtReady(array);
        }
    });
    connect(&spotify,&spotify::albumWikiReady,[this] (const QString &wiki,const Bae::TRACKMAP track) { emit Pulpo::albumWikiReady(wiki,track);});
    connect(&spotify,&spotify::albumTagsReady,[this] (const QStringList &tags,const Bae::TRACKMAP track) { emit Pulpo::albumTagsReady(tags,track);});

    genius genius(this->track);
    connect(&genius,&genius::albumArtReady,[this] (QByteArray array) { emit Pulpo::albumArtReady(array); });

    if(!this->track[Bae::TracksCols::ARTIST].isEmpty() && !this->track[Bae::TracksCols::ALBUM].isEmpty())
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

            }else return false; break;

//        case GeniusInfo:
//            array = startConnection(genius.setUpService());
//            if(!array.isEmpty())
//            {
//                genius.parseAlbumArt(array);
//                return true;


//            }else return false;
//            break;



        case iTunes: break;
        case infoCRAWL: break;
        case AllInfoServices: break;
        case NoneInfoService: break;

        }


    }else return false;

    return false;
}

bool Pulpo::fetchTrackInfo(const Pulpo::TrackInfo &infoType, const Pulpo::LyricServices &lyricService, const Pulpo::InfoServices &service)
{
    QByteArray array;

    lastfm lastfm(this->track);
    connect(&lastfm,&lastfm::trackWikiReady,[this] (const QString &wiki,const Bae::TRACKMAP track) { emit Pulpo::trackWikiReady(wiki,track);});
    connect(&lastfm,&lastfm::trackTagsReady,[this] (const QStringList &tags,const Bae::TRACKMAP track) { emit Pulpo::trackTagsReady(tags,track);});
    connect(&lastfm,&lastfm::trackAlbumReady,[this] (const QString &album,const Bae::TRACKMAP track) { emit Pulpo::trackAlbumReady(album,track);});
    connect(&lastfm,&lastfm::trackPositionReady,[this] (const int &position,const Bae::TRACKMAP track) { emit Pulpo::trackPositionReady(position,track);});

    spotify spotify(this->track);
    connect(&spotify,&spotify::trackWikiReady,[this] (const QString &wiki,const Bae::TRACKMAP track) { emit Pulpo::albumWikiReady(wiki,track);});
    connect(&spotify,&spotify::trackTagsReady,[this] (const QStringList &tags,const Bae::TRACKMAP track) { emit Pulpo::albumTagsReady(tags,track);});

    lyricWikia lyricWikia(this->track);
    connect(&lyricWikia,&lyricWikia::trackLyricsReady,[this] (const QString &lyric,const Bae::TRACKMAP track)
    {
        if(!lyric.isEmpty()) emit Pulpo::trackLyricsReady(lyric,track);
        else fetchTrackInfo(Pulpo::NoneTrackInfo,Pulpo::Genius,Pulpo::NoneInfoService);
    });



    if(!this->track[Bae::TracksCols::ARTIST].isEmpty() && !this->track[Bae::TracksCols::ALBUM].isEmpty() && service!= Pulpo::NoneInfoService)
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
        case GeniusInfo: break;
        case AllInfoServices: break;
        case NoneInfoService: break;

        }


    }

    if(!this->track[Bae::TracksCols::TITLE].isEmpty() && !this->track[Bae::TracksCols::ARTIST].isEmpty() && lyricService!=Pulpo::NoneLyricService)
    {
        switch(lyricService)
        {
        case WikiLyrics: break;
        case LyricWikia:
            array = startConnection(lyricWikia.setUpService());
            if(!array.isEmpty())
            {
                if(lyricWikia.parseLyrics(array)) return true;
                else return false;

            }else return false;
            break;
        case Lyrics: break;
        case Genius:
        {

            auto geniusURL = QUrl(genius::setUpService(this->track));

            if(geniusURL.isValid())
            {
                connect(page,&webEngine::htmlReady,[this]( QString const &html)
                {
                   genius genius(this->track);

                   genius.parseLyrics(html.toLocal8Bit());
                });

                page->load(geniusURL);

            }
            break;
        }
        case lyricCRAWL: break;
        case AllLyricServices: break;
        case NoneLyricService: break;
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
        lastfm lastfm(this->track);
        array = startConnection(lastfm.setUpService(lastfm::TRACK));
        auto title =lastfm.getTrackInfo(array,infoType).toString();

        if(!title.isEmpty()) return title;
        else
        {
            spotify spotify(this->track);
            array = startConnection(spotify.setUpService(spotify::TRACK));
            return title =spotify.getTrackInfo(array,infoType).toString();

        }
    }
    if(infoType ==TrackLyrics)
    {
    }

    if(infoType ==TrackPosition)
    {
        lastfm lastfm(this->track);
        array = startConnection(lastfm.setUpService(lastfm::TRACK));
        auto position =lastfm.getTrackInfo(array,infoType).toInt();

        if(position>0) return position;
        else
        {
            spotify spotify(this->track);
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
    if(!url.isEmpty())
    {
        QUrl mURL(url);
        QNetworkAccessManager manager;
        QNetworkRequest request (mURL);
        QNetworkReply *reply =  manager.get(request);
        QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
                SLOT(quit()));

        loop.exec();

        reply->deleteLater();
        return reply->readAll();
    }else return QByteArray();
}




void Pulpo::dummy() { qDebug() << "QQQQQQQQQQQQQQQQQQQQ on DUMMYT"; }

void Pulpo::saveArt(const QByteArray &array, const QString &path)
{
    if(!array.isNull()&&!array.isEmpty())
    {
        QImage img;
        img.loadFromData(array);
        QString name = this->track[Bae::TracksCols::ALBUM].size() > 0 ? this->track[Bae::TracksCols::ARTIST] + "_" + this->track[Bae::TracksCols::ALBUM] : this->track[Bae::TracksCols::ARTIST];
        name.replace("/", "-");
        name.replace("&", "-");
        QString format = "JPEG";
        if (img.save(path + name + ".jpg", format.toLatin1(), 100))
        {
            if (this->track[Bae::TracksCols::ALBUM].isEmpty())
                emit artSaved(path + name + ".jpg", this->track);
            else
                emit artSaved(path + name + ".jpg", this->track);

            qDebug()<<path + name + ".jpg";
        } else {
            qDebug() << "couldn't save artwork";

            if (this->track[Bae::TracksCols::ALBUM].isEmpty())
                emit artSaved("", this->track);
            else
                emit artSaved("", this->track);
        }
    }else
    {
        if (this->track[Bae::TracksCols::ALBUM].isEmpty())
            emit artSaved("", this->track);
        else
            emit artSaved("", this->track);
    }
}
