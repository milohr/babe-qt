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
        //this->trackInfo();
        this->albumInfo();
        //this->artistInfo();
        //t.msleep(this->interval);
    }

    void trackInfo()
    {
        //        select title, artist, album from tracks t where url not in (select url from tracks_tags)
        //        const auto queryTxt =  QString("SELECT %1, %2, %3 FROM %4 WHERE %5 NOT IN ( SELECT %5 FROM %6 )").arg(Bae::DBColsMap[Bae::DBCols::TITLE],
        //                Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBColsMap[Bae::DBCols::ALBUM],Bae::DBTablesMap[Bae::DBTables::TRACKS],Bae::DBColsMap[Bae::DBCols::URL],
        //                Bae::DBTablesMap[Bae::DBTables::TRACKS_TAGS]);
        //        QSqlQuery query (queryTxt);
        //        for(auto track : connection.getTrackData(query))
        //        {

        //        }
        qDebug()<<"GETTING BRAINZ INFO";
    }

    void albumInfo()
    {

        /*setup pulpo in place*/

        Pulpo pulpo;
        connect(&pulpo, &Pulpo::infoReady, [this] (const Bae::DB &track, const Pulpo::RES &response)
        {
            qDebug()<< response[Pulpo::INFO::TAGS];
        });
        pulpo.setInfo(Pulpo::ONTOLOGY::ALBUM, Pulpo::INFO::ALL);
        pulpo.registerServices({Pulpo::SERVICES::LastFm, Pulpo::SERVICES::Spotify});


        /* get all albums missing information */

        //select album, artist from albums where  album  not in (select album from albums_tags) and artist  not in (select  artist from albums_tags)
        const auto queryTxt =  QString("SELECT %1, %2 FROM %3 WHERE %1 NOT IN ( SELECT %1 FROM %4 ) AND %2 NOT IN ( SELECT %2 FROM %4 )").arg(Bae::DBColsMap[Bae::DBCols::ALBUM],
                Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBTablesMap[Bae::DBTables::ALBUMS],Bae::DBTablesMap[Bae::DBTables::ALBUMS_TAGS]);
        QSqlQuery query (queryTxt);


        for(auto track : connection.getTrackData(query))
        {
            pulpo.feed(track);
        }

    }

    void artistInfo()
    {

    }

    void fetchArtwork()
    {
//        int amountArtists=0;
//        int amountAlbums=0;


//        QString queryTxt;
//        QSqlQuery query_Covers;
//        QSqlQuery query_Heads;

//        connect(&connection, &CollectionDB::artworkInserted,[this](Bae::DB albumMap)
//        {
//            emit this->artworkReady(albumMap);
//        });

//        queryTxt = QString("SELECT %1, %2 FROM %3 WHERE %4 = ''").arg(Bae::DBColsMap[Bae::DBCols::ALBUM],
//                Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBTablesMap[Bae::DBTables::ALBUMS],Bae::DBColsMap[Bae::DBCols::ARTWORK]);
//        query_Covers.prepare(queryTxt);

//        queryTxt = QString("SELECT %1 FROM %2 WHERE %3 = ''").arg(Bae::DBColsMap[Bae::DBCols::ARTIST],
//                Bae::DBTablesMap[Bae::DBTables::ARTISTS],Bae::DBColsMap[Bae::DBCols::ARTWORK]);
//        query_Heads.prepare(queryTxt);

//        if(query_Covers.exec())
//            while (query_Covers.next())
//            {
//                if(go)
//                {
//                    QString album = query_Covers.value(Bae::DBColsMap[Bae::DBCols::ALBUM]).toString();
//                    QString artist = query_Covers.value(Bae::DBColsMap[Bae::DBCols::ARTIST]).toString();
//                    QString title;
//                    QSqlQuery query_Title =
//                            connection.getQuery("SELECT title FROM tracks WHERE artist = \""+artist+"\" AND album = \""+album+"\" LIMIT 1");
//                    if(query_Title.next()) title=query_Title.value(Bae::DBColsMap[Bae::DBCols::TITLE]).toString();

//                    //                connection.insertArtwork({{Bae::DBCols::ARTWORK,""},{Bae::DBCols::ALBUM,album},{Bae::DBCols::ARTIST,artist}});

//                    Pulpo art({{Bae::DBCols::TITLE,title},{Bae::DBCols::ARTIST,artist},{Bae::DBCols::ALBUM,album}});

//                    connect(&art, &Pulpo::albumArtReady,[&] (QByteArray array){ art.saveArt(array,Bae::CachePath); });
//                    connect(&art, &Pulpo::artSaved, &connection, &CollectionDB::insertArtwork);

//                    if (art.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::LastFm)) qDebug()<<"using lastfm";
//                    else if(art.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::Spotify)) qDebug()<<"using spotify";
//                    else if(art.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::GeniusInfo)) qDebug()<<"using genius";
//                    else art.albumArtReady(QByteArray());
//                    amountAlbums++;
//                }
//            }
//        else qDebug()<<"fetchArt queryCover failed";


//        if(query_Heads.exec())
//            while (query_Heads.next())
//            {
//                if(go)
//                {
//                    QString artist = query_Heads.value(Bae::DBColsMap[Bae::DBCols::ARTIST]).toString();
//                    Pulpo art({{Bae::DBCols::ARTIST,artist}});

//                    connect(&art, &Pulpo::artistArtReady,[&] (QByteArray array){ art.saveArt(array,Bae::CachePath); });
//                    connect(&art, &Pulpo::artSaved, &connection, &CollectionDB::insertArtwork);

//                    art.fetchArtistInfo(Pulpo::ArtistArt,Pulpo::LastFm);

//                    amountArtists++;
//                }
//            }
//        else qDebug()<<"fetchArt queryHeads failed";


        emit this->finishedFetchingArtwork();

    }


private:
    QThread t;
    CollectionDB connection;
    uint interval = 20000;
    bool go = true;
    bool wait = true;
signals:

    void finishedFetchingArtwork();
    void artworkReady(Bae::DB album);
};
}
