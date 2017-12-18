#ifndef ALBUMLOADER_H
#define ALBUMLOADER_H

#include <QObject>
#include <QThread>
#include <QList>
#include <QPersistentModelIndex>

#include "bae.h"
#include "../db/collectionDB.h"


class AlbumLoader : public QObject
{
    Q_OBJECT

public:
    AlbumLoader() : QObject()
    {
        qRegisterMetaType<BAE::DB>("BAE::DB");
        qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");

        this->con = new CollectionDB(this);
        this->moveToThread(&t);
        this->t.start();
    }

    ~AlbumLoader()
    {
        this->go = false;
        this->t.quit();
        this->t.wait();
    }

    void requestAlbums(QString query)
    {
        if(this->go) this->go = false;
        this->go = true;

        QMetaObject::invokeMethod(this, "getAlbums", Q_ARG(QString, query));
    }

    void requestAlbums(BAE::DB_LIST albums)
    {
        if(this->go) this->go = false;
        this->go = true;

        QMetaObject::invokeMethod(this, "placeAlbums", Q_ARG(BAE::DB_LIST, albums));
    }

    void next() { this->nextAlbum = false;  }

public slots:

    void placeAlbums(BAE::DB_LIST albums)
    {
        if(!albums.isEmpty())
        {
            for(auto albumMap : albums)
            {
                if(this->go)
                {
                    emit this->albumReady(albumMap);
                    while(this->nextAlbum && this->go){this->t.msleep(100);}
                    this->nextAlbum =! this->nextAlbum;

                }else return;
            }
        }

        this->t.msleep(100);
        emit this->finished();
    }

    void getAlbums(QString query)
    {
        auto albums = con->getDBData(query);
        this->placeAlbums(albums);
    }

signals:
    void albumReady(const BAE::DB &albumMap);
    void finished();

private:
    QThread t;
    bool go = true;
    bool nextAlbum = true;
    CollectionDB *con;
};


#endif // ALBUMLOADER_H
