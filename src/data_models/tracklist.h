#ifndef TRACKLIST_H
#define TRACKLIST_H

#include "vector"
#include <fstream>
#include <QDebug>
#include <QStringList>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QString>
#include "../utils/bae.h"
using namespace BAE;

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
    BAE::DB_LIST getTracks();


private:
    BAE::DB_LIST tracks;
    void setMetaData(QString,QString,QString,QString);


};

#endif // TRACKLIST_H
