#ifndef BAEUTILS_H
#define BAEUTILS_H

#include "string"
#include <QString>
#include <QDebug>

using namespace std;


static string getNameFromLocation(string str)
{
    string ret;
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



static QString fixTitle(QString title,QString s,QString e)
{
    QString newTitle;
    for(int i=0; i<title.size();i++)
    {

        if(title.at(i)==s)
        {
            while(title.at(i)!=e) i++;
        }else
        {
            newTitle+=title.at(i);
        }
    }




    return newTitle.simplified();
}


static QString removeSubstring(QString newTitle, QString subString)
{
    const int indexFt = newTitle.indexOf(subString, 0, Qt::CaseInsensitive);

    if (indexFt != -1) {
        return newTitle.left(indexFt).simplified();
    }else
    {
        return newTitle;
    }
}

static QString fixString(QString title)
{

    //title.remove(QRegExp(QString::fromUtf8("[·-`~!@#$%^&*()_—+=|:;<>«»,.?/{}\'\"\\\[\\\]\\\\]")));


    title=title.contains("(")?fixTitle(title,"(",")"):title;
    title=title.contains("[")?fixTitle(title,"[","]"):title;
    title=title.contains("{")?fixTitle(title,"{","}"):title;
    title=title.contains("ft")?removeSubstring(title, "ft"):title;
    title=title.contains("ft.")?removeSubstring(title, "ft."):title;
    title=title.contains("featuring")?removeSubstring(title, "featuring"):title;
    title=title.contains("feat")?removeSubstring(title, "feat"):title;
    title=title.contains("official video")?removeSubstring(title, "official video"):title;
    title=title.contains("live")?removeSubstring(title, "live"):title;
    title=title.contains("...")?title.replace("...",""):title;
    title=title.contains("|")?title.replace("|",""):title;
    title=title.contains('"')?title.replace('"',""):title;
    qDebug()<<"fixed string:"<<title;

    return title;
}

#endif // UTILS_H
