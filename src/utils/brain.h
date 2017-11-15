#ifndef BRAIN_H
#define BRAIN_H

#endif // BRAIN_H

/* This deamon keeps on running while there are missing information about a track,
 *  it should have the option to turn it off, but the main idea is to here have the
 * brains of the app and collection. so this must be a very good a neat implementation */


#include <QObject>
#include <QThread>

#include "bae.h"

#include "../db/collectionDB.h"
#include "../pulpo/pulpo.h"
#include "../services/local/taginfo.h"

using namespace Bae;

namespace Deamon {

class Brain : public QObject
{
    Q_OBJECT

public:
    Brain() : QObject ()
    {
        this->moveToThread(&t);
        qRegisterMetaType<DB>("DB");
        qRegisterMetaType<TABLE>("TABLE");
        t.start();
    }

    ~Brain()
    {
        qDebug()<<"Deleting Brainz obj";
        this->stop();
    }

    void start()
    {
        if(this->isRunning()) this->go = false;
        this->go = true;
        QMetaObject::invokeMethod(this, "synapse");
    }

    void stop()
    {
        this->go = false;
        t.quit();
        t.wait();
    }

    bool isRunning()
    {
        return this->go;
    }

    void setInterval(const uint &value)
    {
        qDebug()<< "reseting the interval brainz";
        this->interval = value*60000;
    }


public slots:

    void synapse()
    {
        if(go)
        {
            this->albumInfo();
            this->artistInfo();
            this->trackInfo();
        }

        emit this->finished();
        this->go = false;

    }

    void trackInfo()
    {
        if(!go) return;
        Pulpo pulpo;
        connect(&pulpo, &Pulpo::infoReady, [&] (const DB &track, const PULPO::RESPONSE &response)
        {
            for( auto info : response.keys())
                switch(info)
                {
                case PULPO::INFO::TAGS:
                {
                    if(!response[info].isEmpty())
                    {
                        for (auto context :response[info].keys())
                            switch(context)
                            {
                            case PULPO::CONTEXT::TRACK_TEAM:
                            case PULPO::CONTEXT::TAG:
                            case PULPO::CONTEXT::STAT:
                            {
                                if (!response[info][context].toStringList().isEmpty())
                                    for( auto tag : response[info][context].toStringList() )
                                        this->connection.tagsTrack(track,tag,PULPO::CONTEXT_MAP[context]);

                                if (!response[info][context].toString().isEmpty())
                                    this->connection.tagsTrack(track,response[info][context].toString(),PULPO::CONTEXT_MAP[context]);
                                break;
                            }
                            default: continue;
                            }
                    }

                    break;
                }

                case PULPO::INFO::WIKI:
                {
                    if(!response[info].isEmpty())
                        if (!response[info][PULPO::CONTEXT::WIKI].toString().isEmpty())
                            this->connection.wikiTrack(track,response[info][PULPO::CONTEXT::WIKI].toString());
                    break;
                }

                case PULPO::INFO::ARTWORK:
                {
                    if(!response[info].isEmpty())

                        if(!response[info][PULPO::CONTEXT::IMAGE].toByteArray().isEmpty())
                        {
                            connect(&pulpo, &Pulpo::artSaved, &connection, &CollectionDB::insertArtwork);
                            pulpo.saveArt(response[info][PULPO::CONTEXT::IMAGE].toByteArray(),CachePath);
                        }

                    break;
                }

                case PULPO::INFO::METADATA:
                {
                    for (auto context :response[info].keys())
                        switch(context)
                        {
                        case PULPO::CONTEXT::ALBUM_TITLE:
                        {
                            qDebug()<<"SETTING TRACK MISSING METADATA";

                            TagInfo tag(track[KEY::URL]);

                            if(!response[info][context].toString().isEmpty())
                            {
                                tag.setAlbum(response[info][context].toString());
                                this->connection.albumTrack(track, response[info][context].toString());
                            }

                            break;
                        }

                        case PULPO::CONTEXT::TRACK_NUMBER:
                        {

                            TagInfo tag(track[KEY::URL]);
                            if(!response[info][context].toString().isEmpty())
                            {
                                tag.setTrack(response[info][context].toInt());
                            }

                            break;
                        }

                        default: continue;
                        }

                    break;
                }

                case PULPO::INFO::LYRICS:
                {
                    if(!response[info][PULPO::CONTEXT::LYRIC].toString().isEmpty())
                        this->connection.lyricsTrack(track,response[info][PULPO::CONTEXT::LYRIC].toString());
                    break;
                }
                default: continue;

                }
        });

        pulpo.setOntology(PULPO::ONTOLOGY::TRACK);


        //        auto queryTxt =  QString("SELECT %1, %2, %3 FROM %4 WHERE %3 = 'UNKNOWN' GROUP BY %2, a.%3 ").arg(KEYMAP[KEY::TITLE],
        //                KEYMAP[KEY::ARTIST],KEYMAP[KEY::ALBUM],TABLEMAP[TABLE::TRACKS]);
        //        QSqlQuery query (queryTxt);
        //        pulpo.setInfo(PULPO::INFO::METADATA);
        //        for(auto track : connection.getDBData(query))
        //        {
        //            qDebug()<<"UNKOWN TRACK TITLE:"<<track[KEY::TITLE];
        //            pulpo.feed(track,PULPO::RECURSIVE::OFF);
        //            if(!go) return;
        //        }

        pulpo.registerServices({PULPO::SERVICES::LyricWikia});

        auto queryTxt =  QString("SELECT %1, %2, %3 FROM %4 WHERE %5 = ''").arg(KEYMAP[KEY::URL],
                KEYMAP[KEY::TITLE],
                KEYMAP[KEY::ARTIST],
                TABLEMAP[TABLE::TRACKS],
                KEYMAP[KEY::LYRICS]);
        qDebug()<< queryTxt;
        QSqlQuery query (queryTxt);
        pulpo.setInfo(PULPO::INFO::LYRICS);
        for(auto track : connection.getDBData(query))
        {
            this->connection.lyricsTrack(track,SLANG[W::NONE]);
            pulpo.feed(track,PULPO::RECURSIVE::OFF);

            if(!go) return;
        }


        pulpo.registerServices({PULPO::SERVICES::LastFm,PULPO::SERVICES::Spotify,PULPO::SERVICES::MusicBrainz});
        pulpo.setInfo(PULPO::INFO::ARTWORK);
        //select url, title, album, artist from tracks t inner join albums a on a.album=t.album and a.artist=t.artist where a.artwork = ''
        queryTxt =  QString("SELECT DISTINCT t.%1, t.%2, t.%3, t.%4 FROM %5 t INNER JOIN %6 a ON a.%3 = t.%3 AND a.%4 = t.%4  WHERE a.%7 = '' GROUP BY a.%3, a.%4 ").arg(KEYMAP[KEY::URL],
                KEYMAP[KEY::TITLE],
                KEYMAP[KEY::ARTIST],
                KEYMAP[KEY::ALBUM],
                TABLEMAP[TABLE::TRACKS],
                TABLEMAP[TABLE::ALBUMS],
                KEYMAP[KEY::ARTWORK]);
        query.prepare(queryTxt);
        for(auto track : connection.getDBData(query))
        {
            this->connection.insertArtwork(track);
            pulpo.feed(track,PULPO::RECURSIVE::OFF);

            if(!go) return;
        }

        emit this->artworkReady(TABLE::ALBUMS);

        // select title, artist, album from tracks t where url not in (select url from tracks_tags)
        queryTxt =  QString("SELECT %1, %2, %3, %4 FROM %5 WHERE %1 NOT IN ( SELECT %1 FROM %6 )").arg(KEYMAP[KEY::URL],
                KEYMAP[KEY::TITLE],
                KEYMAP[KEY::ARTIST],
                KEYMAP[KEY::ALBUM],
                TABLEMAP[TABLE::TRACKS],
                TABLEMAP[TABLE::TRACKS_TAGS]);

        query.prepare(queryTxt);

        pulpo.setInfo(PULPO::INFO::TAGS);
        for(auto track : connection.getDBData(query))
        {
            pulpo.feed(track,PULPO::RECURSIVE::ON);
            if(!go) return;
        }


        qDebug()<<"getting missing TRACK wikis";
        queryTxt =  QString("SELECT %1, %2, %3, %4 FROM %5 WHERE %6 = ''").arg(KEYMAP[KEY::URL],KEYMAP[KEY::TITLE],
                KEYMAP[KEY::ARTIST],KEYMAP[KEY::ALBUM],TABLEMAP[TABLE::TRACKS],KEYMAP[KEY::WIKI]);
        query.prepare(queryTxt);
        pulpo.setInfo(PULPO::INFO::WIKI);
        for(auto track : connection.getDBData(query))
        {
            this->connection.wikiTrack(track, SLANG[W::NONE]);
            pulpo.feed(track, PULPO::RECURSIVE::OFF);

            if(!go) return;
        }
    }

    void albumInfo()
    {
        if(!go) return;

        /*setup pulpo in place*/
        Pulpo pulpo;
        connect(&pulpo, &Pulpo::infoReady, [&] (const DB &track, const PULPO::RESPONSE &response)
        {
            for( auto info : response.keys())
                switch(info)
                {
                case PULPO::INFO::TAGS:
                {
                    for (auto context : response[info].keys())

                        if(!response[info][context].toMap().isEmpty())
                            for( auto tag :  response[info][context].toMap().keys() )
                                this->connection.tagsAlbum(track,tag, PULPO::CONTEXT_MAP[context]);

                        else if (!response[info][context].toStringList().isEmpty())
                            for( auto tag :  response[info][context].toStringList() )
                                this->connection.tagsAlbum(track,tag,PULPO::CONTEXT_MAP[context]);

                        else if (!response[info][context].toString().isEmpty())
                            this->connection.tagsAlbum(track,response[info][context].toString(),PULPO::CONTEXT_MAP[context]);


                    break;
                }

                case PULPO::INFO::ARTWORK:
                {

                    if(!response[info].isEmpty())

                        if(!response[info][PULPO::CONTEXT::IMAGE].toByteArray().isEmpty())
                        {
                            connect(&pulpo, &Pulpo::artSaved, &connection, &CollectionDB::insertArtwork);
                            pulpo.saveArt(response[info][PULPO::CONTEXT::IMAGE].toByteArray(),CachePath);
                        }

                    break;
                }

                case PULPO::INFO::WIKI:
                {
                    if(!response[info].isEmpty())
                        for (auto context : response[info].keys())
                            this->connection.wikiAlbum(track,response[info][context].toString());
                    break;
                }

                default: continue;
                }
        });

        pulpo.setOntology(PULPO::ONTOLOGY::ALBUM);
        pulpo.registerServices({PULPO::SERVICES::LastFm,PULPO::SERVICES::Spotify,PULPO::SERVICES::MusicBrainz});

        /* get all albums missing information */
        //select album, artist from albums where  album  not in (select album from albums_tags) and artist  not in (select  artist from albums_tags)

        qDebug()<<"getting missing artworks";
        auto queryTxt = QString("SELECT %1, %2 FROM %3 WHERE %4 = ''").arg(KEYMAP[KEY::ALBUM],
                KEYMAP[KEY::ARTIST],TABLEMAP[TABLE::ALBUMS],KEYMAP[KEY::ARTWORK]);
        QSqlQuery query (queryTxt);
        pulpo.setInfo(PULPO::INFO::ARTWORK);
        for(auto track : connection.getDBData(query))
        {
            pulpo.feed(track, PULPO::RECURSIVE::OFF);
            if(!go) return;
        }

        emit this->artworkReady(TABLE::ALBUMS);

        qDebug()<<"getting missing tags";
        queryTxt =  QString("SELECT %1, %2 FROM %3 WHERE %1 NOT IN ( SELECT %1 FROM %4 ) AND %2 NOT IN ( SELECT %2 FROM %4 )").arg(KEYMAP[KEY::ALBUM],
                KEYMAP[KEY::ARTIST],TABLEMAP[TABLE::ALBUMS],TABLEMAP[TABLE::ALBUMS_TAGS]);
        query.prepare(queryTxt);
        pulpo.setInfo(PULPO::INFO::TAGS);
        for(auto track : connection.getDBData(query))
        {
            pulpo.feed(track,PULPO::RECURSIVE::ON);
            if(!go) return;
        }

        qDebug()<<"getting missing wikis";
        queryTxt =  QString("SELECT %1, %2 FROM %3 WHERE %4 = '' ").arg(KEYMAP[KEY::ALBUM],
                KEYMAP[KEY::ARTIST],TABLEMAP[TABLE::ALBUMS],KEYMAP[KEY::WIKI]);
        query.prepare(queryTxt);
        pulpo.setInfo(PULPO::INFO::WIKI);
        for(auto track : connection.getDBData(query))
        {
            pulpo.feed(track, PULPO::RECURSIVE::ON);
            if(!go) return;
        }
    }

    void artistInfo()
    {
        if(!go) return;

        /*setup pulpo in place*/
        Pulpo pulpo;
        connect(&pulpo, &Pulpo::infoReady, [&] (const DB &track, const PULPO::RESPONSE &response)
        {
            for( auto info : response.keys())
                switch(info)
                {
                case PULPO::INFO::TAGS:
                {
                    if(!response[info].isEmpty())
                        for (auto context : response[info].keys())
                        {
                            if(!response[info][context].toMap().isEmpty())
                                for( auto tag :  response[info][context].toMap().keys() )
                                    this->connection.tagsArtist(track,tag, PULPO::CONTEXT_MAP[context]);

                            else if (!response[info][context].toStringList().isEmpty())
                                for( auto tag :  response[info][context].toStringList() )
                                    this->connection.tagsArtist(track,tag,PULPO::CONTEXT_MAP[context]);

                            else if (!response[info][context].toString().isEmpty())
                                this->connection.tagsArtist(track,response[info][context].toString(),PULPO::CONTEXT_MAP[context]);

                        }

                    break;
                }
                case PULPO::INFO::ARTWORK:
                {
                    if(!response[info].isEmpty())

                        if(!response[info][PULPO::CONTEXT::IMAGE].toByteArray().isEmpty())
                        {
                            connect(&pulpo, &Pulpo::artSaved, &connection, &CollectionDB::insertArtwork);
                            pulpo.saveArt(response[info][PULPO::CONTEXT::IMAGE].toByteArray(),CachePath);
                        }

                    break;
                }
                case PULPO::INFO::WIKI:
                {
                    if(!response[info].isEmpty())
                        for (auto context : response[info].keys())
                            this->connection.wikiArtist(track,response[info][context].toString());
                    break;
                }

                default: continue;
                }
        });

        pulpo.setOntology(PULPO::ONTOLOGY::ARTIST);
        pulpo.registerServices({PULPO::SERVICES::LastFm,PULPO::SERVICES::Spotify,PULPO::SERVICES::MusicBrainz});

        /* get all albums missing information */


        qDebug()<<"getting missing artworks";
        auto queryTxt = QString("SELECT %1 FROM %2 WHERE %3 = ''").arg(KEYMAP[KEY::ARTIST],
                TABLEMAP[TABLE::ARTISTS],KEYMAP[KEY::ARTWORK]);
        QSqlQuery query (queryTxt);
        pulpo.setInfo(PULPO::INFO::ARTWORK);
        for(auto track : connection.getDBData(query))
        {
            pulpo.feed(track,PULPO::RECURSIVE::OFF);
            if(!go) return;
        }

        emit this->artworkReady(TABLE::ARTISTS);

        //select artist from artists where  artist  not in (select album from albums_tags)
        qDebug()<<"getting missing tags";
        queryTxt =  QString("SELECT %1 FROM %2 WHERE %1 NOT IN ( SELECT %1 FROM %3 ) ").arg(KEYMAP[KEY::ARTIST],
                TABLEMAP[TABLE::ARTISTS],TABLEMAP[TABLE::ARTISTS_TAGS]);
        query.prepare(queryTxt);
        pulpo.setInfo(PULPO::INFO::TAGS);
        for(auto track : connection.getDBData(query))
        {
            pulpo.feed(track, PULPO::RECURSIVE::ON);
            if(!go) return;
        }

        qDebug()<<"getting missing wikis";
        queryTxt =  QString("SELECT %1 FROM %2 WHERE %3 = '' ").arg(KEYMAP[KEY::ARTIST],
                TABLEMAP[TABLE::ARTISTS],KEYMAP[KEY::WIKI]);
        query.prepare(queryTxt);
        pulpo.setInfo(PULPO::INFO::WIKI);
        for(auto track : connection.getDBData(query))
        {
            pulpo.feed(track, PULPO::RECURSIVE::ON);
            if(!go) return;
        }
    }


private:
    QThread t;
    CollectionDB connection;
    uint interval = 20000;
    bool go = false;
signals:

    void finished();
    void artworkReady(const TABLE &type);
};
}
