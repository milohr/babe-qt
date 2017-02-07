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
