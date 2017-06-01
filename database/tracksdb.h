#ifndef TRACKSDB_H
#define TRACKSDB_H

#include "basedb.h"

class QVariant;

class TracksDB : public BaseDB
{
    Q_OBJECT
private:
    explicit TracksDB(QObject *parent = 0);
    TracksDB(const TracksDB &other);
    virtual ~TracksDB();
    void operator=(TracksDB const &);

public:
    static TracksDB* instance();
    bool trackExists(const QString &location);
    QVariantMap loadTrack(const QVariantMap &filter, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));
    QVariantList loadTracks(const QVariantMap &filter, int limit = -1, int offset = 0, const QString &orderBy = QStringLiteral(""), bool descending = false, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));

private:
    static TracksDB* m_instance;
};

#endif // TRACKSDB_H
