#include "taginfo.h"
#include<taglib/taglib.h>
#include<taglib/tag.h>
#include<taglib/fileref.h>

TagInfo::TagInfo(QString file)
{
    this->file=TagLib::FileRef(file.toUtf8());
}

QString TagInfo::getAlbum()
{
     return QString::fromStdWString(file.tag()->album().toWString());
}

QString TagInfo::getTitle()
{
    return QString::fromStdWString(file.tag()->title().toWString());
}

QString TagInfo::getArtist()
{
    return QString::fromStdWString(file.tag()->artist().toWString());
}

int TagInfo::getTrack()
{
    return file.tag()->track();
}

QString TagInfo::getGenre()
{
    return QString::fromStdWString(file.tag()->genre().toWString());

}
