#ifndef ARTISTSDB_H
#define ARTISTSDB_H

#include "basedb.h"

class QVariant;

class ArtistsDB : public BaseDB
{
    Q_OBJECT
private:
    explicit ArtistsDB(QObject *parent = 0);
    ArtistsDB(const ArtistsDB &other);
    void operator=(ArtistsDB const &);

public:
    static ArtistsDB* instance();
    QVariantMap loadArtist(const QVariantMap &filter, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));
    QVariantList loadArtists(const QVariantMap &filter, int limit = -1, int offset = 0, const QString &orderBy = QStringLiteral(""), bool descending = false, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));

private:
    static ArtistsDB* m_instance;
};

#endif // ARTISTSDB_H
