#ifndef TRACKLIST_H
#define TRACKLIST_H

#include "vector"
#include "track.h"
#include <fstream>
#include <QDebug>
#include <QStringList>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QString>
#include "../services/local/taginfo.h"
#include "../widget_models/babetable.h"
#include "../utils/bae.h"

//#include <QtMultimedia/QMediaPlayer>

class Tracklist
{
public:
    Tracklist();
    ~Tracklist();

    QStringList getList();
    void add(const QStringList &files);
    void remove(const int &index);
    void removeAll();

    bool isMusic(const QString &file);
    QStringList getTracksNameListSimple();
    Bae::DB_LIST getTracks();


private:
    Bae::DB_LIST tracks;
    void setMetaData(QString,QString,QString,QString);


};

#endif // TRACKLIST_H
