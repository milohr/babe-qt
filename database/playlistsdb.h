#ifndef PLAYLISTSDB_H
#define PLAYLISTSDB_H

#include "basedb.h"

class QVariant;

class PlaylistsDB : public BaseDB
{
    Q_OBJECT
private:
    explicit PlaylistsDB(QObject *parent = 0);
    PlaylistsDB(const PlaylistsDB &other);
    virtual ~PlaylistsDB();
    void operator=(PlaylistsDB const &);

public:
    static PlaylistsDB* instance();
    QVariantMap loadPlaylist(const QVariantMap &filter, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));
    QVariantList loadPlaylists(const QVariantMap &filter, int limit = -1, int offset = 0, const QString &orderBy = QStringLiteral(""), bool descending = false, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));

private:
    static PlaylistsDB* m_instance;
};

#endif // PLAYLISTSDB_H
