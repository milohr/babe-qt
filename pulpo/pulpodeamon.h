#ifndef PULPODEAMON_H
#define PULPODEAMON_H

#include "../collectionDB.h"


class PulpoDeamon : public QObject
{
    Q_OBJECT

public:
    PulpoDeamon() : QObject()
    {
        moveToThread(&t);

        qRegisterMetaType<Bae::DB>("Bae::DB");
        qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");

        t.start();
    }

    ~PulpoDeamon()
    {
        go=false;
        t.quit();
        t.wait();
    }

    void requestInfo(Bae::DBTables type, QString query)
    {
        if(type==Bae::DBTables::ALBUMS)
            QMetaObject::invokeMethod(this, "getAlbums", Q_ARG(QString, query));
        else if(type==Bae::DBTables::ARTISTS)
            QMetaObject::invokeMethod(this, "getArtists", Q_ARG(QString, query));
    }

    void next() { this->nextAlbum = false;  }

public slots:


    void getAlbums(QString query)
    {
        qDebug()<<"GETTING TRACKS FROM ALBUMSVIEW";

        QSqlQuery mquery(query);
        auto albums = this->connection.getAlbumData(mquery);
        if(albums.size()>0)
        {
            for(auto albumMap : albums)
            {   if(go)
                {
                    emit albumReady(albumMap);
                    while(this->nextAlbum==true){t.msleep(100);}
                    this->nextAlbum=!this->nextAlbum;

                }else break;
            }
        }
        t.msleep(100);
        emit finished();
    }

    void getArtists(QString query)
    {
        QSqlQuery mquery(query);
        auto artists = this->connection.getArtistData(mquery);
        if(artists.size()>0)
        {
            for(auto albumMap : artists)
            {
                if(go)
                {
                    emit albumReady(albumMap);
                    while(this->nextAlbum==true){t.msleep(100); }
                    this->nextAlbum=true;
                }else break;
            }
        }
        t.msleep(100);
        emit finished();
    }

signals:
    void albumReady(const Bae::DB &albumMap);
    void finished();

private:
    QThread t;
    CollectionDB connection;
    bool go=true;
    bool nextAlbum=true;
};


#endif // PULPODEAMON_H
