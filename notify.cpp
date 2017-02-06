#include "notify.h"

#include <klocalizedstring.h>
#include <KNotifyConfig>
#include <KNotification>
#include <kconfig.h>
#include <QStandardPaths>
#include <QPixmap>
Notify::Notify(QObject *parent) : QObject(parent)
{
    //qDebug()<<QStandardPaths::GenericConfigLocation;




}

void Notify::notify(  const QString &title, const QString &body, const QPixmap &pix)
{
    KNotification *notification = new KNotification(QStringLiteral("RequestConfirmation"),
                                                        KNotification::CloseOnTimeout, this);

       // notification->setComponentName(QStringLiteral("Babe"));
        notification->setTitle(QStringLiteral("%1").arg(title));
        notification->setText(QStringLiteral("%1").arg(body));
        if(!pix.isNull()) notification->setPixmap(pix);


        //connect(notification, SIGNAL(ac), this, SLOT(notify()));

         notification->sendEvent();

}
