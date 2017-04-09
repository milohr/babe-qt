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
    path = file;
}


void TagInfo::writeData()
{
    // QString album_=this->getAlbum();
    QString title_=this->getTitle();
    QString artist_=this->getArtist();

    if(!title_.isEmpty()&&!artist_.isEmpty())
    {
        ArtWork info;
        auto newTitle=info.getAlbumTitle(artist_,title_);

        this->setAlbum(newTitle.isEmpty()?"UNKNOWN":newTitle);
    }
}

QString TagInfo::getAlbum() {

    QString albumName= QString::fromStdWString(file.tag()->album().toWString());

    /*if(albumName.isEmpty())
 {
     writeData();
     albumName= QString::fromStdWString(file.tag()->album().toWString());
 }*/

    return albumName;
}

QString TagInfo::getTitle() {
    return QString::fromStdWString(file.tag()->title().toWString()).size() > 0
            ? QString::fromStdWString(file.tag()->title().toWString())
            : fileName();
}

QString TagInfo::getArtist() {
    return QString::fromStdWString(file.tag()->artist().toWString()).size() > 0
            ? QString::fromStdWString(file.tag()->artist().toWString())
            : "UNKNOWN";
}

int TagInfo::getTrack() { return static_cast<signed int>(file.tag()->track()); }

QString TagInfo::getGenre() {
    return QString::fromStdWString(file.tag()->genre().toWString()).size() > 0
            ? QString::fromStdWString(file.tag()->genre().toWString())
            : "UNKNOWN";
}

QString TagInfo::fileName() {
    return BaeUtils::getNameFromLocation(path);
    //return file.file()->name();
}

int TagInfo::getYear() {
    //return BaeUtils::getNameFromLocation(path);
    return static_cast<signed int>(file.tag()->year());
}


int TagInfo::getDuration()
{
    return file.audioProperties()->length();

}



QString TagInfo::getComment()
{
    return QString::fromStdWString(file.tag()->comment().toWString()).size() > 0
            ? QString::fromStdWString(file.tag()->genre().toWString())
            : "UNKNOWN";
}

void TagInfo::setComment(QString comment)
{

    this->file.tag()->setComment(comment.toStdString());
    this->file.save();

}

void TagInfo::setAlbum(QString album)
{
    this->file.tag()->setAlbum(album.toStdString());
    this->file.save();

}

void TagInfo::setTitle(QString title)
{
    this->file.tag()->setTitle(title.toStdString());
    this->file.save();
}

void TagInfo::setTrack(int track)
{
    this->file.tag()->setTrack(static_cast<unsigned int>(track));
    this->file.save();
}

void TagInfo::setArtist(QString artist)
{
    this->file.tag()->setArtist(artist.toStdString());
    this->file.save();
}

void TagInfo::setGenre(QString genre)
{
    this->file.tag()->setGenre(genre.toStdString());
    this->file.save();
}
