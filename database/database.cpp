#include "database.h"

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QResource>
#include <QSqlRecord>
#include <QByteArray>
#include <QStringList>
#include <QVariantMap>
#include <QVariantHash>
#include <QApplication>
#include <QStandardPaths>

Database* Database::m_instance = nullptr;

Database::Database(QObject *parent) : QObject(parent)
{
    m_applicationName = QApplication::applicationName();
    setName();
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
}

void Database::build(bool forceRebuild)
{
    if (forceRebuild) {
        if (m_databaseName.isEmpty())
            setName();
        QFile databaseFileTemp(m_databaseName);
        databaseFileTemp.remove();
    }
    if (!sqliteFileExists() && !forceRebuild) {
        QFile file(":/DatabaseTables.sql");
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
        QFile::setPermissions(m_databaseName, QFile::WriteOwner | QFile::ReadOwner);
        emit created();
    } else {
        emit logMessage(QStringLiteral("Database file already exists! The build was ignored."));
    }
}

void Database::setName()
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
        QString path(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+"/"+m_applicationName);
        QDir qdir(path);
        if (!qdir.exists())
            qdir.mkpath(path);
        fileName.prepend(QUrl::fromLocalFile(path+"/").toLocalFile());
    }
    // for sqlite, the databaseName is:
    // the path (based on the OS) + the application name + .db
    m_databaseName = fileName;
}

bool Database::sqliteFileExists()
{
    return QFile(m_databaseName).exists();
}

QSqlQuery Database::qsqlQuery(const QString &queryTxt)
{
    openConnection();
    return QSqlQuery(queryTxt, m_qSqlDatabase);
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
        QMap<QString, QVariant>::const_iterator i = where.constBegin();
        while (i != where.constEnd()) {
            QString key(i.key());
            whereStr.append(QString(QStringLiteral("%1 %2 %3 '%4'")).arg((k++ == 0) ? "" : " " + whereOperator).arg(key).arg(whereComparator).arg(i.value().toString()));
            ++i;
        }
        sqlQueryString.append(QStringLiteral(" WHERE ")).append(whereStr.remove("  "));
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
    if (!queryExec(sqlQueryString))
        return resultSet;

    QSqlRecord resultRecord = m_qsqlQuery.record();
    int totalColumns = resultRecord.count();
    if (totalColumns == 0 || m_qsqlQuery.size() == 0)
        return resultSet;

    QSqlRecord record;

    int i = 0;
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

    QStringList values;
    QStringList fields = insertData.keys();
    int totalFields = fields.size();
    for (int i = 0; i < totalFields; ++i)
        values.append("?");
    openConnection();

    if (!m_qSqlDatabase.isOpen()) {
        emit logMessage(QStringLiteral("Fatal error on insert! Database connection cannot be opened!"));
        return 0;
    }

    m_qsqlQuery.prepare(QString(QStringLiteral("INSERT INTO %1(%2) VALUES(%3)")).arg(tableName).arg(QString(fields.join(","))).arg(QString(values.join(","))));

    for (int i = 0; i < totalFields; ++i)
        m_qsqlQuery.addBindValue(insertData.value(QString(fields.at(i)), ""));

    if (m_qsqlQuery.exec())
        return lastInsertId();

    return 0;
}

int Database::remove(const QString &tableName, const QVariantMap &where, const QString &whereOperator)
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
        whereStr += QString("%1%2 %3 '%4'").arg(separator).arg(j.key()).arg(whereOperator).arg(j.value().toString());
        ++j;
    }
    if (queryExec(QString(QStringLiteral("DELETE FROM %1 WHERE %2")).arg(tableName).arg(whereStr)))
        return numRowsAffected();
    return 0;
}

int Database::update(const QString &tableName, const QVariantMap &updateMap, const QVariantMap &where, const QString &whereOperator)
{
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on update! The table name is empty!"));
        return 0;
    } else if (where.empty()) {
        emit logMessage(QStringLiteral("Fatal error on update! The WHERE parameter must be set to update operation!"));
        return 0;
    }
    QString whereStr(QStringLiteral(""));
    QString updateValues(QStringLiteral(""));
    {
        int k = 0;
        QString separator;
        QMap<QString, QVariant>::const_iterator i = updateMap.constBegin();
        while (i != updateMap.constEnd()) {
            separator = (k++ == 0) ? "" : ",";
            updateValues.append(QString("%1%2 = '%3'").arg(separator).arg(i.key()).arg(i.value().toString()));
            ++i;
        }
        k = 0;
        separator = "";
        i = where.constBegin();
        while (i != where.constEnd()) {
            separator = (k++ == 0) ? "" : (QStringLiteral(" ") + whereOperator + QStringLiteral(" "));
            whereStr += QString("%1%2 = '%3'").arg(separator).arg(i.key()).arg(i.value().toString());
            ++i;
        }
    }
    if (queryExec(QString("UPDATE %1 SET %2 WHERE %3").arg(tableName).arg(updateValues).arg(whereStr)))
        return numRowsAffected();
    return 0;
}

int Database::lastInsertId()
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
    return m_qsqlQuery.executedQuery();
}
