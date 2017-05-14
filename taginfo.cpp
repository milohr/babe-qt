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

TagInfo::TagInfo(QString file)
{
    this->file = TagLib::FileRef(file.toUtf8());
    this->path = file;
}

void TagInfo::writeData()
{
    Pulpo info(this->getTitle(), this->getArtist(), this->getAlbum());
    QString newTitle = info.getStaticTrackInfo(Pulpo::TrackAlbum).toString();
    int trackPosition = info.getStaticTrackInfo(Pulpo::TrackPosition).toInt();
    this->setTrack(trackPosition);
    this->setAlbum(newTitle.isEmpty() ? "UNKNOWN" : newTitle);
}

QString TagInfo::getAlbum()
{
    auto *tag = file.tag();
    if (!tag)
        return QStringLiteral("");
    return QString::fromStdWString(tag->album().toWString());
}

QString TagInfo::getTitle()
{
    auto *tag = file.tag();
    if (!tag)
        return QStringLiteral("");
    return QString::fromStdWString(tag->title().toWString()).isEmpty()
           ? fileName()
           : QString::fromStdWString(tag->title().toWString());
}

QString TagInfo::getArtist()
{
    auto *tag = file.tag();
    if (!tag)
        return QStringLiteral("");
    return QString::fromStdWString(tag->artist().toWString()).size() > 0
            ? QString::fromStdWString(tag->artist().toWString())
            : "UNKNOWN";
}

int TagInfo::getTrack()
{
    auto *tag = file.tag();
    if (!tag)
        return 0;
    return static_cast<signed int>(tag->track());
}

QString TagInfo::getGenre()
{
    auto *tag = file.tag();
    if (!tag)
        return QStringLiteral("");
    return QString::fromStdWString(tag->genre().toWString()).size() > 0
            ? QString::fromStdWString(tag->genre().toWString())
            : "UNKNOWN";
}

QString TagInfo::fileName()
{
    return BaeUtils::getNameFromLocation(path);
}

int TagInfo::getYear()
{
    auto *tag = file.tag();
    if (!tag)
        return 0;
    return static_cast<signed int>(tag->year());
}

int TagInfo::getDuration()
{
    return file.audioProperties()->length();
}

QString TagInfo::getComment()
{
    auto *tag = file.tag();
    if (!tag)
        return QStringLiteral("");
    return QString::fromStdWString(tag->comment().toWString()).size() > 0
            ? QString::fromStdWString(tag->genre().toWString())
            : "UNKNOWN";
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
    this->file.tag()->setComment(comment.toStdString());
    this->file.save();
}

void TagInfo::setAlbum(const QString &album)
{
    this->file.tag()->setAlbum(album.toStdString());
    this->file.save();
}

void TagInfo::setTitle(const QString &title)
{
    this->file.tag()->setTitle(title.toStdString());
    this->file.save();
}

void TagInfo::setTrack(int track)
{
    this->file.tag()->setTrack(static_cast<unsigned int>(track));
    this->file.save();
}

void TagInfo::setArtist(const QString &artist)
{
    this->file.tag()->setArtist(artist.toStdString());
    this->file.save();
}

void TagInfo::setGenre(const QString &genre)
{
    this->file.tag()->setGenre(genre.toStdString());
    this->file.save();
}
