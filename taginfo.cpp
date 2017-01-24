#include "taginfo.h"
#include<taglib/taglib.h>
#include<taglib/tag.h>
#include<taglib/fileref.h>

TagInfo::TagInfo(QString file)
{
    this->file=TagLib::FileRef(file.toUtf8());
    path = file;
}

QString TagInfo::getAlbum()
{
     return QString::fromStdWString(file.tag()->album().toWString()).size()>0 ? QString::fromStdWString(file.tag()->album().toWString()) : "UNKWON";
}

QString TagInfo::getTitle()
{
    return QString::fromStdWString(file.tag()->title().toWString()).size()>0 ? QString::fromStdWString(file.tag()->title().toWString()) : "UNKWON";

}

QString TagInfo::getArtist()
{
    return QString::fromStdWString(file.tag()->artist().toWString()).size()>0 ? QString::fromStdWString(file.tag()->artist().toWString()) : "UNKWON";
}

int TagInfo::getTrack()
{
    return file.tag()->track();
}

QString TagInfo::getGenre()
{
    return QString::fromStdWString(file.tag()->genre().toWString()).size()>0 ? QString::fromStdWString(file.tag()->genre().toWString()) : "UNKWON";
}


