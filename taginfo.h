#ifndef TAGINFO_H
#define TAGINFO_H
#include <taglib/taglib.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/mp4properties.h>
#include <taglib/mp4tag.h>
#include <taglib/mp4file.h>
#include <taglib/mp4coverart.h>
#include <taglib/id3v2tag.h>

#include <QString>
#include <QByteArray>
#include "pulpo/pulpo.h"
#include "baeUtils.h"

class TagInfo
{

public:
    TagInfo(QString file);
    QString getAlbum();
    QString getTitle();
    QString getArtist();
    int getTrack();
    QString getGenre();
    QString fileName();
    QString getComment();
    QByteArray getCover();
    int getDuration();
    uint getYear();

    void setAlbum(QString album);
    void setTitle(QString title);
    void setTrack(int track);
    void setArtist(QString artist);
    void setGenre(QString genre);
    void setComment(QString comment);
    void setCover(QByteArray array);
    void writeData();


private:
    TagLib::FileRef file;
    QString path;
};

#endif // TAGINFO_H
