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

class TagInfo
{

public:
    TagInfo(const QString &url);
    ~TagInfo();
    QString getAlbum() const;
    QString getTitle() const;
    QString getArtist() const;
    int getTrack() const;
    QString getGenre() const;
    QString fileName() const;
    QString getComment() const;
    QByteArray getCover() const;
    int getDuration() const;
    uint getYear() const;

    void setAlbum(const QString &album) ;
    void setTitle(const QString &title);
    void setTrack(const int &track);
    void setArtist(const QString &artist);
    void setGenre(const QString &genre);
    void setComment(const QString &comment);
    void setCover(const QByteArray &array);


private:
    TagLib::FileRef file;
    QString path;
};

#endif // TAGINFO_H
