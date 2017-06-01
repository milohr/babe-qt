#ifndef TRACK_H
#define TRACK_H

#include <QObject>

class Track
{
public:
    Track();
    void operator=(Track const &);
    int getTrack();
    QString getName();
    QString getTitle();
    QString getArtist();
    QString getAlbum();
    QString getLocation();
    QString getGenre();
    QString getArtwork();

    void setTrack(int track);
    void setName(const QString &name);
    void setTitle(const QString &title);
    void setArtist(const QString &artist);
    void setAlbum(const QString &album);
    void setLocation(const QString &location);
    void setGenre(const QString &genre);
    void setArtwork(const QString &artwork);

private:
    int m_track = 0;
    QString m_name = "";
    QString m_title = "";
    QString m_artist = "";
    QString m_album = "";
    QString m_genre = "";
    QString m_artwork = "";
    QString m_location = "";
};

#endif // TRACK_H
