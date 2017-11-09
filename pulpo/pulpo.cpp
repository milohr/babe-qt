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

Pulpo::Pulpo(const Bae::DB &song,QObject *parent)
    : QObject(parent), track(song)
{
    //    this->page = new webEngine(this);
}

Pulpo::Pulpo(QObject *parent): QObject(parent) {}

Pulpo::~Pulpo() {}

void Pulpo::feed(const Bae::DB &song, const bool &recursive)
{
    this->track = song;
    this->recursive = recursive;
    this->initServices();

}

void Pulpo::registerServices(const QList<Pulpo::SERVICES> &services)
{
    this->registeredServices = services;
}

void Pulpo::setInfo(const Pulpo::INFO &info)
{
    this->info = info;
}

void Pulpo::setOntology(const Pulpo::ONTOLOGY &ontology)
{
    this->ontology = ontology;
}

void Pulpo::setRecursive(const bool &state)
{
    this->recursive=state;
}


bool Pulpo::initServices()
{
    if(this->registeredServices.isEmpty()) return false;
    if(this->track.isEmpty()) return false;

    for(auto service : this->registeredServices)

        switch (service)
        {
        case SERVICES::LastFm:
        {
            lastfm lastfm(this->track);
            connect(&lastfm, &lastfm::infoReady, this, &Pulpo::passSignal);

            if(lastfm.setUpService(this->ontology,this->info))
            {
                if(!recursive) return true;

            }else qDebug()<<"error settingUp lastfm service";


            break;
        }
        case SERVICES::Spotify:
        {
            spotify spotify(this->track);
            connect(&spotify, &spotify::infoReady, this, &Pulpo::passSignal);

            if(spotify.setUpService(this->ontology,this->info))
            {
                if(!recursive) return true;

            }else qDebug()<<"error settingUp sotify service";

            break;
        }
        case SERVICES::Genius:
        {
            break;
        }
        case SERVICES::MusicBrainz:
        {
            break;
        }
        case SERVICES::iTunes:
        {
            break;
        }
        case SERVICES::WikiLyrics:
        {
            break;
        }
        case SERVICES::LyricWikia:
        {
            break;
        }
        case SERVICES::Wikipedia:
        {
            break;
        }
        case SERVICES::ALL:
        {
            break;
        }
        case SERVICES::NONE:
        {
            break;
        }

        }
    return false;
}

void Pulpo::passSignal(const Bae::DB &track, const Pulpo::RES &response)
{
    emit this->infoReady(track, response);
}




//bool Pulpo::fetchArtistInfo(const INFO &infoType)
//{
//        if(Bae::albumType(this->track)==Bae::DBTables::ARTISTS)
//        {
//            QByteArray array;

//            lastfm lastfm(this->track);
//            connect(&lastfm,&lastfm::artistArtReady,[this] (const QByteArray &array) { emit Pulpo::artistArtReady(array);});
//            connect(&lastfm,&lastfm::artistWikiReady,[this] (const QString &wiki,const Bae::DB track)
//            {
//                qDebug() <<"got the artist wiki";
//                emit Pulpo::artistWikiReady(wiki,track);
//            });

//            connect(&lastfm,&lastfm::artistSimilarReady,[this] (const QMap<QString,QByteArray> &similar,const Bae::DB track)
//            {
//                emit Pulpo::artistSimilarReady(similar,track);
//            });

//            connect(&lastfm,&lastfm::artistTagsReady,[this] (const QStringList &tags,const Bae::DB track)
//            {
//                emit Pulpo::artistTagsReady(tags,track);
//            });

//            spotify spotify(this->track);
//            connect(&spotify,&spotify::artistArtReady,[this] (QByteArray array) { emit Pulpo::artistArtReady(array);});
//            connect(&spotify,&spotify::artistWikiReady,[this] (const QString &wiki,const Bae::DB &track)
//            {
//                emit Pulpo::artistWikiReady(wiki,track);
//            });
//            connect(&spotify,&spotify::artistSimilarReady,[this] (const QMap<QString,QByteArray> &similar,const Bae::DB &track)
//            {
//                emit Pulpo::artistSimilarReady(similar,track);
//            });
//            connect(&spotify,&spotify::artistTagsReady,[this] (const QStringList &tags,const Bae::DB &track)
//            {
//                emit Pulpo::artistTagsReady(tags,track);
//            });


//            /**************************************/

//            switch(service)
//            {
//            case LastFm:

//                array = startConnection(lastfm.setUpService(lastfm::ARTIST));
//                if(!array.isEmpty())
//                {
//                    if(lastfm.parseLastFmArtist(array, infoType)) return true;
//                    else return false;

//                }else return false;

//            case Spotify:

//                array = startConnection(spotify.setUpService(spotify::ARTIST));
//                if(!array.isEmpty())
//                {
//                    if(spotify.parseSpotifyArtist(array, infoType)) return true;
//                    else return false;

//                }else return false;


//            case iTunes: break;
//            case infoCRAWL: break;
//            case AllInfoServices: break;
//            case GeniusInfo: break;
//            case NoneInfoService: break;
//            }

//        }else return false;

//    return false;
//}



//bool Pulpo::fetchTrackInfo(const INFO &infoType)
//{
//    QByteArray array;

//    lastfm lastfm(this->track);
//    connect(&lastfm,&lastfm::trackWikiReady,[this] (const QString &wiki,const Bae::DB track) { emit Pulpo::trackWikiReady(wiki,track);});
//    connect(&lastfm,&lastfm::trackTagsReady,[this] (const QStringList &tags,const Bae::DB track) { emit Pulpo::trackTagsReady(tags,track);});
//    connect(&lastfm,&lastfm::trackAlbumReady,[this] (const QString &album,const Bae::DB track) { emit Pulpo::trackAlbumReady(album,track);});
//    connect(&lastfm,&lastfm::trackPositionReady,[this] (const int &position,const Bae::DB track) { emit Pulpo::trackPositionReady(position,track);});

//    spotify spotify(this->track);
//    connect(&spotify,&spotify::trackWikiReady,[this] (const QString &wiki,const Bae::DB track) { emit Pulpo::albumWikiReady(wiki,track);});
//    connect(&spotify,&spotify::trackTagsReady,[this] (const QStringList &tags,const Bae::DB track) { emit Pulpo::albumTagsReady(tags,track);});

//    lyricWikia lyricWikia(this->track);
//    connect(&lyricWikia,&lyricWikia::trackLyricsReady,[this] (const QString &lyric,const Bae::DB track)
//    {
//        if(!lyric.isEmpty()) emit Pulpo::trackLyricsReady(lyric,track);
//        else fetchTrackInfo(Pulpo::NoneTrackInfo,Pulpo::Genius,Pulpo::NoneInfoService);
//    });



//    if(!this->track[Bae::DBCols::ARTIST].isEmpty() && !this->track[Bae::DBCols::ALBUM].isEmpty() && service!= Pulpo::NoneInfoService)
//    {
//        switch(service)
//        {
//        case LastFm:

//            array = startConnection(lastfm.setUpService(lastfm::TRACK));

//            if(!array.isEmpty())
//            {
//                if(lastfm.parseLastFmTrack(array, infoType)) return true;
//                else return false;

//            }else return false;

//        case Spotify:

//            array = startConnection(spotify.setUpService(spotify::ARTIST));
//            if(!array.isEmpty())
//            {
//                if(spotify.parseSpotifyTrack(array, infoType)) return true;
//                else return false;

//            }else return false;


//        case iTunes: break;
//        case infoCRAWL: break;
//        case GeniusInfo: break;
//        case AllInfoServices: break;
//        case NoneInfoService: break;

//        }


//    }

//    if(!this->track[Bae::DBCols::TITLE].isEmpty() && !this->track[Bae::DBCols::ARTIST].isEmpty() && lyricService!=Pulpo::NoneLyricService)
//    {
//        switch(lyricService)
//        {
//        case WikiLyrics: break;
//        case LyricWikia:
//            array = startConnection(lyricWikia.setUpService());
//            if(!array.isEmpty())
//            {
//                if(lyricWikia.parseLyrics(array)) return true;
//                else return false;

//            }else return false;
//            break;
//        case Lyrics: break;
//        case Genius:
//        {

//            auto geniusURL = QUrl(genius::setUpService(this->track));
//            emit Pulpo::trackLyricsUrlReady(geniusURL,track);
//            //            if(geniusURL.isValid())
//            //            {
//            //                connect(page,&webEngine::htmlReady,[this]( QString const &html)
//            //                {
//            //                   genius genius(this->track);

//            //                   genius.parseLyrics(html.toLocal8Bit());
//            //                });

//            //                page->load(geniusURL);

//            //            }
//            //            break;
//        }
//        case lyricCRAWL: break;
//        case AllLyricServices: break;
//        case NoneLyricService: break;
//        }
//    }else return false;

//    return false;
//}


//QVariant Pulpo::getStaticAlbumInfo(const INFO &infoType)
//{
//    return QVariant ();
//}

//QVariant Pulpo::getStaticArtistInfo(const INFO &infoType)
//{
//    return QVariant ();

//}

//QVariant Pulpo::getStaticTrackInfo(const INFO &infoType)
//{
//    //    QByteArray array;

//    //    if(infoType == TrackAlbum)
//    //    {
//    //        lastfm lastfm(this->track);
//    //        array = startConnection(lastfm.setUpService(lastfm::TRACK));
//    //        auto title =lastfm.getTrackInfo(array,infoType).toString();

//    //        if(!title.isEmpty()) return title;
//    //        else
//    //        {
//    //            spotify spotify(this->track);
//    //            array = startConnection(spotify.setUpService(spotify::TRACK));
//    //            return title =spotify.getTrackInfo(array,infoType).toString();

//    //        }
//    //    }
//    //    if(infoType ==TrackLyrics)
//    //    {
//    //    }

//    //    if(infoType ==TrackPosition)
//    //    {
//    //        lastfm lastfm(this->track);
//    //        array = startConnection(lastfm.setUpService(lastfm::TRACK));
//    //        auto position =lastfm.getTrackInfo(array,infoType).toInt();

//    //        if(position>0) return position;
//    //        else
//    //        {
//    //            spotify spotify(this->track);
//    //            array = startConnection(spotify.setUpService(spotify::TRACK));
//    //            return position =spotify.getTrackInfo(array,infoType).toInt();

//    //        }
//    //    }
//    //    if(infoType ==TrackWiki)
//    //    {
//    //    }

//    return QVariant();
//}

QByteArray Pulpo::startConnection(const QString &url, const QString &auth)
{
    if(!url.isEmpty())
    {
        QUrl mURL(url);
        QNetworkAccessManager manager;
        QNetworkRequest request (mURL);

        if(!auth.isEmpty()) request.setRawHeader("Authorization", auth.toLocal8Bit());

        QNetworkReply *reply =  manager.get(request);
        QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
                SLOT(quit()));

        loop.exec();

        reply->deleteLater();
        if(reply->error()) qDebug()<<reply->error();
        return reply->readAll();
    }

    return QByteArray();
}


void Pulpo::saveArt(const QByteArray &array, const QString &path)
{
    if(!array.isNull()&&!array.isEmpty())
    {
        // qDebug()<<"tryna save array: "<< array;

        QImage img;
        img.loadFromData(array);
        QString name = !this->track[Bae::DBCols::ALBUM].isEmpty()? this->track[Bae::DBCols::ARTIST] + "_" + this->track[Bae::DBCols::ALBUM] : this->track[Bae::DBCols::ARTIST];
        name.replace("/", "-");
        name.replace("&", "-");
        QString format = "JPEG";
        if (img.save(path + name + ".jpg", format.toLatin1(), 100))
            this->track.insert(Bae::DBCols::ARTWORK,path + name + ".jpg");
        else  qDebug() << "couldn't save artwork";
    }else qDebug()<<"array is empty";

    emit artSaved(this->track);
}
