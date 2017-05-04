#ifndef WEB_JGM90_H
#define WEB_JGM90_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

class web_jgm90 : public QObject
{
    Q_OBJECT

public:
    explicit web_jgm90(QObject *parent = 0);

    QList<QMap<int,QString>> getTrackData(const QString &query);
    QList<QMap<int,QString>> fetch(QString query);
signals:

public slots:
};

#endif // WEB_JGM90_H
