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

#include "taginfo.h"

#include <QString>
#include <QByteArray>

TagInfo::TagInfo(QString file)
{
    m_path = file;
    m_file = TagLib::FileRef(m_path.toUtf8());
}

void TagInfo::writeData()
{
    Pulpo info(getTitle(), getArtist(), getAlbum());
    QString newTitle(info.getStaticTrackInfo(Pulpo::TrackAlbum).toString());
    setTrack(info.getStaticTrackInfo(Pulpo::TrackPosition).toInt());
    setAlbum(newTitle.isEmpty() ? QStringLiteral("UNKNOWN") : newTitle);
}

bool TagInfo::isValidTag()
{
    m_tag = m_file.tag();
    return (m_tag && !m_tag->isEmpty());
}

QString TagInfo::getAlbum()
{
    if (isValidTag())
        return QString::fromStdWString(m_tag->album().toWString());
    return QStringLiteral("");
}

QString TagInfo::getTitle()
{
    if (!isValidTag())
        return QStringLiteral("");
    if (QString::fromStdWString(m_tag->title().toWString()).isEmpty())
       return fileName();
    return QString::fromStdWString(m_tag->title().toWString());
}

QString TagInfo::getArtist()
{
    if (!isValidTag())
        return QStringLiteral("");
    if (QString::fromStdWString(m_tag->artist().toWString()).size() > 0)
        return QString::fromStdWString(m_tag->artist().toWString());
    return QStringLiteral("UNKNOWN");
}

int TagInfo::getTrack()
{
    if (isValidTag())
        return static_cast<signed int>(m_tag->track());
    return 0;
}

QString TagInfo::getGenre()
{
    if (!isValidTag())
        return QStringLiteral("");
    if (QString::fromStdWString(m_tag->genre().toWString()).size() > 0)
        return QString::fromStdWString(m_tag->genre().toWString());
    return QStringLiteral("UNKNOWN");
}

QString TagInfo::fileName()
{
    return BaeUtils::getNameFromLocation(m_path);
}

int TagInfo::getYear()
{
    if (!isValidTag())
        return static_cast<signed int>(m_tag->year());
    return 0;
}

int TagInfo::getDuration()
{
    return m_file.audioProperties()->length();
}

QString TagInfo::getComment()
{
    if (!isValidTag())
        return QStringLiteral("");
    if (QString::fromStdWString(m_tag->comment().toWString()).size() > 0)
        return QString::fromStdWString(m_tag->genre().toWString());
    return QStringLiteral("UNKNOWN");
}

QByteArray TagInfo::getCover()
{
    return QByteArray();
}

void TagInfo::setCover(const QByteArray &array)
{
    Q_UNUSED(array);
}

void TagInfo::setComment(const QString &comment)
{
    if (!isValidTag())
        return;
    m_tag->setComment(comment.toStdString());
    m_file.save();
}

void TagInfo::setAlbum(const QString &album)
{
    if (!isValidTag())
        return;
    m_tag->setAlbum(album.toStdString());
    m_file.save();
}

void TagInfo::setTitle(const QString &title)
{
    if (!isValidTag())
        return;
    m_tag->setTitle(title.toStdString());
    m_file.save();
}

void TagInfo::setTrack(int track)
{
    if (!isValidTag())
        return;
    m_tag->setTrack(static_cast<unsigned int>(track));
    m_file.save();
}

void TagInfo::setArtist(const QString &artist)
{
    if (!isValidTag())
        return;
    m_tag->setArtist(artist.toStdString());
    m_file.save();
}

void TagInfo::setGenre(const QString &genre)
{
    if (!isValidTag())
        return;
    m_tag->setGenre(genre.toStdString());
    m_file.save();
}
