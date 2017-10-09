#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "vector"
#include "track.h"
#include <fstream>
#include <QDebug>
#include <QStringList>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QString>
#include <taginfo.h>
#include <babetable.h>

//#include <QtMultimedia/QMediaPlayer>

class Playlist
{
public:
    Playlist();
    ~Playlist();

    QStringList getList();
    void add(QStringList files);
    void addClean(QStringList files);
    void remove(int index);
    void removeAll();

    bool isMusic(QString file);
    QStringList getTracksNameList();
    QStringList getTracksNameListSimple();
    Bae::DB_LIST getTracksData();

    std::vector<Track> tracks;

    QList<Track> getTracks();

private:

    void setMetaData(QString,QString,QString,QString);


};

#endif // PLAYLIST_H
