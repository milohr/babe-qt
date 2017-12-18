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
#include "../services/local/taginfo.h"
#include "track.h"

Tracklist::Tracklist() {}

Tracklist::~Tracklist(){}

bool Tracklist::isMusic(const QString &file)
{
    QMimeDatabase mimeDatabase;
    QMimeType mimeType;

    mimeType = mimeDatabase.mimeTypeForFile(QFileInfo(file));
    // mp4 mpg4
    qDebug()<<mimeType.aliases()<<mimeType.name();

    if (mimeType.inherits("audio/mp4"))
        return true;
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
        return true;
    else if (mimeType.inherits("audio/ogg"))
        return true;
    else if (mimeType.inherits("audio/wav"))
        return true;
    else if (mimeType.inherits("audio/flac"))
        return true;
    else return false;
}


BAE::DB_LIST Tracklist::getTracks()
{
    return this->tracks;
}

void Tracklist::add(const QStringList &files)
{
    for (auto file : files)
    {
        qDebug()<<"FILE<<"<<file;
        if (isMusic(file))
        {
            TagInfo info(file);

            // qDebug()<<QString::fromStdWString(file.tag()->title().toWString());
            auto title = info.getTitle();
            auto artist = info.getArtist();
            auto album = info.getAlbum();
            auto artwork = ""; // here needs to get the artwork;
            auto track_n = info.getTrack();
            auto genre = info.getGenre();

            title = !title.isEmpty() ? title : info.fileName();
            artist = !artist.isEmpty()  ? artist : SLANG[W::UNKNOWN];
            album = !album.isEmpty() ? album : title;

            BAE::DB track = {
                {BAE::KEY::TITLE, title},
                {BAE::KEY::ARTIST, artist},
                {BAE::KEY::ALBUM, album},
                {BAE::KEY::URL, file},
                {BAE::KEY::GENRE, genre},
                {BAE::KEY::TRACK,QString::number(track_n)},
                {BAE::KEY::ARTWORK, artwork}
            };

            this->tracks << track;

        }else qDebug() << "file not valid: " << file;

    }
}


void Tracklist::remove(const int &index) { tracks.erase(tracks.begin() + index); }

void Tracklist::removeAll() { tracks.clear(); }


QStringList Tracklist::getTracksNameListSimple()
{
    QStringList list;
    for (auto track : this->tracks)
        list<<track[BAE::KEY::TITLE];

    return list;
}

QStringList Tracklist::getList()
{
    QStringList list;
    for(auto track : this->tracks)
        list<<track[BAE::KEY::URL];

    return list;
}
