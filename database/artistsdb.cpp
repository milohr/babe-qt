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

QVariantMap ArtistsDB::loadArtist(const QVariantMap &filter, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadItem(filter, whereOperator, whereComparator);
}

QVariantList ArtistsDB::loadArtists(const QVariantMap &filter, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadList(filter, limit, offset, orderBy, descending, whereOperator, whereComparator);
}
