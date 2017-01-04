#ifndef TRACK_H
#define TRACK_H

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

    void setName(string name);
    void setTitle(string title);
    void setArtist(string artist);
    void setAlbum(string album);

    void setLocation(string location);

private:
    string name = "";

    string title = "";
    string artist = "";
    string album ="";

    string location = "";
};

#endif // TRACK_H
