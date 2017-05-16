#include "basedb.h"
#include "database.h"

BaseDB::BaseDB(const QString &tableName, QObject *parent) : QObject(parent)
  ,m_tableName(tableName)
  ,m_database(Database::instance())
{
}

int BaseDB::save(const QVariantMap &data)
{
    return m_database->insert(m_tableName, data);
}

int BaseDB::update(const QVariantMap &data, const QVariantMap &where)
{
    return m_database->update(m_tableName, data, where);
}

int BaseDB::remove(const QVariantMap &where, const QString &whereOperator)
{
    return m_database->remove(m_tableName, where, whereOperator);
}

int BaseDB::resetTable()
{
    return m_database->remove(m_tableName, QVariantMap()) ? 1 : 0;
}

QVariantMap BaseDB::loadItem(const QVariantMap &filter, const QString &whereOperator, const QString &whereComparator)
{
    return m_database->select(m_tableName, filter, -1, 0, "title", false, Database::SELECT_TYPE::All_Itens_Int, whereOperator, whereComparator).at(0).toMap();
}

QVariantList BaseDB::loadList(const QVariantMap &filter, int limit, int offset, const QString &orderBy, bool descending, const QString &whereOperator, const QString &whereComparator)
{
    return m_database->select(m_tableName, filter, limit, offset, orderBy, descending, Database::SELECT_TYPE::All_Itens_Int, whereOperator, whereComparator);
}
