#include "brain.h"
#include "../services/local/taginfo.h"
#include "../db/collectionDB.h"


Brain::Brain() : QObject ()
{
    qRegisterMetaType<DB>("DB");
    qRegisterMetaType<TABLE>("TABLE");
    qRegisterMetaType<PULPO::RESPONSE>("PULPO::RESPONSE");
    this->con = new CollectionDB(this);
    connect(&this->pulpo, &Pulpo::infoReady, this, &Brain::connectionParser);

    this->moveToThread(&t);
    this->t.start();
}

Brain::~Brain()
{
    qDebug()<<"Deleting Brainz obj";
    this->stop();
}

void Brain::start()
{
    if(this->isRunning()) this->go = false;
    this->go = true;
    QMetaObject::invokeMethod(this, "synapse");

}

void Brain::stop()
{
    this->go = false;
    this->t.quit();
    this->t.wait();
}

bool Brain::isRunning() const
{
    return this->go;
}

void Brain::setInterval(const uint &value)
{
    qDebug()<< "reseting the interval brainz";
    this->interval = value*60000;
}

void Brain::setInfo(DB_LIST dataList, ONTOLOGY ontology, QList<SERVICES> services, INFO info, RECURSIVE recursive, void (*cb)(DB))
{
    this->pulpo.registerServices(services);
    this->pulpo.setOntology(ontology);
    this->pulpo.setInfo(info);

    if(dataList.isEmpty()) return;

    for(auto data : dataList)
    {
        if (cb != nullptr) cb(data);
        this->pulpo.feed(data, recursive);

        this->t.msleep(500);
        if(!go) return;
    }
}

void Brain::synapse()
{
    if(this->go)
    {
        this->albumInfo();
        this->artistInfo();
        this->trackInfo();
    }

    emit this->finished();
    this->go = false;
}

void Brain::connectionParser(DB track, RESPONSE response)
{
    for(auto res : response.keys())
        switch(res)
        {
            case ONTOLOGY::ALBUM: this->parseAlbumInfo(track, response[res]); break;
            case ONTOLOGY::ARTIST: this->parseArtistInfo(track, response[res]); break;
            case ONTOLOGY::TRACK:  this->parseTrackInfo(track, response[res]); break;
            default: return;
        }
}

void Brain::parseAlbumInfo(DB &track, const INFO_K &response)
{

    for(auto info : response.keys())
        switch(info)
        {
            case PULPO::INFO::TAGS:
            {
                for(auto context : response[info].keys())

                    if(!response[info][context].toMap().isEmpty())
                    {
                        for(auto tag : response[info][context].toMap().keys() )
                            con->tagsAlbum(track, tag, CONTEXT_MAP[context]);

                    }else if (!response[info][context].toStringList().isEmpty())
                    {
                        for(auto tag : response[info][context].toStringList() )
                            con->tagsAlbum(track, tag, CONTEXT_MAP[context]);

                    } else if (!response[info][context].toString().isEmpty())
                    {
                        con->tagsAlbum(track, response[info][context].toString(), CONTEXT_MAP[context]);
                    }
                break;
            }

            case PULPO::INFO::ARTWORK:
            {
                if(!response[info].isEmpty())

                    if(!response[info][CONTEXT::IMAGE].toByteArray().isEmpty())
                    {
                        qDebug()<<"SAVING ARTWORK FOR: " << track[KEY::ALBUM];
                        BAE::saveArt(track, response[info][CONTEXT::IMAGE].toByteArray(), BAE::CachePath);
                        con->insertArtwork(track);
                    }

                break;
            }

            case PULPO::INFO::WIKI:
            {
                if(!response[info].isEmpty())
                    for (auto context : response[info].keys())
                        con->wikiAlbum(track, response[info][context].toString());
                break;
            }

            default: continue;
        }
}

void Brain::parseArtistInfo(DB &track, const INFO_K &response)
{
    for(auto info : response.keys())
    {
        switch(info)
        {
            case PULPO::INFO::TAGS:
            {
                if(!response[info].isEmpty())
                {
                    for(auto context : response[info].keys())
                    {
                        if(!response[info][context].toMap().isEmpty())
                        {
                            for(auto tag : response[info][context].toMap().keys() )
                                con->tagsArtist(track, tag, CONTEXT_MAP[context]);

                        }else if(!response[info][context].toStringList().isEmpty())
                        {
                            for(auto tag : response[info][context].toStringList() )
                                con->tagsArtist(track, tag, CONTEXT_MAP[context]);

                        }else if(!response[info][context].toString().isEmpty())
                        {
                            con->tagsArtist(track, response[info][context].toString(), CONTEXT_MAP[context]);
                        }
                    }

                } break;
            }

            case PULPO::INFO::ARTWORK:
            {
                if(!response[info].isEmpty())
                {
                    if(!response[info][CONTEXT::IMAGE].toByteArray().isEmpty())
                    {
                        BAE::saveArt(track, response[info][CONTEXT::IMAGE].toByteArray(), BAE::CachePath);
                        con->insertArtwork(track);
                    }
                }

                break;
            }

            case PULPO::INFO::WIKI:
            {
                if(!response[info].isEmpty())
                {
                    for (auto context : response[info].keys())
                        con->wikiArtist(track, response[info][context].toString());
                }

                break;
            }

            default: continue;
        }
    }
}

void Brain::parseTrackInfo(DB &track, const INFO_K &response)
{
    for(auto info : response.keys())
        switch(info)
        {
            case PULPO::INFO::TAGS:
            {
                if(!response[info].isEmpty())
                {
                    for(auto context : response[info].keys())
                    {
                        if (!response[info][context].toStringList().isEmpty())
                        {
                            for(auto tag : response[info][context].toStringList() )
                                con->tagsTrack(track, tag, CONTEXT_MAP[context]);
                        }

                        if (!response[info][context].toString().isEmpty())
                            con->tagsTrack(track, response[info][context].toString(), CONTEXT_MAP[context]);
                    }
                }

                break;
            }

            case PULPO::INFO::WIKI:
            {
                if(!response[info].isEmpty())
                {
                    if (!response[info][CONTEXT::WIKI].toString().isEmpty())
                        con->wikiTrack(track, response[info][CONTEXT::WIKI].toString());

                }

                break;
            }

            case PULPO::INFO::ARTWORK:
            {
                if(!response[info].isEmpty())
                {
                    if(!response[info][CONTEXT::IMAGE].toByteArray().isEmpty())
                    {
                        BAE::saveArt(track, response[info][CONTEXT::IMAGE].toByteArray(),CachePath);
                        con->insertArtwork(track);
                    }
                }

                break;
            }

            case PULPO::INFO::METADATA:
            {
                for(auto context :response[info].keys())
                {
                    switch(context)
                    {
                        case CONTEXT::ALBUM_TITLE:
                        {
                            qDebug()<<"SETTING TRACK MISSING METADATA";

                            TagInfo tag(track[KEY::URL]);
                            if(!response[info][context].toString().isEmpty())
                            {
                                tag.setAlbum(response[info][context].toString());
                                con->albumTrack(track, response[info][context].toString());
                            }

                            break;
                        }

                        case CONTEXT::TRACK_NUMBER:
                        {
                            TagInfo tag(track[KEY::URL]);
                            if(!response[info][context].toString().isEmpty())
                                tag.setTrack(response[info][context].toInt());

                            break;
                        }

                        default: continue;
                    }
                }

                break;
            }

            case PULPO::INFO::LYRICS:
            {
                if(!response[info][CONTEXT::LYRIC].toString().isEmpty())
                    con->lyricsTrack(track, response[info][CONTEXT::LYRIC].toString());
                break;
            }

            default: continue;
        }
}

void Brain::trackInfo()
{
    if(!this->go) return;

    auto ontology = PULPO::ONTOLOGY::TRACK;
    auto services = {PULPO::SERVICES::LyricWikia, PULPO::SERVICES::Genius};

    qDebug()<<"getting missing track lyrics";
    auto queryTxt = QString("SELECT %1, %2, %3 FROM %4 WHERE %5 = ''").arg(KEYMAP[KEY::URL],
            KEYMAP[KEY::TITLE],
            KEYMAP[KEY::ARTIST],
            TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::LYRICS]);

    this->setInfo(con->getDBData(queryTxt), ontology, services, PULPO::INFO::LYRICS, PULPO::RECURSIVE::OFF, [](DB track)
    {
        CollectionDB conn(nullptr);
        conn.lyricsTrack(track, SLANG[W::NONE]);
    });

    services = {PULPO::SERVICES::LastFm, PULPO::SERVICES::Spotify, PULPO::SERVICES::MusicBrainz, PULPO::SERVICES::Genius};

    qDebug()<<"getting missing track artwork";
    //select url, title, album, artist from tracks t inner join albums a on a.album=t.album and a.artist=t.artist where a.artwork = ''
    queryTxt =  QString("SELECT DISTINCT t.%1, t.%2, t.%3, t.%4 FROM %5 t INNER JOIN %6 a ON a.%3 = t.%3 AND a.%4 = t.%4  WHERE a.%7 = '' GROUP BY a.%3, a.%4 ").arg(KEYMAP[KEY::URL],
            KEYMAP[KEY::TITLE],
            KEYMAP[KEY::ARTIST],
            KEYMAP[KEY::ALBUM],
            TABLEMAP[TABLE::TRACKS],
            TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::ARTWORK]);

    auto artworks = con->getDBData(queryTxt);
    this->setInfo(artworks, ontology, services, PULPO::INFO::ARTWORK, PULPO::RECURSIVE::OFF, [](DB track)
    {
        CollectionDB conn(nullptr);
        conn.insertArtwork(track);
    });

    if(!artworks.isEmpty())
        emit this->done(TABLE::ALBUMS);

    qDebug()<<"getting missing track tags";
    // select title, artist, album from tracks t where url not in (select url from tracks_tags)
    queryTxt = QString("SELECT %1, %2, %3, %4 FROM %5 WHERE %1 NOT IN ( SELECT %1 FROM %6 )").arg(KEYMAP[KEY::URL],
            KEYMAP[KEY::TITLE],
            KEYMAP[KEY::ARTIST],
            KEYMAP[KEY::ALBUM],
            TABLEMAP[TABLE::TRACKS],
            TABLEMAP[TABLE::TRACKS_TAGS]);
    this->setInfo(con->getDBData(queryTxt), ontology, services, PULPO::INFO::TAGS, RECURSIVE::ON, nullptr);

    qDebug()<<"getting missing track wikis";
    queryTxt = QString("SELECT %1, %2, %3, %4 FROM %5 WHERE %6 = ''").arg(KEYMAP[KEY::URL],
            KEYMAP[KEY::TITLE],
            KEYMAP[KEY::ARTIST],
            KEYMAP[KEY::ALBUM],
            TABLEMAP[TABLE::TRACKS],
            KEYMAP[KEY::WIKI]);
    this->setInfo(con->getDBData(queryTxt), ontology, services, PULPO::INFO::WIKI, RECURSIVE::OFF, [](DB track)
    {
        CollectionDB conn(nullptr);
        conn.wikiTrack(track, SLANG[W::NONE]);
    });

    //        auto queryTxt =  QString("SELECT %1, %2, %3 FROM %4 WHERE %3 = 'UNKNOWN' GROUP BY %2, a.%3 ").arg(KEYMAP[KEY::TITLE],
    //                KEYMAP[KEY::ARTIST],KEYMAP[KEY::ALBUM],TABLEMAP[TABLE::TRACKS]);
    //        QSqlQuery query (queryTxt);
    //        pulpo.setInfo(INFO::METADATA);
    //        for(auto track : con->getDBData(query))
    //        {
    //            qDebug()<<"UNKOWN TRACK TITLE:"<<track[KEY::TITLE];
    //            pulpo.feed(track,RECURSIVE::OFF);
    //            if(!go) return;
    //        }

    // emit this->done(TABLE::TRACKS);


}

void Brain::albumInfo()
{
    if(!this->go) return;

    auto services = {PULPO::SERVICES::LastFm, PULPO::SERVICES::Spotify, PULPO::SERVICES::MusicBrainz};
    auto ontology = PULPO::ONTOLOGY::ALBUM;

    qDebug()<<"getting missing album artworks";
    auto queryTxt = QString("SELECT %1, %2 FROM %3 WHERE %4 = ''").arg(KEYMAP[KEY::ALBUM],
            KEYMAP[KEY::ARTIST], TABLEMAP[TABLE::ALBUMS], KEYMAP[KEY::ARTWORK]);
    auto artworks = con->getDBData(queryTxt);

    /* BEFORE FETCHING ONLINE LOOK UP IN THE CACHE FOR THE IMAGE */
    for(auto album : artworks)
        if(BAE::artworkCache(album, KEY::ALBUM))
            con->insertArtwork(album);

    artworks = con->getDBData(queryTxt);
    this->setInfo(artworks, ontology, services, PULPO::INFO::ARTWORK, PULPO::RECURSIVE::OFF, nullptr);

    //select album, artist from albums where  album  not in (select album from albums_tags) and artist  not in (select  artist from albums_tags)
    qDebug()<<"getting missing album tags";
    queryTxt =  QString("SELECT %1, %2 FROM %3 WHERE %1 NOT IN ( SELECT %1 FROM %4 ) AND %2 NOT IN ( SELECT %2 FROM %4 )").arg(KEYMAP[KEY::ALBUM],
            KEYMAP[KEY::ARTIST],
            TABLEMAP[TABLE::ALBUMS],
            TABLEMAP[TABLE::ALBUMS_TAGS]);
    this->setInfo(con->getDBData(queryTxt), ontology, services, PULPO::INFO::TAGS, PULPO::RECURSIVE::ON, nullptr);

    qDebug()<<"getting missing album wikis";
    queryTxt =  QString("SELECT %1, %2 FROM %3 WHERE %4 = '' ").arg(KEYMAP[KEY::ALBUM],
            KEYMAP[KEY::ARTIST],
            TABLEMAP[TABLE::ALBUMS],
            KEYMAP[KEY::WIKI]);
    this->setInfo(con->getDBData(queryTxt), ontology, services, PULPO::INFO::WIKI, PULPO::RECURSIVE::OFF, [](DB track)
    {
        CollectionDB conn(nullptr);
        conn.wikiAlbum(track, SLANG[W::NONE]);
    });

    emit this->done(TABLE::ALBUMS);
}

void Brain::artistInfo()
{
    if(!this->go) return;

    auto services = {PULPO::SERVICES::LastFm, PULPO::SERVICES::Spotify, PULPO::SERVICES::MusicBrainz, PULPO::SERVICES::Genius};
    auto ontology = PULPO::ONTOLOGY::ARTIST;

    qDebug()<<"getting missing artist artworks";
    auto queryTxt = QString("SELECT %1 FROM %2 WHERE %3 = ''").arg(KEYMAP[KEY::ARTIST],
            TABLEMAP[TABLE::ARTISTS],
            KEYMAP[KEY::ARTWORK]);
    auto artworks = con->getDBData(queryTxt);

    /* BEFORE FETCHING ONLINE LOOK UP IN THE CACHE FOR THE IMAGE */
    for(auto artist : artworks)
        if(BAE::artworkCache(artist, KEY::ARTIST))
            con->insertArtwork(artist);

    artworks = con->getDBData(queryTxt);
    this->setInfo(artworks, ontology, services, PULPO::INFO::ARTWORK, PULPO::RECURSIVE::OFF, nullptr);

    //select artist from artists where  artist  not in (select album from albums_tags)
    qDebug()<<"getting missing artist tags";
    queryTxt =  QString("SELECT %1 FROM %2 WHERE %1 NOT IN ( SELECT %1 FROM %3 ) ").arg(KEYMAP[KEY::ARTIST],
            TABLEMAP[TABLE::ARTISTS],
            TABLEMAP[TABLE::ARTISTS_TAGS]);
    this->setInfo(con->getDBData(queryTxt), ontology, services, PULPO::INFO::TAGS, PULPO::RECURSIVE::ON, nullptr);


    qDebug()<<"getting missing artist wikis";
    queryTxt =  QString("SELECT %1 FROM %2 WHERE %3 = '' ").arg(KEYMAP[KEY::ARTIST],
            TABLEMAP[TABLE::ARTISTS],
            KEYMAP[KEY::WIKI]);
    this->setInfo(con->getDBData(queryTxt), ontology, services, PULPO::INFO::WIKI, PULPO::RECURSIVE::OFF,  [](DB track)
    {
        CollectionDB conn(nullptr);
        conn.wikiArtist(track, SLANG[W::NONE]);
    });

    emit this->done(TABLE::ARTISTS);
}

