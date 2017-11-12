#ifndef BRAIN_H
#define BRAIN_H

#endif // BRAIN_H

/* This deamon keeps on running while there are missing information about a track,
 *  it should have the option to turn it off, but the main idea is to here have the
 * brains of the app and collection. so this must be a very good a neat implementation */


#include <QObject>
#include <QThread>

#include "baeUtils.h"
#include "collectionDB.h"
#include "pulpo/pulpo.h"

namespace Deamon {

class Brain : public QObject
{
    Q_OBJECT

public:
    Brain() : QObject ()
    {
        this->moveToThread(&t);
        qRegisterMetaType<Bae::DB>("Bae::DB");
        qRegisterMetaType<Bae::DBTables>("Bae::DBTables");
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
            //t.msleep(this->interval);
        }

        emit this->finished();
        this->go = false;

    }

    void trackInfo()
    {
        if(!go) return;
        Pulpo pulpo;
        connect(&pulpo, &Pulpo::infoReady, [&] (const Bae::DB &track, const Pulpo::RESPONSE &response)
        {
            for( auto info : response.keys())
                switch(info)
                {
                case Pulpo::INFO::TAGS:
                {
                    if(!response[info].isEmpty())
                    {
                        for (auto context :response[info].keys())
                            switch(context)
                            {
                            case Pulpo::CONTEXT::STAT:
                            {
                                if (!response[info][context].toStringList().isEmpty())
                                    for( auto stat : response[info][context].toStringList() )
                                        this->connection.tagsTrack(track,stat,pulpo.CONTEXT_MAP[context]);

                                if (!response[info][context].toString().isEmpty())
                                    this->connection.tagsTrack(track,response[info][context].toString(),pulpo.CONTEXT_MAP[context]);
                                break;
                            }
                            case Pulpo::CONTEXT::TAG:
                            {
                                if (!response[info][context].toStringList().isEmpty())
                                    for(auto tag : response[info][context].toStringList())
                                        this->connection.tagsTrack(track,tag,pulpo.CONTEXT_MAP[context]);

                                if (!response[info][context].toString().isEmpty())
                                    this->connection.tagsTrack(track,response[info][context].toString(),pulpo.CONTEXT_MAP[context]);

                                break;
                            }
                            default: continue;
                            }
                    }

                    break;
                }
                case Pulpo::INFO::WIKI:
                {
                    if(!response[info].isEmpty())
                        if (!response[info][Pulpo::CONTEXT::WIKI].toString().isEmpty())
                            this->connection.wikiTrack(track,response[info][Pulpo::CONTEXT::WIKI].toString());
                    break;
                }

                case Pulpo::INFO::ARTWORK:
                {
                    qDebug()<<"GETTING ARTWORK FROM TRACK TITLE";
                    connect(&connection, &CollectionDB::artworkInserted,[this](const Bae::DB &albumMap)
                    {
                        emit this->artworkReady(albumMap);
                    });

                    connect(&pulpo, &Pulpo::artSaved, &connection, &CollectionDB::insertArtwork);

                    if(!response[info][Pulpo::CONTEXT::IMAGE].toByteArray().isEmpty())
                        pulpo.saveArt(response[info][Pulpo::CONTEXT::IMAGE].toByteArray(),Bae::CachePath);

                    break;
                }

                default: continue;
                }
        });

        pulpo.setOntology(Pulpo::ONTOLOGY::TRACK);
        pulpo.registerServices({Pulpo::SERVICES::LastFm,Pulpo::SERVICES::Spotify});

        //select url, title, album, artist from tracks t inner join albums a on a.album=t.album and a.artist=t.artist where a.artwork = ''
        auto queryTxt =  QString("SELECT DISTINCT t.%1, t.%2, t.%3, t.%4 FROM %5 t INNER JOIN %6 a ON a.%3 = t.%3 AND a.%4 = t.%4  WHERE a.%7 = '' GROUP BY a.%3, a.%4 ").arg(Bae::DBColsMap[Bae::DBCols::URL],Bae::DBColsMap[Bae::DBCols::TITLE],
                Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBColsMap[Bae::DBCols::ALBUM],Bae::DBTablesMap[Bae::DBTables::TRACKS],Bae::DBTablesMap[Bae::DBTables::ALBUMS],
                Bae::DBColsMap[Bae::DBCols::ARTWORK]);
        QSqlQuery query (queryTxt);
        pulpo.setInfo(Pulpo::INFO::ARTWORK);
        for(auto track : connection.getTrackData(query))
        {
            qDebug()<<"TRACK TITLE:"<<track[Bae::DBCols::TITLE];
            pulpo.feed(track,Pulpo::RECURSIVE::OFF);
            if(!go) return;
        }

        // select title, artist, album from tracks t where url not in (select url from tracks_tags)
        queryTxt =  QString("SELECT %1, %2, %3, %5 FROM %4 WHERE %5 NOT IN ( SELECT %5 FROM %6 )").arg(Bae::DBColsMap[Bae::DBCols::TITLE],
                Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBColsMap[Bae::DBCols::ALBUM],Bae::DBTablesMap[Bae::DBTables::TRACKS],Bae::DBColsMap[Bae::DBCols::URL],
                Bae::DBTablesMap[Bae::DBTables::TRACKS_TAGS]);
        query.prepare(queryTxt);

        pulpo.setInfo(Pulpo::INFO::TAGS);
        for(auto track : connection.getTrackData(query))
        {
            pulpo.feed(track,Pulpo::RECURSIVE::ON);
            if(!go) return;
        }


        qDebug()<<"getting missing TRACK wikis";
        queryTxt =  QString("SELECT %1, %2, %3, %4 FROM %5 WHERE %6 = '' OR %6 IS NULL").arg(Bae::DBColsMap[Bae::DBCols::URL],Bae::DBColsMap[Bae::DBCols::TITLE],
                Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBColsMap[Bae::DBCols::ALBUM],Bae::DBTablesMap[Bae::DBTables::TRACKS],Bae::DBColsMap[Bae::DBCols::WIKI]);
        query.prepare(queryTxt);
        pulpo.setInfo(Pulpo::INFO::WIKI);
        for(auto track : connection.getTrackData(query))
        {
            qDebug()<<"getting missing TRACK wikis";

            pulpo.feed(track, Pulpo::RECURSIVE::OFF);
            if(!go) return;
        }
    }

    void albumInfo()
    {
        if(!go) return;

        /*setup pulpo in place*/
        Pulpo pulpo;
        connect(&pulpo, &Pulpo::infoReady, [&] (const Bae::DB &track, const Pulpo::RESPONSE &response)
        {
            for( auto info : response.keys())
                switch(info)
                {
                case Pulpo::INFO::TAGS:
                {
                    for (auto context : response[info].keys())

                        if(!response[info][context].toMap().isEmpty())
                            for( auto tag :  response[info][context].toMap().keys() )
                                this->connection.tagsAlbum(track,tag, pulpo.CONTEXT_MAP[context]);

                        else if (!response[info][context].toStringList().isEmpty())
                            for( auto tag :  response[info][context].toStringList() )
                                this->connection.tagsAlbum(track,tag,pulpo.CONTEXT_MAP[context]);

                        else if (!response[info][context].toString().isEmpty())
                            this->connection.tagsAlbum(track,response[info][context].toString(),pulpo.CONTEXT_MAP[context]);


                    break;
                }
                case Pulpo::INFO::ARTWORK:
                {
                    connect(&connection, &CollectionDB::artworkInserted,[this](const Bae::DB &albumMap)
                    {
                        emit this->artworkReady(albumMap);
                    });
                    connect(&pulpo, &Pulpo::artSaved, &connection, &CollectionDB::insertArtwork);

                    if(!response[info].isEmpty())
                        for (auto context : response[info].keys())
                            pulpo.saveArt(response[info][context].toByteArray(),Bae::CachePath);

                    break;
                }
                case Pulpo::INFO::WIKI:
                {
                    if(!response[info].isEmpty())
                        for (auto context : response[info].keys())
                            this->connection.wikiAlbum(track,response[info][context].toString());
                    break;
                }
                default: continue;
                }
        });

        pulpo.setOntology(Pulpo::ONTOLOGY::ALBUM);
        pulpo.registerServices({Pulpo::SERVICES::LastFm,Pulpo::SERVICES::Spotify});

        /* get all albums missing information */
        //select album, artist from albums where  album  not in (select album from albums_tags) and artist  not in (select  artist from albums_tags)

        qDebug()<<"getting missing artworks";
        auto queryTxt = QString("SELECT %1, %2 FROM %3 WHERE %4 = ''").arg(Bae::DBColsMap[Bae::DBCols::ALBUM],
                Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBTablesMap[Bae::DBTables::ALBUMS],Bae::DBColsMap[Bae::DBCols::ARTWORK]);
        QSqlQuery query (queryTxt);
        pulpo.setInfo(Pulpo::INFO::ARTWORK);
        for(auto track : connection.getTrackData(query))
        {
            pulpo.feed(track, Pulpo::RECURSIVE::OFF);
            if(!go) return;
        }

        qDebug()<<"getting missing tags";
        queryTxt =  QString("SELECT %1, %2 FROM %3 WHERE %1 NOT IN ( SELECT %1 FROM %4 ) AND %2 NOT IN ( SELECT %2 FROM %4 )").arg(Bae::DBColsMap[Bae::DBCols::ALBUM],
                Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBTablesMap[Bae::DBTables::ALBUMS],Bae::DBTablesMap[Bae::DBTables::ALBUMS_TAGS]);
        query.prepare(queryTxt);
        pulpo.setInfo(Pulpo::INFO::TAGS);
        for(auto track : connection.getTrackData(query))
        {
            pulpo.feed(track,Pulpo::RECURSIVE::ON);
            if(!go) return;
        }

        qDebug()<<"getting missing wikis";
        queryTxt =  QString("SELECT %1, %2 FROM %3 WHERE %4 = '' ").arg(Bae::DBColsMap[Bae::DBCols::ALBUM],
                Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBTablesMap[Bae::DBTables::ALBUMS],Bae::DBColsMap[Bae::DBCols::WIKI]);
        query.prepare(queryTxt);
        pulpo.setInfo(Pulpo::INFO::WIKI);
        for(auto track : connection.getTrackData(query))
        {
            pulpo.feed(track, Pulpo::RECURSIVE::ON);
            if(!go) return;
        }
    }

    void artistInfo()
    {
        if(!go) return;

        /*setup pulpo in place*/
        Pulpo pulpo;
        connect(&pulpo, &Pulpo::infoReady, [&] (const Bae::DB &track, const Pulpo::RESPONSE &response)
        {
            for( auto info : response.keys())
                switch(info)
                {
                case Pulpo::INFO::TAGS:
                {
                    if(!response[info].isEmpty())
                    {

                        for (auto context : response[info].keys())

                            if(!response[info][context].toMap().isEmpty())
                                for( auto tag :  response[info][context].toMap().keys() )
                                    this->connection.tagsArtist(track,tag, pulpo.CONTEXT_MAP[context]);

                            else if (!response[info][context].toStringList().isEmpty())
                                for( auto tag :  response[info][context].toStringList() )
                                    this->connection.tagsArtist(track,tag,pulpo.CONTEXT_MAP[context]);

                            else if (!response[info][context].toString().isEmpty())
                                this->connection.tagsArtist(track,response[info][context].toString(),pulpo.CONTEXT_MAP[context]);
                    }

                    break;
                }
                case Pulpo::INFO::ARTWORK:
                {
                    connect(&connection, &CollectionDB::artworkInserted,[this](const Bae::DB &albumMap)
                    {
                        emit this->artworkReady(albumMap);
                    });
                    connect(&pulpo, &Pulpo::artSaved, &connection, &CollectionDB::insertArtwork);

                    if(!response[info].isEmpty())
                        for (auto context : response[info].keys())
                            pulpo.saveArt(response[info][context].toByteArray(),Bae::CachePath);


                    break;
                }
                case Pulpo::INFO::WIKI:
                {
                    if(!response[info].isEmpty())
                        for (auto context : response[info].keys())
                            this->connection.wikiArtist(track,response[info][context].toString());
                    break;
                }

                default: continue;
                }
        });

        pulpo.setOntology(Pulpo::ONTOLOGY::ARTIST);
        pulpo.registerServices({Pulpo::SERVICES::LastFm,Pulpo::SERVICES::Spotify});

        /* get all albums missing information */


        qDebug()<<"getting missing artworks";
        auto queryTxt = QString("SELECT %1 FROM %2 WHERE %3 = ''").arg(Bae::DBColsMap[Bae::DBCols::ARTIST],
                Bae::DBTablesMap[Bae::DBTables::ARTISTS],Bae::DBColsMap[Bae::DBCols::ARTWORK]);
        QSqlQuery query (queryTxt);
        pulpo.setInfo(Pulpo::INFO::ARTWORK);
        for(auto track : connection.getTrackData(query))
        {
            pulpo.feed(track,Pulpo::RECURSIVE::OFF);
            if(!go) return;
        }


        //select artist from artists where  artist  not in (select album from albums_tags)
        qDebug()<<"getting missing tags";
        queryTxt =  QString("SELECT %1 FROM %2 WHERE %1 NOT IN ( SELECT %1 FROM %3 ) ").arg(Bae::DBColsMap[Bae::DBCols::ARTIST],
                Bae::DBTablesMap[Bae::DBTables::ARTISTS],Bae::DBTablesMap[Bae::DBTables::ARTISTS_TAGS]);
        query.prepare(queryTxt);
        pulpo.setInfo(Pulpo::INFO::TAGS);
        for(auto track : connection.getTrackData(query))
        {
            pulpo.feed(track, Pulpo::RECURSIVE::ON);
            if(!go) return;
        }

        qDebug()<<"getting missing wikis";
        queryTxt =  QString("SELECT %1 FROM %2 WHERE %3 = '' ").arg(Bae::DBColsMap[Bae::DBCols::ARTIST],
                Bae::DBTablesMap[Bae::DBTables::ARTISTS],Bae::DBColsMap[Bae::DBCols::WIKI]);
        query.prepare(queryTxt);
        pulpo.setInfo(Pulpo::INFO::WIKI);
        for(auto track : connection.getTrackData(query))
        {
            pulpo.feed(track, Pulpo::RECURSIVE::ON);
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
    void artworkReady(const Bae::DB &album);
};
}
