#ifndef NOTIFY_H
#define NOTIFY_H
#include <QObject>
#include<QByteArray>
#include <knotification.h>

#include <QDebug>
class Notify : public QObject
{
    Q_OBJECT
public:
    explicit Notify(QObject *parent = 0);
    void notify(const QString &title, const QString &body, const QPixmap &pix);
private:

   // NotifyNotification *notify;


signals:
    void babeSong(QString title, QString artist, QString album);

public slots:
    void babeIt();
};

#endif // NOTIFY_H
