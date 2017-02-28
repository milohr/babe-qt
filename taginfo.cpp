#include "taginfo.h"



TagInfo::TagInfo(QString file)
{
    this->file = TagLib::FileRef(file.toUtf8());
    path = file;
}


void TagInfo::writeData()
{
    // QString album_=this->getAlbum();
    QString title_=this->getTitle();
    QString artist_=this->getArtist();

    if(!title_.isEmpty()&&!artist_.isEmpty())
    {
        auto info = new ArtWork ();
        std::string newTitle=info->getAlbumTitle(artist_,title_).toStdString();

        this->file.tag()->setAlbum(newTitle.empty()?"UNKNOWN":newTitle);
        this->file.save();


    }
}

QString TagInfo::getAlbum() {

    QString albumName= QString::fromStdWString(file.tag()->album().toWString());

    /*if(albumName.isEmpty())
 {
     writeData();
     albumName= QString::fromStdWString(file.tag()->album().toWString());
 }*/

    return albumName;
}

QString TagInfo::getTitle() {
    return QString::fromStdWString(file.tag()->title().toWString()).size() > 0
            ? QString::fromStdWString(file.tag()->title().toWString())
            : fileName();
}

QString TagInfo::getArtist() {
    return QString::fromStdWString(file.tag()->artist().toWString()).size() > 0
            ? QString::fromStdWString(file.tag()->artist().toWString())
            : "UNKNOWN";
}

int TagInfo::getTrack() { return file.tag()->track(); }

QString TagInfo::getGenre() {
    return QString::fromStdWString(file.tag()->genre().toWString()).size() > 0
            ? QString::fromStdWString(file.tag()->genre().toWString())
            : "UNKNOWN";
}

QString TagInfo::fileName() {
    return QString::fromStdString(getNameFromLocation(path.toStdString()));
}
