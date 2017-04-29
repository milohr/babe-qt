#pragma once

#include <string>

using namespace std;

class Track
{
public:
    Track();


    string getName();
    string getTitle();
    string getArtist();
    string getAlbum();
    string getLocation();
    int getTrack();
    string getGenre();
    string getArtwork();

    void setName(string name);
    void setTitle(string title);
    void setArtist(string artist);
    void setAlbum(string album);
    void setLocation(string location);
    void setTrack(int track);
    void setGenre(string genre);
    void setArtwork(string artwork);

private:
    string name = "";

    string title = "";
    string artist = "";
    string album ="";
    int track=0;
    string genre="";
    string artwork="";

    string location = "";
};

