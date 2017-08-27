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
#include "baeUtils.h"

class Notify : public QObject
{
    Q_OBJECT

public:
    explicit Notify(QObject *parent = 0);
    void notifySong(const Bae::TRACKMAP &, const QPixmap &pix);
    void notify(const QString &title, const QString &body);
    void notifyUrgent(const QString &title, const QString &body);

private:
  Bae::TRACKMAP track;
   // NotifyNotification *notify;


signals:
    void babeSong(const Bae::TRACKMAP &track);
    void skipSong();

public slots:
    void actions(uint id);
};

#endif // NOTIFY_H
