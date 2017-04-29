#ifndef BAEUTILS_H
#define BAEUTILS_H

#include "string"
#include <QString>
#include <QDebug>
#include <QStandardPaths>
#include <QFileInfo>
using namespace std;

namespace BaeUtils
{
static inline QString getNameFromLocation(QString str)
{
    QString ret;
    int index;

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

static inline QString getSettingPath() { return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)+"/babe/";}
static inline QString getCollectionDBPath() { return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/babe/";}
static inline QString getCachePath() {return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)+"/babe/";}
static inline QString getYoutubeCachePath() { return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)+"/babe/youtube/";}
static inline QString getExtensionFetchingPath() { return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation); }
static inline QString getNotifyDir(){return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);}


static inline QString fixTitle(QString title,QString s,QString e)
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
        }else
        {
            newTitle+=title.at(i);
        }
    }




    return newTitle.simplified();
}


static inline QString removeSubstring(QString newTitle, QString subString)
{
    const int indexFt = newTitle.indexOf(subString, 0, Qt::CaseInsensitive);

    if (indexFt != -1) {
        return newTitle.left(indexFt).simplified();
    }else
    {
        return newTitle;
    }
}

static inline QString ucfirst(const QString str) {
    if (str.size() < 1) {
        return "";
    }

    QStringList tokens = str.split(" ");
   QStringList result;

    for(auto str : tokens)
    {
         if(!str.isEmpty())
         {
             str = str.toLower();
             str[0] = str[0].toUpper();
             result<<str;
         }
    }


    return result.join(" ");
}

static inline QString fixString(QString title)
{
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
    qDebug()<<"fixed string:"<<title;

    return ucfirst(title).simplified();
}

static inline bool fileExists(QString url)
{
    QFileInfo path(url);
    if (path.exists()) return true;
    else return false;
}


}

#endif // UTILS_H
