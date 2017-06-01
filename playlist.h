#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <babetable.h>
#include <taginfo.h>

#include "vector"
#include "track.h"
#include <fstream>

class QFileInfo;
class QMimeDatabase;
class QMimeType;

class Playlist : QObject
{
    Q_OBJECT
public:
    explicit Playlist(QObject *parent = 0);
    QStringList getList();
    void add(QStringList files);
    void addClean(QStringList files);
    void remove(int index);
    void removeAll();
    bool isMusic(const QString &file);
    QStringList getTracksNameList();
    QStringList getTracksNameListSimple();
    QList<QMap<int, QString>> getTracksData();
    QList<Track> getTracks();

private:
    std::vector<Track> tracks;
    void setMetaData(const QString &title, const QString &artist, const QString &album, const QString &location);
};

#endif // PLAYLIST_H
