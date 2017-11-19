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

Notify::Notify(QObject *parent) : QObject(parent) {}

Notify::~Notify()
{
    qDebug()<<"DELETING KNOTIFY";
}

void Notify::notify(const QString &title, const QString &body)
{
    auto notification = new KNotification(QStringLiteral("Notify"),KNotification::CloseOnTimeout, this);
    // notification->setComponentName(QStringLiteral("Babe"));
    notification->setTitle(QStringLiteral("%1").arg(title));
    notification->setText(QStringLiteral("%1").arg(body));
    notification->sendEvent();

}

void Notify::notifySong(const Bae::DB &trackMap,  const QPixmap &pix)
{
    this->track = trackMap;
    auto notification = new KNotification(QStringLiteral("Notify"),KNotification::CloseOnTimeout, this);
    // notification->setComponentName(QStringLiteral("Babe"));
    notification->setTitle(QStringLiteral("%1").arg(track[Bae::KEY::TITLE]));
    notification->setText(QStringLiteral("%1\n%2").arg(track[Bae::KEY::ARTIST],track[Bae::KEY::ALBUM]));
    if(!pix.isNull()) notification->setPixmap(pix);
    QStringList actions;

    if(track[Bae::KEY::BABE].toInt()==1) actions<<i18n("Un-Babe it  \xe2\x99\xa1");
    else actions<<i18n("Babe it  \xe2\x99\xa1");

    actions<<i18n("Skip");

    notification->setActions(actions);
    connect(notification, SIGNAL(activated(uint)), SLOT(actions(uint)));

    notification->sendEvent();


}

void Notify::actions(uint id)
{
    switch(id)
    {
    case 1: emit this->babeSong(this->track); break;
    case 2: emit this->skipSong(); break;
    default: break;
    }

}
