#ifndef TAGINFO_H
#define TAGINFO_H
#include<taglib/taglib.h>
#include<taglib/tag.h>
#include<taglib/fileref.h>
#include <QString>


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


private:
    TagLib::FileRef file;
    QString path;
};

#endif // TAGINFO_H
