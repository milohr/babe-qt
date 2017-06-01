#ifndef NOTIFY_H
#define NOTIFY_H

#include <klocalizedstring.h>
#include <KNotifyConfig>
#include <KNotification>

#include <QObject>

class Notify : public QObject
{
    Q_OBJECT
public:
    explicit Notify(QObject *parent = 0);
    void notify(const QString &title, const QString &body);
    void notifySong(const QMap<int, QString> &, const QPixmap &pix);
    void notifyUrgent(const QString &title, const QString &body);

private:
   QMap<int, QString> track;

signals:
    void babeSong(QMap<int, QString> mapList);

public slots:
    void babeIt();
};

#endif // NOTIFY_H
