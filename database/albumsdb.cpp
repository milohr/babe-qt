#include "albumsdb.h"

#include <QVariant>

AlbumsDB* AlbumsDB::m_instance = nullptr;

AlbumsDB::AlbumsDB(QObject *parent) : BaseDB(QStringLiteral("albums"), parent)
{
}

AlbumsDB *AlbumsDB::instance()
{
    if (!m_instance)
        m_instance = new AlbumsDB;
    return m_instance;
}

int AlbumsDB::save(const QVariantMap &albumData)
{
    return BaseDB::save(albumData);
}

int AlbumsDB::update(const QVariantMap &albumData, const QVariantMap &where)
{
    return BaseDB::update(albumData, where);
}

QVariantMap AlbumsDB::loadAlbum(const QVariantMap &filter, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadItem(filter, whereOperator, whereComparator);
}

QVariantList AlbumsDB::loadAlbums(const QVariantMap &filter, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadList(filter, limit, offset, orderBy, descending, whereOperator, whereComparator);
}
