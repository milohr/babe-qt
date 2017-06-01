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

#include "baeUtils.h"
#include "pulpo/pulpo.h"

class QUrl;
class QString;
class QByteArray;
using namespace TagLib;

class TagInfo
{
public:
    TagInfo(QString file);
    int getTrack();
    QString getAlbum();
    QString getTitle();
    QString getArtist();
    QString getGenre();
    QString fileName();
    QString getComment();
    QByteArray getCover();

    int getDuration();
    int getYear();

    void setTrack(int track);
    void setAlbum(const QString &album);
    void setTitle(const QString &title);
    void setArtist(const QString &artist);
    void setGenre(const QString &genre);
    void setComment(const QString &comment);
    void setCover(const QByteArray &array);
    void writeData();
    bool isValidTag();

private:
    QString m_path;
    TagLib::Tag *m_tag;
    TagLib::FileRef m_file;
};

#endif // TAGINFO_H
