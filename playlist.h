#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "QStringList"
#include "vector"
#include "track.h"
//#include <QtMultimedia/QMediaPlayer>

class Playlist
{
public:
    Playlist();
    QStringList getList();
    void add(QStringList files);
    void addClean(QStringList files);
    void remove(int index);
    void removeAll();
    void save();
    bool isMusic(QString file);
    QStringList getTracksNameList();
    QStringList getTracksNameListSimple();
    QList<QStringList> getTracksData();

    std::vector<Track> tracks;

    QList<Track> getTracks();

private:
    //QMediaPlayer *p;
    void setMetaData(QString,QString,QString,QString);


};

#endif // PLAYLIST_H
