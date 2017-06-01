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

#include "track.h"

Track::Track()
{
}

void Track::operator=(const Track &)
{
}

int Track::getTrack()
{
    return m_track;
}

QString Track::getName()
{
    return m_name;
}

QString Track::getAlbum()
{
    return m_album;
}

QString Track::getTitle()
{
    return m_title;
}

QString Track::getArtist()
{
    return m_artist;
}

QString Track::getLocation()
{
    return m_location;
}

QString Track::getGenre()
{
    return m_genre;
}

QString Track::getArtwork()
{
    return m_artwork;
}

void Track::setTrack(int track)
{
    m_track = track;
}

void Track::setGenre(const QString &genre)
{
    m_genre = genre;
}

void Track::setArtwork(const QString &artwork)
{
    m_artwork = artwork;
}

void Track::setAlbum(const QString &album)
{
    m_album = album;
}

void Track::setArtist(const QString &artist)
{
    m_artist = artist;
}

void Track::setTitle(const QString &title)
{
    m_title = title;
}

void Track::setName(const QString &name)
{
    m_name = name;
}

void Track::setLocation(const QString &location)
{
    m_location = location;
}
