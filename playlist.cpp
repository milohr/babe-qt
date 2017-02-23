#include "playlist.h"
#include <fstream>
// include "utils.h"

#include <QDebug>

#include "track.h"
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QString>
#include <taginfo.h>

Playlist::Playlist() {}

bool Playlist::isMusic(QString file) {
    QMimeDatabase mimeDatabase;
    QMimeType mimeType;

    mimeType = mimeDatabase.mimeTypeForFile(QFileInfo(file));
    // mp4 mpg4

    if (mimeType.inherits("audio/mp4"))
        return true;

    // mpeg mpg mpe
    else if (mimeType.inherits("audio/mpeg"))
        return true;
    else if (mimeType.inherits("video/mp4"))
        return true;
    else if (mimeType.inherits("audio/MPEG-4"))
        return true;
    else if (mimeType.inherits("video/mpeg"))
        return true;
    else if (mimeType.inherits("audio/m4a"))
        return true;

    else if (mimeType.inherits("audio/mp3"))
        return false;

    else if (mimeType.inherits("audio/ogg"))
        return true;

    else if (mimeType.inherits("audio/wav"))
        return true;

    else if (mimeType.inherits("audio/flac"))
        return true;

    else
        return false;
}


QList<QStringList> Playlist::getTracksData()
{
    QList<QStringList> tracks;

    for(auto file : getTracks())
    {
        QStringList track;
        track<<QString::number(file.getTrack());
        track<<QString::fromStdString(file.getTitle());
        track<<QString::fromStdString(file.getArtist());
        track<<QString::fromStdString(file.getAlbum());
        track<<QString::fromStdString(file.getGenre());
        track<<QString::fromStdString(file.getLocation());
        track<<"";
        track<<"";
        track<<"";
        track<<"";
        track<<"";
        tracks<<track;

    }

    return tracks;
}

void Playlist::add(QStringList files) {

    for (int i = 0; i < files.size(); i++) {
        if (isMusic(files[i])) {

            TagInfo info(files[i]);

            Track track;

            // qDebug()<<QString::fromStdWString(file.tag()->title().toWString());
            QString title = info.getTitle();
            QString artist = info.getArtist();
            QString album = info.getAlbum();
            QString artwork = ""; // here needs to get the artwork;
            int track_n = info.getTrack();
            QString genre = info.getGenre();

            title = title.size() > 0 ? title : info.fileName();
            artist = artist.size() > 0 ? artist : "UNKNOWN";
            album = album.size() > 0 ? album : "UNKNOWN";

            track.setTitle(title.toStdString());

            track.setArtist(artist.toStdString());
            track.setAlbum(album.toStdString());
            track.setLocation(files[i].toStdString());
            track.setGenre(genre.toStdString());
            track.setTrack(track_n);
            track.setArtwork(artwork.toStdString());
            track.setName(info.fileName().toStdString());

            tracks.push_back(track);
        } else {
            qDebug() << "file not valid: " << files[i];
        }
    }
}

void Playlist::addClean(QStringList files) {

    for (int i = 0; i < files.size(); i++) {


        TagInfo info(files[i]);

        Track track;

        // qDebug()<<QString::fromStdWString(file.tag()->title().toWString());
        QString title = info.getTitle();
        QString artist = info.getArtist();
        QString album = info.getAlbum();
        QString artwork = ""; // here needs to get the artwork;
        int track_n = info.getTrack();
        QString genre = info.getGenre();

        title = title.size() > 0 ? title : info.fileName();
        artist = artist.size() > 0 ? artist : "UNKNOWN";
        album = album.size() > 0 ? album : "UNKNOWN";

        track.setTitle(title.toStdString());

        track.setArtist(artist.toStdString());
        track.setAlbum(album.toStdString());
        track.setLocation(files[i].toStdString());
        track.setGenre(genre.toStdString());
        track.setTrack(track_n);
        track.setArtwork(artwork.toStdString());
        track.setName(info.fileName().toStdString());

        tracks.push_back(track);
    }
}
void Playlist::setMetaData(QString title, QString artist, QString album,
                           QString location) {
    qDebug() << title + artist + album;
    Track track;
    track.setTitle(title.toStdString());

    track.setArtist(artist.toStdString());
    track.setAlbum(album.toStdString());
    track.setLocation(location.toStdString());
    // track.setName(getNameFromLocation(location.toStdString()));
    tracks.push_back(track);
}

void Playlist::remove(int index) { tracks.erase(tracks.begin() + index); }

void Playlist::removeAll() { tracks.clear(); }



QStringList Playlist::getTracksNameList() {
    QStringList list;
    for (int i = 0; i < tracks.size(); i++) {
        QString qstr = QString::fromStdString(tracks[i].getTitle() + "\nby " +
                                              tracks[i].getArtist());
        list.push_back(qstr);
    }
    return list;
}

QStringList Playlist::getTracksNameListSimple() {
    QStringList list;
    for (int i = 0; i < tracks.size(); i++) {
        QString qstr = QString::fromStdString(tracks[i].getTitle());
        list.push_back(qstr);
    }
    return list;
}

QList<Track> Playlist::getTracks() {
    QList<Track> list;
    for (int i = 0; i < tracks.size(); i++) {

        list.push_back(tracks[i]);
    }
    return list;
}

QStringList Playlist::getList()
{
    QStringList list;
    for(auto track : getTracks())
    {
        list<<QString::fromStdString(track.getLocation());
    }

    return list;
}
