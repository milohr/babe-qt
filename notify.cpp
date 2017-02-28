#include "notify.h"

Notify::Notify(QObject *parent) : QObject(parent)
{
    //qDebug()<<QStandardPaths::GenericConfigLocation;




}


void Notify::notify(  const QString &title, const QString &body)
{
    KNotification *notification = new KNotification(QStringLiteral("Notify"),
                                                    KNotification::CloseOnTimeout, this);

    // notification->setComponentName(QStringLiteral("Babe"));
    notification->setTitle(QStringLiteral("%1").arg(title));
    notification->setText(QStringLiteral("%1").arg(body));
    QPixmap babeIcon;
    babeIcon.load(":Data/data/babe_128.svg");
    notification->setPixmap(babeIcon);
    //connect(notification, SIGNAL(ac), this, SLOT(notify()));

    notification->sendEvent();

}

void Notify::notifySong(const QString &title,const QString &artist,const QString &album,const QString &url,  const QPixmap &pix)
{
    this->title=title; this->artist=artist; this->album=album; this->url=url;

    KNotification *notification = new KNotification(QStringLiteral("Playing"),
                                                    KNotification::CloseOnTimeout, this);

    // notification->setComponentName(QStringLiteral("Babe"));
    notification->setTitle(QStringLiteral("%1").arg(title));
    notification->setText(QStringLiteral("by %1 \xe2\x99\xa1 %2").arg(artist,album));
    if(!pix.isNull()) notification->setPixmap(pix);

    notification->setActions(QStringList(i18n("Babe it  \xe2\x99\xa1")));

    connect(notification, SIGNAL(activated(uint)), SLOT(babeIt()));


    //connect(notification, SIGNAL(ac), this, SLOT(notify()));

    notification->sendEvent();

}

void Notify::notifyUrgent(  const QString &title, const QString &body)
{
    KNotification *notification = new KNotification(QStringLiteral("Urgent"),
                                                    KNotification::CloseOnTimeout, this);

    // notification->setComponentName(QStringLiteral("Babe"));
    notification->setTitle(QStringLiteral("%1").arg(title));
    notification->setText(QStringLiteral("%1").arg(body));

    notification->sendEvent();

}

void Notify::babeIt()
{
    qDebug()<<"babe the shit out of it";
    emit babeSong({url});
}
