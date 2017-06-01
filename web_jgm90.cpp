#include "web_jgm90.h"

WebJgm90::WebJgm90(QObject *parent) : QObject(parent)
{
}

QList<QMap<int,QString>> WebJgm90::getTrackData(const QString &query)
{
    Q_UNUSED(query)
    QList<QMap<int, QString>> mapList;
    return mapList;
}

QList<QMap<int,QString>> WebJgm90::fetch(const QString &query)
{
    Q_UNUSED(query)
    QList<QMap<int, QString>> mapList;
    return mapList;
}
