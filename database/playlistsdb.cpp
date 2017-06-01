#include "playlistsdb.h"

#include <QVariant>

PlaylistsDB *PlaylistsDB::m_instance = nullptr;

PlaylistsDB::PlaylistsDB(QObject *parent) : BaseDB(QStringLiteral("playlists"), parent)
{
}

PlaylistsDB::~PlaylistsDB()
{
}

PlaylistsDB *PlaylistsDB::instance()
{
    if (!m_instance)
        m_instance = new PlaylistsDB;
    return m_instance;
}

QVariantMap PlaylistsDB::loadPlaylist(const QVariantMap &filter, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadItem(filter, whereOperator, whereComparator);
}

QVariantList PlaylistsDB::loadPlaylists(const QVariantMap &filter, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return BaseDB::loadList(filter, limit, offset, orderBy, descending, whereOperator, whereComparator);
}
