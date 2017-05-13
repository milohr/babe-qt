#include "artistsdb.h"

#include <QVariant>

ArtistsDB* ArtistsDB::m_instance = nullptr;

ArtistsDB::ArtistsDB(QObject *parent) : BaseDB(QStringLiteral("artists"), parent)
{
}

ArtistsDB *ArtistsDB::instance()
{
    if (!m_instance)
        m_instance = new ArtistsDB;
    return m_instance;
}

int ArtistsDB::save(const QVariantMap &artistData)
{
    return BaseDB::save(artistData);
}

int ArtistsDB::update(const QVariantMap &artistData, const QVariantMap &where)
{
    return BaseDB::update(artistData, where);
}

QVariantMap ArtistsDB::loadArtist(const QVariantMap &filter, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadItem(filter, whereOperator, whereComparator);
}

QVariantList ArtistsDB::loadArtists(const QVariantMap &filter, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadList(filter, limit, offset, orderBy, descending, whereOperator, whereComparator);
}
