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

class Playlist
{
public:
    Playlist();
    QStringList getList();
    void add(QStringList files);
    void addClean(QStringList files);
    void remove(int index);
    void removeAll();

    bool isMusic(QString file);
    QStringList getTracksNameList();
    QStringList getTracksNameListSimple();
    QList<QMap<int,QString>> getTracksData();

    std::vector<Track> tracks;

    QList<Track> getTracks();

private:

    void setMetaData(QString,QString,QString,QString);


};

#endif // PLAYLIST_H
