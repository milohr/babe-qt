#ifndef WEB_JGM90_H
#define WEB_JGM90_H

#include <QObject>
#include <QList>
#include <QMap>

class WebJgm90 : public QObject
{
    Q_OBJECT
public:
    explicit WebJgm90(QObject *parent = 0);
    QList<QMap<int, QString>> getTrackData(const QString &query);
    QList<QMap<int, QString>> fetch(const QString &query);
};

#endif // WEB_JGM90_H
