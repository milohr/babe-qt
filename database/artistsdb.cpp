#include "artistsdb.h"

#include <QVariant>

ArtistsDB* ArtistsDB::m_instance = nullptr;

ArtistsDB::ArtistsDB(QObject *parent) : BaseDB(QStringLiteral("artists"), parent)
{
}

ArtistsDB::~ArtistsDB()
{
}

ArtistsDB *ArtistsDB::instance()
{
    if (!m_instance)
        m_instance = new ArtistsDB;
    return m_instance;
}

bool ArtistsDB::artistExists(const QString &title)
{
    return BaseDB::loadItem(QVariantMap({{"title", title}})).size() > 0;
}

QString ArtistsDB::getArt(const QString &title)
{
    QVariantMap result = BaseDB::loadItem(QVariantMap({{"title", title}}));
    if (result.isEmpty())
        return QStringLiteral("");
    return result.value("art").toString();
}

QVariantMap ArtistsDB::loadArtist(const QVariantMap &filter, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadItem(filter, whereOperator, whereComparator);
}

QVariantList ArtistsDB::loadArtists(const QVariantMap &filter, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadList(filter, limit, offset, orderBy, descending, whereOperator, whereComparator);
}
