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


#include "tracklist.h"


Tracklist::Tracklist() {}

Tracklist::~Tracklist(){}

bool Tracklist::isMusic(const QString &file)
{
    QMimeDatabase mimeDatabase;
    QMimeType mimeType;

    mimeType = mimeDatabase.mimeTypeForFile(QFileInfo(file));
    // mp4 mpg4

    if (mimeType.inherits("audio/mp4"))
        return true;

    // mpeg mpg mpe
    else if (mimeType.inherits("audio/mpeg"))
        return true;
    else if (mimeType.inherits("video/mp4"))
        return true;
    else if (mimeType.inherits("audio/MPEG-4"))
        return true;
    else if (mimeType.inherits("video/mpeg"))
        return true;
    else if (mimeType.inherits("audio/m4a"))
        return true;
    else if (mimeType.inherits("audio/mp3"))
        return false;
    else if (mimeType.inherits("audio/ogg"))
        return true;
    else if (mimeType.inherits("audio/wav"))
        return true;
    else if (mimeType.inherits("audio/flac"))
        return true;
    else return false;
}


Bae::DB_LIST Tracklist::getTracks()
{
    return this->tracks;
}

void Tracklist::add(const QStringList &files)
{

    for (auto file : files)
    {
        if (isMusic(file))
        {
            TagInfo info(file);

            // qDebug()<<QString::fromStdWString(file.tag()->title().toWString());
            QString title = info.getTitle();
            QString artist = info.getArtist();
            QString album = info.getAlbum();
            QString artwork = ""; // here needs to get the artwork;
            int track_n = info.getTrack();
            QString genre = info.getGenre();

            title = !title.isEmpty() ? title : info.fileName();
            artist = !artist.isEmpty()  ? artist : SLANG[W::UNKNOWN];
            album = !album.isEmpty() ? album : title;

            Bae::DB track = {
                {Bae::KEY::TITLE,title},
                {Bae::KEY::ARTIST,artist},
                {Bae::KEY::ALBUM,album},
                {Bae::KEY::URL,file},
                {Bae::KEY::GENRE,genre},
                {Bae::KEY::TRACK,QString::number(track_n)},
                {Bae::KEY::ARTWORK,artwork}
            };

            this->tracks<<track;
        } qDebug() << "file not valid: " << file;

    }
}


void Tracklist::remove(const int &index) { tracks.erase(tracks.begin() + index); }

void Tracklist::removeAll() { tracks.clear(); }


QStringList Tracklist::getTracksNameListSimple()
{
    QStringList list;
    for (auto track : this->tracks)
        list<<track[Bae::KEY::TITLE];

    return list;
}

QStringList Tracklist::getList()
{
    QStringList list;
    for(auto track : this->tracks)
        list<<track[Bae::KEY::URL];

    return list;
}
