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

namespace Bae {

enum SearchT
{
    LIKE, SIMILAR
};

typedef QMap<Bae::SearchT,QString> SEARCH;

static const SEARCH SearchTMap {
    {Bae::SearchT::LIKE,"like"},
    {Bae::SearchT::SIMILAR,"similar"}
};

enum Order
{
    DESC, ASC
};

static const QMap<Order,QString> OrderMap =
{
    {Order::DESC, "DESC"},
    {Order::ASC,"ASC"}
};

enum class DBTables : uint8_t
{
    ALBUMS=0,
    ARTISTS=1,
    MOODS=2,
    PLAYLISTS=3,
    SOURCES=4,
    SOURCES_TYPES=5,
    TRACKS=6,
    TRACKS_MOODS=7,
    TRACKS_PLAYLISTS=8,
    TAGS=9,
    ALBUMS_TAGS=10,
    ARTISTS_TAGS=11,
    TRACKS_TAGS=12,
    ALL=13,
    NONE=14

};

static const QMap<DBTables,QString> DBTablesMap =
{
    {DBTables::ALBUMS,"albums"},
    {DBTables::ARTISTS,"artists"},
    {DBTables::MOODS,"moods"},
    {DBTables::PLAYLISTS,"playlists"},
    {DBTables::SOURCES,"sources"},
    {DBTables::SOURCES_TYPES,"sources_types"},
    {DBTables::TRACKS,"tracks"},
    {DBTables::TRACKS_MOODS,"tracks_moods"},
    {DBTables::TRACKS_PLAYLISTS,"tracks_playlists"},
    {DBTables::TAGS,"tags"},
    {DBTables::ALBUMS_TAGS,"albums_tags"},
    {DBTables::ARTISTS_TAGS,"artists_tags"},
    {DBTables::TRACKS_TAGS,"tracks_tags"}

};

enum class DBCols :uint8_t
{
    URL=0,
    SOURCES_URL=1,
    TRACK=2,
    TITLE=3,
    ARTIST=4,
    ALBUM=5,
    DURATION=6,
    PLAYED=7,
    BABE=8,
    STARS=9,
    RELEASE_DATE=10,
    ADD_DATE=11,
    LYRICS=12,
    GENRE=13,
    ART=14,
    TAG=15,
    MOOD=16,
    PLAYLIST=17,
    ARTWORK=18,
    WIKI=19,
    SOURCE_TYPE=20,
    CONTEXT=21,
    RETRIEVAL_DATE=22,
    NONE=23
};

typedef QMap<Bae::DBCols, QString> DB;
typedef QList<DB> DB_LIST;


//inline bool operator==(const Bae::DB &e1, const Bae::DB &e2)
//{
//    return e1[Bae::DBCols::ALBUM] == e2[Bae::DBCols::ALBUM]
//            && e1[Bae::DBCols::ALBUM] == e2[Bae::DBCols::ALBUM];
//}

static const DB DBColsMap =
{
    {DBCols::URL,"url"},
    {DBCols::SOURCES_URL,"sources_url"},
    {DBCols::TRACK,"track"},
    {DBCols::TITLE,"title"},
    {DBCols::ARTIST,"artist"},
    {DBCols::ALBUM,"album"},
    {DBCols::DURATION,"duration"},
    {DBCols::PLAYED,"played"},
    {DBCols::BABE,"babe"},
    {DBCols::STARS,"stars"},
    {DBCols::RELEASE_DATE,"releaseDate"},
    {DBCols::ADD_DATE,"addDate"},
    {DBCols::LYRICS,"lyrics"},
    {DBCols::GENRE,"genre"},
    {DBCols::ART,"art"},
    {DBCols::TAG,"tag"},
    {DBCols::MOOD,"mood"},
    {DBCols::PLAYLIST,"playlist"},
    {DBCols::ARTWORK,"artwork"},
    {DBCols::WIKI,"wiki"},
    {DBCols::SOURCE_TYPE,"source_types_id"},
    {DBCols::CONTEXT,"context"},
    {DBCols::RETRIEVAL_DATE,"retrieval_date"}
};



static const DB TracksColsMap =
{
    {DBCols::URL,DBColsMap[DBCols::URL]},
    {DBCols::SOURCES_URL,DBColsMap[DBCols::SOURCES_URL]},
    {DBCols::TRACK,DBColsMap[DBCols::TRACK]},
    {DBCols::TITLE,DBColsMap[DBCols::TITLE]},
    {DBCols::ARTIST,DBColsMap[DBCols::ARTIST]},
    {DBCols::ALBUM,DBColsMap[DBCols::ALBUM]},
    {DBCols::DURATION,DBColsMap[DBCols::DURATION]},
    {DBCols::PLAYED,DBColsMap[DBCols::PLAYED]},
    {DBCols::BABE,DBColsMap[DBCols::BABE]},
    {DBCols::STARS,DBColsMap[DBCols::STARS]},
    {DBCols::RELEASE_DATE,DBColsMap[DBCols::RELEASE_DATE]},
    {DBCols::ADD_DATE,DBColsMap[DBCols::ADD_DATE]},
    {DBCols::LYRICS,DBColsMap[DBCols::LYRICS]},
    {DBCols::GENRE,DBColsMap[DBCols::GENRE]},
    {DBCols::ART,DBColsMap[DBCols::ART]}
};


enum class AlbumSizeHint : uint {BIG_ALBUM=200,MEDIUM_ALBUM=120,SMALL_ALBUM=80};

static const uint MAX_BIG_ALBUM_SIZE=300;
static const uint MAX_MID_ALBUM_SIZE=200;
static const uint MAX_MIN_ALBUM_SIZE=100;

static const double BIG_ALBUM_FACTOR = 0.039;
static const double BIG_ALBUM_FACTOR_SUBWIDGET = 0.27;

static const double MEDIUM_ALBUM_FACTOR = 0.013;
static const double MEDIUM_ALBUM_FACTOR_SUBWIDGET = 0.4;

static const double SMALL_ALBUM_FACTOR = 0.006;
static const double SMALL_ALBUM_FACTOR_SUBWIDGET = 0.5;

inline uint getWidgetSizeHint(const double &factor, const AlbumSizeHint &deafultValue)
{
    auto ALBUM_SIZE = static_cast<uint>(deafultValue);
    auto screenSize = QApplication::desktop()->availableGeometry().size();
    auto albumSizeHint =  static_cast<uint>(sqrt((screenSize.height()*screenSize.width())*factor));
    ALBUM_SIZE = albumSizeHint > ALBUM_SIZE? albumSizeHint : ALBUM_SIZE;


    switch(deafultValue)
    {

    case AlbumSizeHint::BIG_ALBUM:  return ALBUM_SIZE > MAX_BIG_ALBUM_SIZE? MAX_BIG_ALBUM_SIZE : ALBUM_SIZE;
    case AlbumSizeHint::MEDIUM_ALBUM:  return ALBUM_SIZE > MAX_MID_ALBUM_SIZE? MAX_MID_ALBUM_SIZE : ALBUM_SIZE;
    case AlbumSizeHint::SMALL_ALBUM:  return ALBUM_SIZE > MAX_MIN_ALBUM_SIZE ? MAX_MIN_ALBUM_SIZE :ALBUM_SIZE;
    default: return MAX_MID_ALBUM_SIZE;
    }
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
const QStringList formats {"*.mp4","*.mp3","*.wav","*.flac","*.ogg","*.m4a"};


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

inline QString ucfirst(const QString &str)/*uppercase fist letter*/
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

inline Bae::DBTables albumType(const Bae::DB &albumMap)
{
    if(albumMap[Bae::DBCols::ALBUM].isEmpty() && !albumMap[Bae::DBCols::ARTIST].isEmpty())
        return Bae::DBTables::ARTISTS;
    else if(!albumMap[Bae::DBCols::ALBUM].isEmpty() && !albumMap[Bae::DBCols::ARTIST].isEmpty())
        return Bae::DBTables::ALBUMS;

    return Bae::DBTables::NONE;
}}

#endif // UTILS_H
