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

Track::Track() {}

string Track::getName() { return name; }

string Track::getAlbum() { return album; }

string Track::getTitle() { return title; }

string Track::getArtist() { return artist; }

string Track::getLocation() { return location; }

int Track::getTrack() { return track; }

string Track::getGenre() { return genre; }

string Track::getArtwork() { return artwork; }

void Track::setTrack(int track) { this->track = track; }

void Track::setGenre(string genre) { this->genre = genre; }

void Track::setArtwork(string artwork) { this->artwork = artwork; }

void Track::setAlbum(string album) { this->album = album; }

void Track::setArtist(string artist) { this->artist = artist; }

void Track::setTitle(string title) { this->title = title; }

void Track::setName(string name) { this->name = name; }
void Track::setLocation(string location) { this->location = location; }
