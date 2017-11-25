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
        qRegisterMetaType<Bae::DB>("Bae::DB");
        qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");

        this->moveToThread(&t);
        t.start();
    }

    ~AlbumLoader()
    {
        go=false;
        t.quit();
        t.wait();
    }

    void requestAlbums(QString query)
    {
        if(this->go) this->go = false;

        this->go = true;

        QMetaObject::invokeMethod(this, "getAlbums", Q_ARG(QString, query));
    }

    void requestAlbums(Bae::DB_LIST albums)
    {
        if(this->go) this->go = false;

        this->go = true;

        QMetaObject::invokeMethod(this, "placeAlbums", Q_ARG(Bae::DB_LIST, albums));
    }

    void next() { this->nextAlbum = false;  }

public slots:

    void placeAlbums(Bae::DB_LIST albums)
    {
        if(!albums.isEmpty())
            for(auto albumMap : albums)
            {
                if(go)
                {
                    emit albumReady(albumMap);
                    while(this->nextAlbum && go){t.msleep(100);}
                    this->nextAlbum=!this->nextAlbum;

                }else return;
            }

        t.msleep(100);
        emit finished();
    }

    void getAlbums(QString query)
    {
        QSqlQuery mquery(query);
        auto albums = this->connection.getDBData(mquery);
        this->placeAlbums(albums);
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


#endif // ALBUMLOADER_H
