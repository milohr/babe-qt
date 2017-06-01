#include "database.h"

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlRecord>
#include <QByteArray>
#include <QStringList>
#include <QApplication>
#include <QStandardPaths>

Database* Database::m_instance = nullptr;

Database::Database(QObject *parent) : QObject(parent)
{
    m_applicationName = QApplication::applicationName();
    setFileName();
    build();
}

Database::~Database()
{
    if (m_qSqlDatabase.isOpen())
        m_qSqlDatabase.close();
}

Database *Database::instance()
{
    if (!Database::m_instance)
        Database::m_instance = new Database;
    return Database::m_instance;
}

void Database::openConnection()
{
    if (!m_qSqlDatabase.isOpen()) {
        if (!QSqlDatabase::contains(m_applicationName)) {
            m_qSqlDatabase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_applicationName);
            m_qSqlDatabase.setDatabaseName(m_databaseName);
        }
        m_qSqlDatabase = QSqlDatabase::database(m_applicationName);
        m_qsqlQuery = QSqlQuery(m_qSqlDatabase);
    }
    if (!m_qSqlDatabase.isOpen())
        emit logMessage(QStringLiteral("Fatal error on insert! Database connection cannot be opened!"));
}

void Database::build(bool forceRebuild)
{
    if (forceRebuild) {
        if (m_databaseName.isEmpty())
            setFileName();
        QFile(m_databaseName).remove();
    }
    if (!QFile(m_databaseName).exists() && !forceRebuild) {
        QFile file(":/database/DatabaseTables.sql");
        if (!file.exists()) {
            QString log = QStringLiteral("Fatal error on build database. The file '");
            log.append(file.fileName() + QStringLiteral("' for database and tables creation query cannot be not found!"));
            emit logMessage(log);
            return;
        }
        if (!file.open(QIODevice::ReadOnly)) {
            emit logMessage(QStringLiteral("Fatal error on try to create database! The file with sql queries for database creation cannot be opened!"));
            return;
        }
        bool hasText;
        QString line;
        QByteArray readLine;
        QString cleanedLine;
        QStringList strings;
        while (!file.atEnd()) {
            hasText     = false;
            line        = "";
            readLine    = "";
            cleanedLine = "";
            strings.clear();
            while (!hasText) {
                readLine    = file.readLine();
                cleanedLine = readLine.trimmed();
                strings     = cleanedLine.split("--");
                cleanedLine = strings.at(0);
                if (!cleanedLine.startsWith("--") && !cleanedLine.startsWith("DROP") && !cleanedLine.isEmpty())
                    line += cleanedLine;
                if (cleanedLine.endsWith(";"))
                    break;
                if (cleanedLine.startsWith("COMMIT"))
                    hasText = true;
            }
            if (!line.isEmpty())
                queryExec(line);
            if (!m_qsqlQuery.isActive())
                emit logMessage(m_qsqlQuery.lastError().text());
        }
        file.close();
        QFile::setPermissions(m_databaseName, QFile::WriteOwner | QFile::ReadGroup | QFile::ReadUser | QFile::ReadOther);
        emit created();
    } else {
        emit logMessage(QStringLiteral("Database file already exists! The build was ignored."));
    }
}

void Database::setFileName()
{
    // set the absolute path for .db sqlite file
    QString fileName(m_applicationName+".db");
    if (qEnvironmentVariableIsSet("Q_OS_MAC") || qEnvironmentVariableIsSet("Q_OS_IOS")) {
        QString qTmpDir(getenv("TMPDIR"));
        qTmpDir.append("/"+fileName);
        fileName = qTmpDir;
    } else if (qEnvironmentVariableIsSet("Q_OS_ANDROID")) {
        // set the database file to APK parent directory
        fileName.prepend("../");
    } else {
        // is desktop
        QString path(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
        QDir qdir(path);
        if (!qdir.exists())
            qdir.mkpath(path);
        fileName.prepend(QUrl::fromLocalFile(path+"/").toLocalFile());
    }
    // for sqlite, the m_databaseName is:
    // the path (based on the OS) + the application name + .db
    m_databaseName = fileName;
}

bool Database::queryExec(const QString &sqlQueryString)
{
    openConnection();
    if (!m_qSqlDatabase.isOpen()) {
        emit logMessage(QStringLiteral("Fatal error on execute query! The database connection cannot be opened!"));
        return false;
    } else if (m_qsqlQuery.exec(sqlQueryString)) {
        emit logMessage(QStringLiteral("Query success executed: ") + sqlQueryString);
        return true;
    }
    QString error(m_qsqlQuery.lastError().text());
    if (!error.isEmpty()) {
        error.prepend(QStringLiteral("Fatal error on execute query! The error is: "));
        error.append(QStringLiteral(" The query executed is: ") + sqlQueryString);
        emit logMessage(error);
    }
    return false;
}

QVariantList Database::select(const QString &tableName, const QVariantMap &where, int limit, int offset, const QString &orderBy, bool descending, enum SELECT_TYPE select_type, const QString &whereOperator, const QString &whereComparator)
{
    bool withLikeClause = (whereComparator.compare("LIKE", Qt::CaseInsensitive) == 0);
    QVariantList resultSet;
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on select data! The table name is empty!"));
        return resultSet;
    }

    QString whereStr;
    QVariantMap map;
    QString sqlQueryString = QString(QStringLiteral("SELECT * FROM %1")).arg(tableName);

    if (!where.isEmpty()) {
        int k = 0;
        foreach (const QString &key, where.keys())
            whereStr.append(QString(QStringLiteral("%1 %2 %3 %4")).arg(k++ == 0 ? "" : " " + whereOperator, key, whereComparator, "?"));
        sqlQueryString.append(QStringLiteral(" WHERE ")).append(whereStr.remove("  ").trimmed().simplified());
    }

    if (!orderBy.isEmpty()) {
        sqlQueryString.append(QString(" ORDER BY %1").arg(orderBy));
        if (!orderBy.contains("asc") || orderBy.contains("desc"))
            sqlQueryString.append(descending ? QStringLiteral(" DESC") : QStringLiteral(" ASC"));
    }

    if (limit > 0 && offset == 0)
        sqlQueryString.append(QStringLiteral(" LIMIT ") + QString::number(limit));
    else if (offset > 0)
        sqlQueryString.append(QStringLiteral(" LIMIT ") + QString::number(limit) + " OFFSET " + QString::number(offset));

    openConnection();

    m_qsqlQuery.prepare(sqlQueryString);

    QVariant value;
    foreach (const QString &key, where.keys()) {
        value = where.value(key);
        if (withLikeClause)
            value = QVariant("\%"+where.value(key).toString()+"\%");
        m_qsqlQuery.addBindValue(value);
    }

    if (!m_qsqlQuery.exec())
        return resultSet;

    int totalColumns = m_qsqlQuery.record().count();
    if (!totalColumns || !m_qsqlQuery.size())
        return resultSet;

    int i = 0;
    QSqlRecord record;
    while (m_qsqlQuery.next()) {
        record = m_qsqlQuery.record();
        if (record.isEmpty() || !m_qsqlQuery.isValid())
            continue;
        for (i = 0; i < totalColumns; ++i) {
            if (select_type == All_Itens_Int)
                map.insert(record.fieldName(i), m_qsqlQuery.value(i));
            else if (select_type == Meta_Key_Value_Int)
                map.insert(m_qsqlQuery.value(0).toString(), m_qsqlQuery.value(1));
        }
        if (!map.isEmpty())
            resultSet.append(map);
    }
    return resultSet;
}

int Database::insert(const QString &tableName, const QVariantMap &insertData)
{
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on insert! The table name is empty!"));
        return 0;
    } else if (insertData.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on insert! The insertData is empty!"));
        return 0;
    }

    QStringList strValues;
    QStringList fields = insertData.keys();
    QVariantList values = insertData.values();
    int totalFields = fields.size();
    for (int i = 0; i < totalFields; ++i)
        strValues.append("?");

    openConnection();
    if (!m_qSqlDatabase.isOpen()) {
        emit logMessage(QStringLiteral("Fatal error on insert! Database connection cannot be opened!"));
        return 0;
    }

    QString sqlQueryString;
    sqlQueryString = "INSERT INTO " + tableName + "(" + QString(fields.join(",")) + ") VALUES(" + QString(strValues.join(",")) + ")";
    m_qsqlQuery.prepare(sqlQueryString);

    int k = 0;
    foreach (const QVariant &value, values)
        m_qsqlQuery.bindValue(k++, value);

    return m_qsqlQuery.exec() ? lastInsertId() : 0;
}

int Database::remove(const QString &tableName, const QVariantMap &where, const QString &whereComparator)
{
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on remove! The table name is empty!"));
        return 0;
    } else if (where.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on remove! The where parameter is empty!"));
        return 0;
    }
    int k = 0;
    QString whereStr(QStringLiteral(""));
    QString separator(QStringLiteral(""));
    QMap<QString, QVariant>::const_iterator j = where.constBegin();
    while (j != where.constEnd()) {
        separator = (k++ == 0) ? QStringLiteral("") : QStringLiteral(" AND ");
        whereStr += QString("%1%2 %3 '%4'").arg(separator).arg(j.key()).arg(whereComparator).arg(j.value().toString());
        ++j;
    }
    QString query(QStringLiteral("DELETE FROM ") + tableName);
    if (!where.isEmpty())
        query.append(QStringLiteral(" WHERE ") + whereStr);
    if (queryExec(query))
        return numRowsAffected();
    return 0;
}

int Database::update(const QString &tableName, const QVariantMap &updateData, const QVariantMap &where, const QString &whereOperator, const QString &whereComparator)
{
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on update! The table name is empty!"));
        return 0;
    } else if (where.empty()) {
        emit logMessage(QStringLiteral("Fatal error on update! The WHERE parameter must be set to update operation!"));
        return 0;
    }

    QVariantList values;
    QString whereStr(QStringLiteral(""));
    QString updateValues(QStringLiteral(""));
    {
        int k = 0;
        QString key;
        QString separator;
        QMap<QString, QVariant>::const_iterator i = updateData.constBegin();
        while (i != updateData.constEnd()) {
            key = i.key();
            separator = (k++ == 0) ? "" : ",";
            updateValues.append(QString("%1%2=%3").arg(separator, key, ":"+key));
            values << i.value();
            ++i;
        }
        k = 0;
        separator = "";
        i = where.constBegin();
        while (i != where.constEnd()) {
            key = i.key();
            separator = (k++ == 0) ? "" : (QStringLiteral(" ") + whereOperator + QStringLiteral(" "));
            whereStr += QString("%1%2%3%4").arg(separator, key, whereComparator, ":"+key);
            values << i.value();
            ++i;
        }
    }
    openConnection();
    if (!m_qSqlDatabase.isOpen()) {
        emit logMessage(QStringLiteral("Fatal error on insert! Database connection cannot be opened!"));
        return 0;
    }

    QString sqlQueryString;
    sqlQueryString = "UPDATE " + tableName + " SET " + updateValues + " WHERE " + whereStr +";";
    m_qsqlQuery.prepare(sqlQueryString);

    foreach (const QVariant &value, values)
        m_qsqlQuery.addBindValue(value);

    return m_qsqlQuery.exec() ? numRowsAffected() : 0;
}

int Database::lastInsertId() const
{
    // WARNING!
    // lastInsertId() return value that have been auto-generated by the autoincremented column!
    QVariant idTemp = m_qsqlQuery.lastInsertId();
    return idTemp.isValid() ? idTemp.toInt() : 0;
}

int Database::lastRowId(const QString &tableName)
{
    int rowId = 0;
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on lastRowId! The table name is empty!"));
    } else if (queryExec(QString("SELECT ROWID from %1 order by ROWID DESC limit 1").arg(tableName))) {
        while (m_qsqlQuery.next())
            rowId = m_qsqlQuery.value(0).toInt();
    }
    return rowId;
}

int Database::numRowsAffected() const
{
    return m_qsqlQuery.numRowsAffected();
}

QString Database::lastQuery() const
{
    return m_qsqlQuery.lastQuery();
}

QString Database::lastError() const
{
    return m_qsqlQuery.lastError().text();
}
