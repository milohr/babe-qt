#ifndef NOTIFY_H
#define NOTIFY_H

#include <QObject>
#include <QByteArray>
#include <klocalizedstring.h>
#include <KNotifyConfig>
#include <KNotification>
#include <QStandardPaths>
#include <QPixmap>
#include <QDebug>
#include <QMap>


class Notify : public QObject
{
    Q_OBJECT

public:
    explicit Notify(QObject *parent = 0);
    void notifySong(const QMap<int, QString> &, const QPixmap &pix);
    void notify(const QString &title, const QString &body);
    void notifyUrgent(const QString &title, const QString &body);

private:
   QMap<int, QString> track;
   // NotifyNotification *notify;


signals:
    void babeSong(QMap<int, QString> mapList);

public slots:
    void babeIt();
};

#endif // NOTIFY_H
