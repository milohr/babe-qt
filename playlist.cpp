#include "playlist.h"
#include <fstream>
#include "utils.h"
#include <QtMultimedia/QMediaMetaData>
#include <QtMultimedia/QMediaPlayer>
#include <QDebug>
#include<taglib/taglib.h>
#include<taglib/tag.h>
#include<taglib/fileref.h>
#include<QString>
#include"track.h"
Playlist::Playlist()
{
    std::ifstream read("playlist");
    string loc;
    while(getline(read, loc)){
        Track track;
        track.setLocation(loc);
        track.setName(getNameFromLocation(loc));
        tracks.push_back(track);
    }
}

void Playlist::add(QStringList files)
{

    for(int i = 0; i < files.size(); i++)
    {

       /** QMediaPlayer *p = new QMediaPlayer();
        p->setMedia(QUrl::fromLocalFile(files[i]));
 p->connect(p, static_cast<void(QMediaObject::*)()>(&QMediaObject::metaDataChanged),
     [=](){ setMetaData(p->metaData(QMediaMetaData::Title).toString(), p->metaData(QMediaMetaData::AlbumArtist).toString(),p->metaData(QMediaMetaData::AlbumTitle).toString(),files[i]);});
**/
       TagLib::FileRef file(files[i].toUtf8());

        Track track;

         //qDebug()<<QString::fromStdWString(file.tag()->title().toWString());
         QString title = QString::fromStdWString(file.tag()->title().toWString());
         QString artist = QString::fromStdWString(file.tag()->artist().toWString());
         QString album = QString::fromStdWString(file.tag()->album().toWString());
//qDebug()<<title+artist+album;

        title = title.size()>0 ? title : QString::fromStdString( getNameFromLocation(files[i].toStdString()));
        artist = artist.size()>0 ? artist : "UNKWON";
        album = album.size()>0 ? album : "UNKWON";

        track.setTitle(title.toStdString());

        track.setArtist(artist.toStdString());
        track.setAlbum(album.toStdString());
        track.setLocation(files[i].toStdString());

        track.setName(getNameFromLocation(files[i].toStdString()));

        tracks.push_back(track);

    }
}

void Playlist::setMetaData(QString title, QString artist, QString album, QString location)
{
    qDebug()<<title+artist+album;
    Track track;
    track.setTitle(title.toStdString());

    track.setArtist(artist.toStdString());
    track.setAlbum(album.toStdString());
    track.setLocation(location.toStdString());
     //track.setName(getNameFromLocation(location.toStdString()));
    tracks.push_back(track);

}

void Playlist::remove(int index)
{
    tracks.erase(tracks.begin() + index);
}

void Playlist::save()
{
    std::ofstream write("playlist");
    for(int i = 0; i < tracks.size(); i++){
        write << tracks[i].getLocation() << std::endl;
    }
}

QStringList Playlist::getTracksNameList()
{
    QStringList list;
    for(int i = 0; i < tracks.size(); i++)
    {
        QString qstr = QString::fromStdString(tracks[i].getTitle()+"\nby "+tracks[i].getArtist());
        list.push_back(qstr);
    }
    return list;
}

QList<Track> Playlist::getTracks()
{
    QList<Track> list;
    for(int i = 0; i < tracks.size(); i++)
    {

        list.push_back(tracks[i]);
    }
    return list;
}
