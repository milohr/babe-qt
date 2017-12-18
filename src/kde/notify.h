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
#include "../utils/bae.h"

class Notify : public QObject
{
    Q_OBJECT

public:
    explicit Notify(QObject *parent = nullptr);
    ~Notify();
    void notifySong(const BAE::DB &, const QPixmap &pix);
    void notify(const QString &title, const QString &body);

private:
  BAE::DB track;

signals:
    void babeSong(const BAE::DB &track);
    void skipSong();

public slots:
    void actions(uint id);
};

#endif // NOTIFY_H
