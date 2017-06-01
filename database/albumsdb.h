#ifndef ALBUMSDB_H
#define ALBUMSDB_H

#include "basedb.h"

class QVariant;

class AlbumsDB: public BaseDB
{
    Q_OBJECT
private:
    explicit AlbumsDB(QObject *parent = 0);
    AlbumsDB(const AlbumsDB &other);
    virtual ~AlbumsDB();
    void operator=(AlbumsDB const &);

public:
    static AlbumsDB* instance();
    bool albumExists(const QString &artist, const QString &albumTitle);
    QString getArt(const QString &title);
    QVariantMap loadAlbum(const QVariantMap &filter, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));
    QVariantList loadAlbums(const QVariantMap &filter, int limit = -1, int offset = 0, const QString &orderBy = QStringLiteral(""), bool descending = false, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));

private:
    static AlbumsDB* m_instance;
};

#endif // ALBUMSDB_H
