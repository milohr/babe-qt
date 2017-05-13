#ifndef BASEDB_H
#define BASEDB_H

#include <QObject>

class Database;

class BaseDB : public QObject
{
    Q_OBJECT
public:
    explicit BaseDB(const QString &tableName, QObject *parent = 0);

    int save(const QVariantMap &data);
    int update(const QVariantMap &data, const QVariantMap &where);

    QVariantMap loadItem(const QVariantMap &filter, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));
    QVariantList loadList(const QVariantMap &filter, int limit = -1, int offset = 0, const QString &orderBy = QStringLiteral(""), bool descending = false, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));

signals:
    void saved(const QVariantMap &track);

private:
    QString m_tableName;
    Database *m_database;
};

#endif // BASEDB_H
