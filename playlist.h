#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "QStringList"
#include "vector"
#include "track.h"
#include <QtMultimedia/QMediaPlayer>

class Playlist
{
public:
    Playlist();

    void add(QStringList files);

    void remove(int index);

    void save();
    bool isMusic(QString file);
    QStringList getTracksNameList();

    std::vector<Track> tracks;

     QList<Track> getTracks();

private:
    QMediaPlayer *p;
    void setMetaData(QString,QString,QString,QString);

signals:
    //void mediaStatusChanged();
};

#endif // PLAYLIST_H
