#include "tracksdb.h"

#include <QVariant>

TracksDB* TracksDB::m_instance = nullptr;

TracksDB::TracksDB(QObject *parent) : BaseDB(QStringLiteral("tracks"), parent)
{
}

TracksDB *TracksDB::instance()
{
    if (!m_instance)
        m_instance = new TracksDB;
    return m_instance;
}

int TracksDB::save(const QVariantMap &trackData)
{
    return BaseDB::save(trackData);
}

int TracksDB::update(const QVariantMap &trackData, const QVariantMap &where)
{
    return BaseDB::update(trackData, where);
}

QVariantMap TracksDB::loadTrack(const QVariantMap &filter, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadItem(filter, whereOperator, whereComparator);
}

QVariantList TracksDB::loadTracks(const QVariantMap &filter, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadList(filter, limit, offset, orderBy, descending, whereOperator, whereComparator);
}
