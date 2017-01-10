#ifndef COLLECTIONDB_H
#define COLLECTIONDB_H
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <track.h>
class CollectionDB
{
public:

    CollectionDB();
    void setCollectionDB(QString path);
    bool addTrack(QList<Track> tracks);
    void prepareCollectionDB(QString path);
private:
    QSqlDatabase m_db;
};
#endif // COLLECTION_H
