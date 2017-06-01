#include "tracksdb.h"

#include "database.h"
#include <QDebug>
#include <QVariant>

TracksDB* TracksDB::m_instance = nullptr;

TracksDB::TracksDB(QObject *parent) : BaseDB(QStringLiteral("tracks"), parent)
{
}

TracksDB::~TracksDB()
{
}

TracksDB *TracksDB::instance()
{
    if (!m_instance)
        m_instance = new TracksDB;
    return m_instance;
}

bool TracksDB::trackExists(const QString &location)
{
    return BaseDB::loadItem(QVariantMap({{"location", location}})).size() > 0;
}

QVariantMap TracksDB::loadTrack(const QVariantMap &filter, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadItem(filter, whereOperator, whereComparator);
}

QVariantList TracksDB::loadTracks(const QVariantMap &filter, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadList(filter, limit, offset, orderBy, descending, whereOperator, whereComparator);
}
