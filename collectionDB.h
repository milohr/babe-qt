#ifndef COLLECTIONDB_H
#define COLLECTIONDB_H
#include <QString>
#include <QStringList>
#include <QList>
#include <QSqlDatabase>
#include <track.h>
#include <QThread>
#include <QWidget>


class CollectionDB : public QObject
{
     Q_OBJECT
public:

    CollectionDB();
    virtual ~CollectionDB(){}
    void setCollectionDB(QString path);

    void setTrackList(QList <Track>);
    void prepareCollectionDB(QString path);
private:
    QSqlDatabase m_db;
    QList <Track> trackList;

public slots:
    void addTrack();


signals:
    void progress(int);
    void DBactionFinished(bool);

};


#endif // COLLECTION_H
