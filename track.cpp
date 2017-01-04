#include "track.h"

Track::Track()
{

}

string Track::getName()
{
    return name;
}

string Track::getAlbum()
{
    return album;
}

string Track::getTitle()
{
    return title;
}

string Track::getArtist()
{
    return artist;
}


string Track::getLocation()
{
    return location;
}

void Track::setAlbum(string album)
{
    this->album = album;
}

void Track::setArtist(string artist)
{
    this->artist = artist;
}

void Track::setTitle(string title)
{
    this->title = title;
}

void Track::setName(string name)
{
    this->name = name;
}
void Track::setLocation(string location)
{
    this->location = location;
}
