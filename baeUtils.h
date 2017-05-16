#ifndef BAEUTILS_H
#define BAEUTILS_H

#include "string"
#include <QString>
#include <QDebug>
#include <QStandardPaths>
#include <QFileInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <cmath>
using namespace std;

namespace BaeUtils
{

enum ALbumSizeHint {BIG_ALBUM=200,MEDIUM_ALBUM=120,SMALL_ALBUM=80};

static const int MAX_ALBUM_SIZE=300;
static const int MIN_ALBUM_SIZE=80;

static const double BIG_ALBUM_FACTOR = 0.039;
static const double MEDIUM_ALBUM_FACTOR = 0.013;
static const double SMALL_ALBUM_FACTOR = 0.006;

static inline int getWidgetSizeHint(const double &factor, const int &deafultValue)
{
    int ALBUM_SIZE = deafultValue;
    auto screenSize = QApplication::desktop()->availableGeometry().size();
    int albumSizeHint =  static_cast<int>(sqrt((screenSize.height()*screenSize.width())*factor));
    ALBUM_SIZE = albumSizeHint > ALBUM_SIZE? albumSizeHint : ALBUM_SIZE;
    return ALBUM_SIZE;
}

static inline QString getNameFromLocation(const QString &str)
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
    {
        ret.push_back(str[index]);
    }

    return ret;
}

static const QString SettingPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+"/babe/";
static const QString CollectionDBPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/babe/";
static const QString CachePath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)+"/babe/";
static const QString YoutubeCachePath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)+"/babe/youtube/";
static const QString ExtensionFetchingPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
static const QString NotifyDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
static const QStringList MoodColors = {"#F0FF01","#01FF5B","#3DAEFD","#B401FF","#E91E63"};


static inline QString fixTitle(const QString &title,const QString &s,const QString &e)
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


static inline QString removeSubstring(const QString &newTitle, const QString &subString)
{
    const int indexFt = newTitle.indexOf(subString, 0, Qt::CaseInsensitive);

    if (indexFt != -1) {
        return newTitle.left(indexFt).simplified();
    }else
    {
        return newTitle;
    }
}

static inline QString ucfirst(const QString &str)
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

static inline QString fixString (const QString &str)
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

static inline bool fileExists(const QString &url)
{
    QFileInfo path(url);
    if (path.exists()) return true;
    else return false;
}


}

#endif // UTILS_H
