/*
   Babe - tiny music player
   Copyright (C) 2017  Camilo Higuita
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

   */


#include "notify.h"
#include "babetable.h"


Notify::Notify(QObject *parent) : QObject(parent){}

void Notify::notify(  const QString &title, const QString &body)
{
    KNotification *notification = new KNotification(QStringLiteral("Notify"),
                                                    KNotification::CloseOnTimeout, this);

    notification->setTitle(QStringLiteral("%1").arg(title));
    notification->setText(QStringLiteral("%1").arg(body));
    QPixmap babeIcon;
    babeIcon.load(":Data/data/babe_128.svg");
    notification->setPixmap(babeIcon);

    notification->sendEvent();

}

void Notify::notifySong(const QMap<int, QString> &trackMap,  const QPixmap &pix)
{
    this->track = trackMap;

    KNotification *notification = new KNotification(QStringLiteral("Playing"),
                                                    KNotification::CloseOnTimeout, this);

    notification->setTitle(QStringLiteral("%1").arg(track[BabeTable::TITLE]));
    notification->setText(QStringLiteral("by %1 - %2").arg(track[BabeTable::ARTIST],track[BabeTable::ALBUM]));
    if(!pix.isNull()) notification->setPixmap(pix);

    if(track[BabeTable::BABE].toInt()==1) notification->setActions(QStringList(i18n("Un-Babe it  \xe2\x99\xa1")));
    else notification->setActions(QStringList(i18n("Babe it  \xe2\x99\xa1")));

    connect(notification, SIGNAL(activated(uint)), SLOT(babeIt()));

    notification->sendEvent();

}

void Notify::notifyUrgent(  const QString &title, const QString &body)
{
    KNotification *notification = new KNotification(QStringLiteral("Urgent"),
                                                    KNotification::CloseOnTimeout, this);

    notification->setTitle(QStringLiteral("%1").arg(title));
    notification->setText(QStringLiteral("%1").arg(body));

    notification->sendEvent();

}

void Notify::babeIt()
{
    qDebug()<<"babe the shit out of it";
     emit babeSong(track);

}
