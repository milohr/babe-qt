#include "albumsdb.h"

#include <QVariant>

AlbumsDB* AlbumsDB::m_instance = nullptr;

AlbumsDB::AlbumsDB(QObject *parent) : BaseDB(QStringLiteral("albums"), parent)
{
}

AlbumsDB::~AlbumsDB()
{
}

AlbumsDB *AlbumsDB::instance()
{
    if (!m_instance)
        m_instance = new AlbumsDB;
    return m_instance;
}

bool AlbumsDB::albumExists(const QString &artist, const QString &albumTitle)
{
    return BaseDB::loadItem(QVariantMap({{"title", albumTitle}, {"artist", artist}}), "AND", "=").size() > 0;
}

QString AlbumsDB::getArt(const QString &title)
{
    QVariantMap result = BaseDB::loadItem(QVariantMap({{"title", title}}));
    if (result.isEmpty())
        return QStringLiteral("");
    return result.value("art").toString();
}

QVariantMap AlbumsDB::loadAlbum(const QVariantMap &filter, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadItem(filter, whereOperator, whereComparator);
}

QVariantList AlbumsDB::loadAlbums(const QVariantMap &filter, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadList(filter, limit, offset, orderBy, descending, whereOperator, whereComparator);
}
