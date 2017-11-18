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
using namespace PULPO;

namespace Deamon {

static CollectionDB connection;


class Brain : public QObject
{
    Q_OBJECT

public:
    Brain() : QObject ()
    {
        this->moveToThread(&t);

        qRegisterMetaType<DB>("DB");
        qRegisterMetaType<TABLE>("TABLE");
        qRegisterMetaType<PULPO::RESPONSE>("PULPO::RESPONSE");

        connect(&this->pulpo, &Pulpo::infoReady, this, &Brain::connectionParser);

        t.start();
    }

    ~Brain()
    {
        qDebug()<<"Deleting Brainz obj";
        this->stop();
        connection.deleteLater();
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

    void setInfo(DB_LIST dataList, ONTOLOGY ontology, QList<SERVICES> services, INFO info, RECURSIVE recursive = RECURSIVE::ON, void (*cb)(DB) = nullptr)
    {
        this->pulpo.registerServices(services);
        this->pulpo.setOntology(ontology);
        this->pulpo.setInfo(info);

        if(dataList.isEmpty()) return;

        for(auto data : dataList)
        {
            if (cb != nullptr) cb(data);
            pulpo.feed(data,recursive);

            if(!go) return;
        }

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


    void connectionParser(DB track, RESPONSE response)
    {
        for( auto res : response.keys())
            switch(res)
            {
            case ONTOLOGY::ALBUM: this->parseAlbumInfo(track, response[res]); break;
            case ONTOLOGY::ARTIST: this->parseArtistInfo(track, response[res]); break;
            case ONTOLOGY::TRACK:  this->parseTrackInfo(track, response[res]); break;
            default: return;
            }
    }

    void parseAlbumInfo(DB &track, const INFO_K &response)
    {
        for( auto info : response.keys())
            switch(info)
            {
            case INFO::TAGS:
            {
                for (auto context : response[info].keys())

                    if(!response[info][context].toMap().isEmpty())
                        for( auto tag :  response[info][context].toMap().keys() )
                            connection.tagsAlbum(track,tag, CONTEXT_MAP[context]);

                    else if (!response[info][context].toStringList().isEmpty())
                        for( auto tag :  response[info][context].toStringList() )
                            connection.tagsAlbum(track,tag,CONTEXT_MAP[context]);

                    else if (!response[info][context].toString().isEmpty())
                        connection.tagsAlbum(track,response[info][context].toString(),CONTEXT_MAP[context]);
                break;
            }

            case INFO::ARTWORK:
            {
                if(!response[info].isEmpty())

                    if(!response[info][CONTEXT::IMAGE].toByteArray().isEmpty())
                    {
                        qDebug()<<"SAVING ARTWORK FOR: "<<track[KEY::ALBUM];
                        // connect(&connection, &CollectionDB::artworkInserted, this, &Brain::artworkReady);
                        saveArt(track, response[info][CONTEXT::IMAGE].toByteArray(), CachePath);
                        connection.insertArtwork(track);
                    }

                break;
            }

            case INFO::WIKI:
            {
                if(!response[info].isEmpty())
                    for (auto context : response[info].keys())
                        connection.wikiAlbum(track,response[info][context].toString());
                break;
            }

            default: continue;
            }
    }

    void parseArtistInfo(DB &track, const INFO_K &response)
    {
        for( auto info : response.keys())
            switch(info)
            {
            case INFO::TAGS:
            {
                if(!response[info].isEmpty())
                    for (auto context : response[info].keys())
                    {
                        if(!response[info][context].toMap().isEmpty())
                            for( auto tag :  response[info][context].toMap().keys() )
                                connection.tagsArtist(track,tag, CONTEXT_MAP[context]);

                        else if (!response[info][context].toStringList().isEmpty())
                            for( auto tag :  response[info][context].toStringList() )
                                connection.tagsArtist(track,tag,CONTEXT_MAP[context]);

                        else if (!response[info][context].toString().isEmpty())
                            connection.tagsArtist(track,response[info][context].toString(),CONTEXT_MAP[context]);

                    }

                break;
            }
            case INFO::ARTWORK:
            {
                if(!response[info].isEmpty())

                    if(!response[info][CONTEXT::IMAGE].toByteArray().isEmpty())
                    {
                        //                        connect(&connection, &CollectionDB::artworkInserted, this, &Brain::artworkReady);
                        saveArt(track,response[info][CONTEXT::IMAGE].toByteArray(),CachePath);
                        connection.insertArtwork(track);
                    }

                break;
            }
            case INFO::WIKI:
            {
                if(!response[info].isEmpty())
                    for (auto context : response[info].keys())
                        connection.wikiArtist(track,response[info][context].toString());
                break;
            }

            default: continue;
            }
    }

    void parseTrackInfo(DB &track, const INFO_K &response)
    {
        for( auto info : response.keys())
            switch(info)
            {
            case INFO::TAGS:
            {
                if(!response[info].isEmpty())
                {
                    qDebug()<<"CUSTOM PERFROMANCES"<<response[info][CONTEXT::TRACK_TEAM].toStringList();

                    for (auto context :response[info].keys())
                    {

                        if (!response[info][context].toStringList().isEmpty())
                            for( auto tag : response[info][context].toStringList() )
                                connection.tagsTrack(track,tag,CONTEXT_MAP[context]);

                        if (!response[info][context].toString().isEmpty())
                            connection.tagsTrack(track,response[info][context].toString(),CONTEXT_MAP[context]);

                    }
                }

                break;
            }

            case INFO::WIKI:
            {
                if(!response[info].isEmpty())
                    if (!response[info][CONTEXT::WIKI].toString().isEmpty())
                        connection.wikiTrack(track,response[info][CONTEXT::WIKI].toString());
                break;
            }

            case INFO::ARTWORK:
            {
                if(!response[info].isEmpty())

                    if(!response[info][CONTEXT::IMAGE].toByteArray().isEmpty())
                    {
                        //                        connect(&connection, &CollectionDB::artworkInserted, this, &Brain::artworkReady);
                        saveArt(track, response[info][CONTEXT::IMAGE].toByteArray(),CachePath);
                        connection.insertArtwork(track);
                    }

                break;
            }

            case INFO::METADATA:
            {
                for (auto context :response[info].keys())
                    switch(context)
                    {
                    case CONTEXT::ALBUM_TITLE:
                    {
                        qDebug()<<"SETTING TRACK MISSING METADATA";

                        TagInfo tag(track[KEY::URL]);

                        if(!response[info][context].toString().isEmpty())
                        {
                            tag.setAlbum(response[info][context].toString());
                            connection.albumTrack(track, response[info][context].toString());
                        }

                        break;
                    }

                    case CONTEXT::TRACK_NUMBER:
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

            case INFO::LYRICS:
            {
                if(!response[info][CONTEXT::LYRIC].toString().isEmpty())
                    connection.lyricsTrack(track,response[info][CONTEXT::LYRIC].toString());
                break;
            }
            default: continue;

            }
    }

    void trackInfo()
    {
        if(!go) return;

        auto ontology = ONTOLOGY::TRACK;
        auto services = {SERVICES::LyricWikia};

        qDebug()<<"getting missing track lyrics";
        auto queryTxt =  QString("SELECT %1, %2, %3 FROM %4 WHERE %5 = ''").arg(KEYMAP[KEY::URL],
                KEYMAP[KEY::TITLE],
                KEYMAP[KEY::ARTIST],
                TABLEMAP[TABLE::TRACKS],
                KEYMAP[KEY::LYRICS]);
        QSqlQuery query (queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::LYRICS, RECURSIVE::OFF, [](DB track)
        {
            connection.lyricsTrack(track,SLANG[W::NONE]);
        });

        services = {SERVICES::LastFm,SERVICES::Spotify,SERVICES::MusicBrainz,SERVICES::Genius};

        qDebug()<<"getting missing track artwork";
        //select url, title, album, artist from tracks t inner join albums a on a.album=t.album and a.artist=t.artist where a.artwork = ''
        queryTxt =  QString("SELECT DISTINCT t.%1, t.%2, t.%3, t.%4 FROM %5 t INNER JOIN %6 a ON a.%3 = t.%3 AND a.%4 = t.%4  WHERE a.%7 = '' GROUP BY a.%3, a.%4 ").arg(KEYMAP[KEY::URL],
                KEYMAP[KEY::TITLE],
                KEYMAP[KEY::ARTIST],
                KEYMAP[KEY::ALBUM],
                TABLEMAP[TABLE::TRACKS],
                TABLEMAP[TABLE::ALBUMS],
                KEYMAP[KEY::ARTWORK]);
        query.prepare(queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::ARTWORK, RECURSIVE::OFF, [](DB track)
        {
            connection.insertArtwork(track);
        });


        qDebug()<<"getting missing track tags";
        // select title, artist, album from tracks t where url not in (select url from tracks_tags)
        queryTxt =  QString("SELECT %1, %2, %3, %4 FROM %5 WHERE %1 NOT IN ( SELECT %1 FROM %6 )").arg(KEYMAP[KEY::URL],
                KEYMAP[KEY::TITLE],
                KEYMAP[KEY::ARTIST],
                KEYMAP[KEY::ALBUM],
                TABLEMAP[TABLE::TRACKS],
                TABLEMAP[TABLE::TRACKS_TAGS]);
        query.prepare(queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::TAGS, RECURSIVE::ON, nullptr);

        qDebug()<<"getting missing track wikis";
        queryTxt =  QString("SELECT %1, %2, %3, %4 FROM %5 WHERE %6 = ''").arg(KEYMAP[KEY::URL],KEYMAP[KEY::TITLE],
                KEYMAP[KEY::ARTIST],KEYMAP[KEY::ALBUM],TABLEMAP[TABLE::TRACKS],KEYMAP[KEY::WIKI]);
        query.prepare(queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::WIKI, RECURSIVE::OFF, [](DB track)
        {
            connection.wikiTrack(track,SLANG[W::NONE]);
        });

        //        auto queryTxt =  QString("SELECT %1, %2, %3 FROM %4 WHERE %3 = 'UNKNOWN' GROUP BY %2, a.%3 ").arg(KEYMAP[KEY::TITLE],
        //                KEYMAP[KEY::ARTIST],KEYMAP[KEY::ALBUM],TABLEMAP[TABLE::TRACKS]);
        //        QSqlQuery query (queryTxt);
        //        pulpo.setInfo(INFO::METADATA);
        //        for(auto track : connection.getDBData(query))
        //        {
        //            qDebug()<<"UNKOWN TRACK TITLE:"<<track[KEY::TITLE];
        //            pulpo.feed(track,RECURSIVE::OFF);
        //            if(!go) return;
        //        }

        // emit this->done(TABLE::TRACKS);
        emit this->done(TABLE::ALBUMS);

    }

    void albumInfo()
    {
        if(!go) return;

        auto services = {SERVICES::LastFm,SERVICES::Spotify,SERVICES::MusicBrainz};
        auto ontology = ONTOLOGY::ALBUM;

        qDebug()<<"getting missing album artworks";
        auto queryTxt = QString("SELECT %1, %2 FROM %3 WHERE %4 = ''").arg(KEYMAP[KEY::ALBUM],
                KEYMAP[KEY::ARTIST],TABLEMAP[TABLE::ALBUMS],KEYMAP[KEY::ARTWORK]);
        QSqlQuery query (queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::ARTWORK, RECURSIVE::OFF, nullptr);


        //select album, artist from albums where  album  not in (select album from albums_tags) and artist  not in (select  artist from albums_tags)
        qDebug()<<"getting missing album tags";
        queryTxt =  QString("SELECT %1, %2 FROM %3 WHERE %1 NOT IN ( SELECT %1 FROM %4 ) AND %2 NOT IN ( SELECT %2 FROM %4 )").arg(KEYMAP[KEY::ALBUM],
                KEYMAP[KEY::ARTIST],TABLEMAP[TABLE::ALBUMS],TABLEMAP[TABLE::ALBUMS_TAGS]);
        query.prepare(queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::TAGS, RECURSIVE::ON, nullptr);


        qDebug()<<"getting missing album wikis";
        queryTxt =  QString("SELECT %1, %2 FROM %3 WHERE %4 = '' ").arg(KEYMAP[KEY::ALBUM],
                KEYMAP[KEY::ARTIST],TABLEMAP[TABLE::ALBUMS],KEYMAP[KEY::WIKI]);
        query.prepare(queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::WIKI, RECURSIVE::OFF, [](DB track)
        {
            connection.wikiAlbum(track,SLANG[W::NONE]);
        });

        emit this->done(TABLE::ALBUMS);


    }

    void artistInfo()
    {
        if(!go) return;

        auto services = {SERVICES::LastFm,SERVICES::Spotify,SERVICES::MusicBrainz, SERVICES::Genius};
        auto ontology = ONTOLOGY::ARTIST;


        qDebug()<<"getting missing artist artworks";
        auto queryTxt = QString("SELECT %1 FROM %2 WHERE %3 = ''").arg(KEYMAP[KEY::ARTIST],
                TABLEMAP[TABLE::ARTISTS],KEYMAP[KEY::ARTWORK]);
        QSqlQuery query (queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::ARTWORK, RECURSIVE::OFF, nullptr);

        //select artist from artists where  artist  not in (select album from albums_tags)
        qDebug()<<"getting missing artist tags";
        queryTxt =  QString("SELECT %1 FROM %2 WHERE %1 NOT IN ( SELECT %1 FROM %3 ) ").arg(KEYMAP[KEY::ARTIST],
                TABLEMAP[TABLE::ARTISTS],TABLEMAP[TABLE::ARTISTS_TAGS]);
        query.prepare(queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::TAGS, RECURSIVE::ON, nullptr);


        qDebug()<<"getting missing artist wikis";
        queryTxt =  QString("SELECT %1 FROM %2 WHERE %3 = '' ").arg(KEYMAP[KEY::ARTIST],
                TABLEMAP[TABLE::ARTISTS],KEYMAP[KEY::WIKI]);
        query.prepare(queryTxt);
        this->setInfo(connection.getDBData(query), ontology, services, INFO::WIKI, RECURSIVE::OFF,  [](DB track)
        {
            connection.wikiArtist(track,SLANG[W::NONE]);
        });

        emit this->done(TABLE::ARTISTS);

    }


private:
    QThread t;
    Pulpo pulpo;
    uint interval = 20000;
    bool go = false;
signals:

    void finished();
    void artworkReady(const DB &track);

    void done(const TABLE &type);
};
}
