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

enum class TABLE : uint8_t
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
    LOGS=13,
    ALL=14,
    NONE=15

};

static const QMap<TABLE,QString> TABLEMAP =
{
    {TABLE::ALBUMS,"albums"},
    {TABLE::ARTISTS,"artists"},
    {TABLE::MOODS,"moods"},
    {TABLE::PLAYLISTS,"playlists"},
    {TABLE::SOURCES,"sources"},
    {TABLE::SOURCES_TYPES,"sources_types"},
    {TABLE::TRACKS,"tracks"},
    {TABLE::TRACKS_MOODS,"tracks_moods"},
    {TABLE::TRACKS_PLAYLISTS,"tracks_playlists"},
    {TABLE::TAGS,"tags"},
    {TABLE::ALBUMS_TAGS,"albums_tags"},
    {TABLE::ARTISTS_TAGS,"artists_tags"},
    {TABLE::TRACKS_TAGS,"tracks_tags"},
    {TABLE::LOGS,"logs"}

};

enum class KEY :uint8_t
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
    COMMENT=23,
    ID=24,
    NONE=25
};

typedef QMap<Bae::KEY, QString> DB;
typedef QList<DB> DB_LIST;


//inline bool operator==(const Bae::DB &e1, const Bae::DB &e2)
//{
//    return e1[Bae::KEY::ALBUM] == e2[Bae::KEY::ALBUM]
//            && e1[Bae::KEY::ALBUM] == e2[Bae::KEY::ALBUM];
//}

static const DB KEYMAP =
{
    {KEY::URL,"url"},
    {KEY::SOURCES_URL,"sources_url"},
    {KEY::TRACK,"track"},
    {KEY::TITLE,"title"},
    {KEY::ARTIST,"artist"},
    {KEY::ALBUM,"album"},
    {KEY::DURATION,"duration"},
    {KEY::PLAYED,"played"},
    {KEY::BABE,"babe"},
    {KEY::STARS,"stars"},
    {KEY::RELEASE_DATE,"releaseDate"},
    {KEY::ADD_DATE,"addDate"},
    {KEY::LYRICS,"lyrics"},
    {KEY::GENRE,"genre"},
    {KEY::ART,"art"},
    {KEY::TAG,"tag"},
    {KEY::MOOD,"mood"},
    {KEY::PLAYLIST,"playlist"},
    {KEY::ARTWORK,"artwork"},
    {KEY::WIKI,"wiki"},
    {KEY::SOURCE_TYPE,"source_types_id"},
    {KEY::CONTEXT,"context"},
    {KEY::RETRIEVAL_DATE,"retrieval_date"},
    {KEY::ID,"id"},
    {KEY::COMMENT,"comment"}
};



static const DB TracksColsMap =
{
    {KEY::URL,KEYMAP[KEY::URL]},
    {KEY::SOURCES_URL,KEYMAP[KEY::SOURCES_URL]},
    {KEY::TRACK,KEYMAP[KEY::TRACK]},
    {KEY::TITLE,KEYMAP[KEY::TITLE]},
    {KEY::ARTIST,KEYMAP[KEY::ARTIST]},
    {KEY::ALBUM,KEYMAP[KEY::ALBUM]},
    {KEY::DURATION,KEYMAP[KEY::DURATION]},
    {KEY::PLAYED,KEYMAP[KEY::PLAYED]},
    {KEY::BABE,KEYMAP[KEY::BABE]},
    {KEY::STARS,KEYMAP[KEY::STARS]},
    {KEY::RELEASE_DATE,KEYMAP[KEY::RELEASE_DATE]},
    {KEY::ADD_DATE,KEYMAP[KEY::ADD_DATE]},
    {KEY::LYRICS,KEYMAP[KEY::LYRICS]},
    {KEY::GENRE,KEYMAP[KEY::GENRE]},
    {KEY::ART,KEYMAP[KEY::ART]}
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

    if (indexFt != -1) return newTitle.left(indexFt).simplified();
    else  return newTitle;
}

inline QString ucfirst(const QString &str)/*uppercase first letter*/
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
    title=title.remove(QChar::Null);
    title=title.contains('\u0000')?title.replace('\u0000',""):title;
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
    title=title.contains("|")?removeSubstring(title, "|"):title;
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

inline Bae::TABLE albumType(const Bae::DB &albumMap)
{
    if(albumMap[Bae::KEY::ALBUM].isEmpty() && !albumMap[Bae::KEY::ARTIST].isEmpty())
        return Bae::TABLE::ARTISTS;
    else if(!albumMap[Bae::KEY::ALBUM].isEmpty() && !albumMap[Bae::KEY::ARTIST].isEmpty())
        return Bae::TABLE::ALBUMS;

    return Bae::TABLE::NONE;
}}

#endif // UTILS_H
