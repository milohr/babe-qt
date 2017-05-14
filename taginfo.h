#ifndef TAGINFO_H
#define TAGINFO_H

#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/mp4coverart.h>
#include <taglib/mp4file.h>
#include <taglib/mp4properties.h>
#include <taglib/mp4tag.h>

#include <QString>
#include <QByteArray>
#include "baeUtils.h"
#include "pulpo/pulpo.h"

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
    int getYear();

    void setAlbum(const QString &album);
    void setTitle(const QString &title);
    void setTrack(int track);
    void setArtist(const QString &artist);
    void setGenre(const QString &genre);
    void setComment(const QString &comment);
    void setCover(const QByteArray &array);
    void writeData();

private:
    TagLib::FileRef file;
    QString path;
};

#endif // TAGINFO_H
