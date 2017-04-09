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

class Notify : public QObject
{
    Q_OBJECT
public:
    explicit Notify(QObject *parent = 0);
    void notifySong(const QString &title, const QString &artist, const QString &album, const QString &url, const QPixmap &pix);
    void notify(const QString &title, const QString &body);
    void notifyUrgent(const QString &title, const QString &body);
private:
    QString title;
    QString album;
    QString artist;
    QString url;
   // NotifyNotification *notify;


signals:
    void babeSong(QStringList urls);

public slots:
    void babeIt();
};

#endif // NOTIFY_H
