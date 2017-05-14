#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QSqlQuery>
#include <QJsonObject>
#include <QSqlDatabase>

class QUrl;
class QDir;
class QFile;
class QSqlError;
class QSqlRecord;
class QByteArray;
class QStringList;

class Database : public QObject
{
    Q_OBJECT
private:
    explicit Database(QObject *parent = 0);
    Database(const Database &other);
    void operator=(Database const &);
    virtual ~Database();
    void openConnection();
    bool queryExec(const QString &sqlQueryString);

public:
    enum SELECT_TYPE
    {
        All_Itens_Int,
        Meta_Key_Value_Int
    };

public:
    static Database *instance();
    void build(bool forceRebuild = false);
    void setName();
    bool sqliteFileExists();

    QVariantList select(const QString &tableName, const QVariantMap &where = QVariantMap(), int limit = -1, int offset = 0, const QString &orderBy = QStringLiteral(""), bool descending = false, enum SELECT_TYPE select_type = All_Itens_Int, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));
    int insert(const QString &tableName, const QVariantMap &insertData);
    int remove(const QString &tableName, const QVariantMap &where, const QString &whereOperator = "=");
    int update(const QString &tableName, const QVariantMap &updateMap, const QVariantMap &where, const QString &whereOperator = "AND");

    int lastInsertId() const;
    int numRowsAffected() const;
    int lastRowId(const QString &tableName);

    QString lastQuery() const;

signals:
    void created();
    void logMessage(const QString &message);

private:
    static Database *m_instance;
    QString m_databaseName;
    QString m_applicationName;
    QSqlQuery m_qsqlQuery;
    QSqlDatabase m_qSqlDatabase;
};

#endif // DATABASE_H
