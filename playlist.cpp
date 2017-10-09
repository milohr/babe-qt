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


#include "playlist.h"


Playlist::Playlist() {}

Playlist::~Playlist()
{
    delete this;
}

bool Playlist::isMusic(QString file)
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


Bae::DB_LIST Playlist::getTracksData()
{
    Bae::DB_LIST mapList;

    for(auto file : getTracks())
    {       
        QString track = QString::number(file.getTrack());
        QString title = QString::fromStdString(file.getTitle());
        QString artist = QString::fromStdString(file.getArtist());
        QString album = QString::fromStdString(file.getAlbum());
        QString genre = QString::fromStdString(file.getGenre());
        QString location = QString::fromStdString(file.getLocation());
        QString art = "";
        QString stars = "";
        QString babe = "";
        QString played = "";
        QString addDate = "";
        QString releaseDate = "";
        QString duration = "";

       mapList<< Bae::DB {{Bae::DBCols::TRACK,track},{Bae::DBCols::TITLE,title},{Bae::DBCols::ARTIST,artist},{Bae::DBCols::ALBUM,album},{Bae::DBCols::DURATION,duration},{Bae::DBCols::GENRE,genre},{Bae::DBCols::URL,location},{Bae::DBCols::STARS,stars},{Bae::DBCols::BABE,babe},{Bae::DBCols::ART,art},{Bae::DBCols::PLAYED,played},{Bae::DBCols::RELEASE_DATE,releaseDate},{Bae::DBCols::ADD_DATE,addDate}};
    }

    return mapList;
}

void Playlist::add(QStringList files)
{

    for (int i = 0; i < files.size(); i++) {
        if (isMusic(files[i])) {

            TagInfo info(files[i]);

            Track track;

            // qDebug()<<QString::fromStdWString(file.tag()->title().toWString());
            QString title = info.getTitle();
            QString artist = info.getArtist();
            QString album = info.getAlbum();
            QString artwork = ""; // here needs to get the artwork;
            int track_n = info.getTrack();
            QString genre = info.getGenre();

            title = !title.isEmpty() ? title : info.fileName();
            artist = !artist.isEmpty()  ? artist : "UNKNOWN";
            album = !album.isEmpty() ? album : title;

            track.setTitle(title.toStdString());

            track.setArtist(artist.toStdString());
            track.setAlbum(album.toStdString());
            track.setLocation(files[i].toStdString());
            track.setGenre(genre.toStdString());
            track.setTrack(track_n);
            track.setArtwork(artwork.toStdString());
            track.setName(info.fileName().toStdString());

            tracks.push_back(track);
        } else {
            qDebug() << "file not valid: " << files[i];
        }
    }
}

void Playlist::addClean(QStringList files)
{

    for (int i = 0; i < files.size(); i++)
    {
        TagInfo info(files[i]);

        Track track;

        // qDebug()<<QString::fromStdWString(file.tag()->title().toWString());
        QString title = info.getTitle();
        QString artist = info.getArtist();
        QString album = info.getAlbum();
        QString artwork = ""; // here needs to get the artwork;
        int track_n = info.getTrack();
        QString genre = info.getGenre();

        title = title.size() > 0 ? title : info.fileName();
        artist = artist.size() > 0 ? artist : "UNKNOWN";
        album = album.size() > 0 ? album : "UNKNOWN";

        track.setTitle(title.toStdString());

        track.setArtist(artist.toStdString());
        track.setAlbum(album.toStdString());
        track.setLocation(files[i].toStdString());
        track.setGenre(genre.toStdString());
        track.setTrack(track_n);
        track.setArtwork(artwork.toStdString());
        track.setName(info.fileName().toStdString());

        tracks.push_back(track);
    }
}

void Playlist::setMetaData(QString title, QString artist, QString album,QString location)
{
    qDebug() << title + artist + album;
    Track track;
    track.setTitle(title.toStdString());

    track.setArtist(artist.toStdString());
    track.setAlbum(album.toStdString());
    track.setLocation(location.toStdString());
    // track.setName(getNameFromLocation(location.toStdString()));
    tracks.push_back(track);
}

void Playlist::remove(int index) { tracks.erase(tracks.begin() + index); }

void Playlist::removeAll() { tracks.clear(); }

QStringList Playlist::getTracksNameList()
{
    QStringList list;
    for (unsigned int i = 0; i < tracks.size(); i++)
    {
        QString qstr = QString::fromStdString(tracks[i].getTitle() + "\nby " +
                                              tracks[i].getArtist());
        list.push_back(qstr);
    }
    return list;
}

QStringList Playlist::getTracksNameListSimple()
{
    QStringList list;
    for (unsigned int i = 0; i < tracks.size(); i++)
    {
        QString qstr = QString::fromStdString(tracks[i].getTitle());
        list.push_back(qstr);
    }
    return list;
}

QList<Track> Playlist::getTracks()
{
    QList<Track> list;
    for (unsigned int i = 0; i < tracks.size(); i++)
    {

        list.push_back(tracks[i]);
    }
    return list;
}

QStringList Playlist::getList()
{
    QStringList list;
    for(auto track : getTracks())
    {
        list<<QString::fromStdString(track.getLocation());
    }

    return list;
}
