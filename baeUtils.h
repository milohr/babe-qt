#ifndef BAEUTILS_H
#define BAEUTILS_H

#include "string"
#include <QString>
#include <QDebug>
#include <QStandardPaths>
#include <QFileInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QTime>
#include <cmath>
using namespace std;

namespace Bae
{

enum DBTables
{
    ALBUMS=0,ARTISTS=1,MOODS=2,PLAYLISTS=3,SOURCES=4,SOURCES_TYPES=5,
    TRACKS=6,TRACKS_MOODS=7,TRACKS_PLAYLISTS=8
};

static const QMap<DBTables,QString> DBTablesMap =
{
    {DBTables::ALBUMS,"albums"},{DBTables::ARTISTS,"artists"},{DBTables::MOODS,"moods"},
    {DBTables::PLAYLISTS,"playlists"},{DBTables::SOURCES,"sources"},{DBTables::SOURCES_TYPES,"sources_types"},
    {DBTables::TRACKS,"tracks"},{DBTables::TRACKS_MOODS,"tracks_moods"},{DBTables::TRACKS_PLAYLISTS,"tracks_playlists"}
};

enum ArtistsCols
{
    ARTIST_TITLE=0, ARTIST_ARTWORK=1, ARTIST_WIKI=2
};

static const QMap<ArtistsCols,QString> ArtistsColsMap =
{
    {ArtistsCols::ARTIST_TITLE,"title"},
    {ArtistsCols::ARTIST_ARTWORK,"artwork"},
    {ArtistsCols::ARTIST_WIKI,"wiki"}
};

enum AlbumsCols
{
    ALBUM_TITLE=0, ALBUM_ARTIST=1, ALBUM_ARTWORK=2, ALBUM_WIKI=3
};
typedef QMap<Bae::AlbumsCols, QString> ALBUMMAP;

static const QMap<AlbumsCols,QString> AlbumsColsMap =
{
    {AlbumsCols::ALBUM_TITLE,"title"},
    {AlbumsCols::ALBUM_ARTIST,"artist"},
    {AlbumsCols::ALBUM_ARTWORK,"artwork"},
    {AlbumsCols::ALBUM_WIKI,"wiki"},
};

enum TracksCols
{
    URL=0,SOURCES_URL=1,TRACK=2,TITLE=3,
    ARTIST=4, ALBUM=5, DURATION=6,
    PLAYED=7,BABE=8,STARS=9,RELEASE_DATE=10,
    ADD_DATE=11,LYRICS=12,GENRE=13,ART=14, columnsCOUNT=15
};

typedef QMap<Bae::TracksCols, QString> TRACKMAP;
typedef QList<TRACKMAP> TRACKMAP_LIST;

static const TRACKMAP TracksColsMap =
{
    {TracksCols::URL,"url"},{TracksCols::SOURCES_URL,"sources_url"},
    {TracksCols::TRACK,"track"},{TracksCols::TITLE,"title"},
    {TracksCols::ARTIST,"artist"},{TracksCols::ALBUM,"album"},
    {TracksCols::DURATION,"duration"},{TracksCols::PLAYED,"played"},{TracksCols::BABE,"babe"},
    {TracksCols::STARS,"stars"},{TracksCols::RELEASE_DATE,"releaseDate"},{TracksCols::ADD_DATE,"addDate"},
    {TracksCols::LYRICS,"lyrics"},{TracksCols::GENRE,"genre"},{TracksCols::ART,"art"}
};


enum ALbumSizeHint {BIG_ALBUM=200,MEDIUM_ALBUM=120,SMALL_ALBUM=80};

static const int MAX_BIG_ALBUM_SIZE=300;
static const int MAX_MID_ALBUM_SIZE=200;
static const int MAX_MIN_ALBUM_SIZE=100;

static const double BIG_ALBUM_FACTOR = 0.039;
static const double BIG_ALBUM_FACTOR_SUBWIDGET = 0.27;

static const double MEDIUM_ALBUM_FACTOR = 0.013;
static const double MEDIUM_ALBUM_FACTOR_SUBWIDGET = 0.4;

static const double SMALL_ALBUM_FACTOR = 0.006;
static const double SMALL_ALBUM_FACTOR_SUBWIDGET = 0.5;

inline int getWidgetSizeHint(const double &factor, const ALbumSizeHint &deafultValue)
{
    int ALBUM_SIZE = deafultValue;
    auto screenSize = QApplication::desktop()->availableGeometry().size();
    int albumSizeHint =  static_cast<int>(sqrt((screenSize.height()*screenSize.width())*factor));
    ALBUM_SIZE = albumSizeHint > ALBUM_SIZE? albumSizeHint : ALBUM_SIZE;


    switch(deafultValue)
    {

    case BIG_ALBUM:  return ALBUM_SIZE > MAX_BIG_ALBUM_SIZE? MAX_BIG_ALBUM_SIZE : ALBUM_SIZE;
    case MEDIUM_ALBUM:  return ALBUM_SIZE > MAX_MID_ALBUM_SIZE? MAX_MID_ALBUM_SIZE : ALBUM_SIZE;
    case SMALL_ALBUM:  return ALBUM_SIZE > MAX_MIN_ALBUM_SIZE ? MAX_MIN_ALBUM_SIZE :ALBUM_SIZE;

    }

    return MAX_MID_ALBUM_SIZE;
}

inline QString transformTime(const qint64 &value)
{
    QString tStr;
    if (value)
    {
        QTime time((value/3600)%60, (value/60)%60, value%60, (value*1000)%1000);
        QString format = "mm:ss";
        if (value > 3600)
            format = "hh:mm:ss";
        tStr = time.toString(format);
    }
    return tStr.isEmpty()?"00:00":tStr;
}



inline QString getNameFromLocation(const QString &str)
{
    QString ret;
    int index = 0;

    for(int i = str.size() - 1; i >= 0; i--)
    {
        if(str[i] == '/')
        {
            index = i + 1;
            i = -1;
        }
    }

    for(; index < str.size(); index++)
        ret.push_back(str[index]);


    return ret;
}

const QString SettingPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+"/babe/";
const QString CollectionDBPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/babe/";
const QString CachePath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)+"/babe/";
const QString YoutubeCachePath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)+"/babe/youtube/";
const QString ExtensionFetchingPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
const QString NotifyDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
const QStringList MoodColors = {"#F0FF01","#01FF5B","#3DAEFD","#B401FF","#E91E63"};


inline QString fixTitle(const QString &title,const QString &s,const QString &e)
{
    QString newTitle;
    for(int i=0; i<title.size();i++)
    {
        if(title.at(i)==s)
        {
            while(title.at(i)!=e)
            {
                if(i==title.size()-1) break;
                else i++;
            }
        }else newTitle+=title.at(i);
    }

    return newTitle.simplified();
}


inline QString removeSubstring(const QString &newTitle, const QString &subString)
{
    const int indexFt = newTitle.indexOf(subString, 0, Qt::CaseInsensitive);

    if (indexFt != -1) {
        return newTitle.left(indexFt).simplified();
    }else
    {
        return newTitle;
    }
}

inline QString ucfirst(const QString &str)
{
    if (str.isEmpty()) return "";

    QStringList tokens;
    QStringList result;
    QString output;

    if(str.contains(" "))
    {
        tokens = str.split(" ");

        for(auto str : tokens)
        {
            str = str.toLower();
            str[0] = str[0].toUpper();
            result<<str;
        }

        output = result.join(" ");
    }else output = str;


    return output.simplified();
}

inline QString fixString (const QString &str)
{

    //title.remove(QRegExp(QString::fromUtf8("[·-`~!@#$%^&*()_—+=|:;<>«»,.?/{}\'\"\\\[\\\]\\\\]")));
    QString title=str;

    title=title.contains("(")&&title.contains(")")?fixTitle(title,"(",")"):title;
    title=title.contains("[")&&title.contains("]")?fixTitle(title,"[","]"):title;
    title=title.contains("{")&&title.contains("}")?fixTitle(title,"{","}"):title;
    title=title.contains("ft")?removeSubstring(title, "ft"):title;
    title=title.contains("ft.")?removeSubstring(title, "ft."):title;
    title=title.contains("featuring")?removeSubstring(title, "featuring"):title;
    title=title.contains("feat")?removeSubstring(title, "feat"):title;
    title=title.contains("official video")?removeSubstring(title, "official video"):title;
    title=title.contains("live")?removeSubstring(title, "live"):title;
    title=title.contains("...")?title.replace("...",""):title;
    title=title.contains("|")?title.replace("|",""):title;
    title=title.contains('"')?title.replace('"',""):title;
    title=title.contains(":")?title.replace(":",""):title;
    title=title.contains("&")? title.replace("&", "and"):title;
    //qDebug()<<"fixed string:"<<title;

    return ucfirst(title).simplified();
}

inline bool fileExists(const QString &url)
{
    QFileInfo path(url);
    if (path.exists()) return true;
    else return false;
}


}

#endif // UTILS_H
