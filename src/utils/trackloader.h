#ifndef TRACKLOADER_H
#define TRACKLOADER_H

#include <QThread>
#include <QList>
#include <QObject>
#include <QAbstractItemModel>

#include "../utils/bae.h"
#include "../db/collectionDB.h"

class TrackLoader : public QObject
{
    Q_OBJECT

public:
    TrackLoader() : QObject()
    {
        this->con = new CollectionDB(this);
        qRegisterMetaType<BAE::DB>("BAE::DB");
        qRegisterMetaType<Qt::SortOrder>("Qt::SortOrder");
        qRegisterMetaType<QVector<int>>("<QVector<int>");
        qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
        qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
        this->moveToThread(&t);
        this->t.start();
    }

    ~TrackLoader()
    {
        this->go = false;
        this->t.quit();
        this->t.wait();
    }

    void requestTracks(QString query)
    {
        QMetaObject::invokeMethod(this, "getTracks", Q_ARG(QString, query));
    }

public slots:
    void getTracks(QString query)
    {
        qDebug()<<"GETTING TRACKS FROM BABETABLE THREAD";

        auto tracks = con->getDBData(query);
        if(tracks.size()>0)
        {
            for(auto trackMap : tracks)
                if(go) emit trackReady(trackMap);
                else break;
        }

        emit finished();
    }

signals:
    void trackReady(BAE::DB &trackMap);
    void finished();

private:
    QThread t;
    bool go = true;
    CollectionDB *con;
};



#endif // TRACKLOADER_H
