#include "htmlparser.h"

htmlParser::htmlParser(QObject *parent) : QObject(parent)
{

}

void htmlParser::setHtml(const QByteArray &array)
{
    this->html = array;

}


QString htmlParser::extractProp(const QString &tag,const QString &prop)
{
//    qDebug()<<"extractProp"<<tag;
    auto list = tag.split(" ");
    auto result =list.filter(prop,Qt::CaseInsensitive);
//    qDebug()<<result;
    auto url = result.first().replace(prop,"").replace('\"',"");
//    qDebug()<<url;
    return url;

}

QStringList htmlParser::parseTag(const QString &tagRef, const QString &attribute)
{

    QStringList results;
    QStringList html(QString(this->html).split(">"));

    for(auto i =0; i<html.size();i++)
    {
        QString tag = html.at(i);
        tag+=">";


        if(findTag(tag,"<"+tagRef+">") && tag.contains(attribute))
        {
            QString subResult;
//            qDebug()<<subResult;
            while(!html.at(i).contains("</"+tagRef))
            {
                auto subTag=html.at(i);
                subTag+=">";
                subResult+=subTag;
                i++;
                if(i>html.size()) break;
            }

            results<<subResult.simplified();
            qDebug()<<subResult;
        }


    }

    return results;
}

/*QStringList htmlParser::parseTag_old(const QString &tagRef)
{
    QString html(this->html);
    QStringList tags;
    int i =0;
    while(html.size()>i)
    {
        if(html.at(i)=="<")
        {
            QString tag;

            while(html.at(i)!=">")
            {
                tag+=html.at(i);
                i++;
                if(i>html.size()) break;
            }
            tag+=">";


            if(findTag(tag,tagRef))
            {
                tags<<tag;
                qDebug()<<tag;
            }

        }

        i++;
        if(i>html.size()) break;
    }

    return tags;
}*/

bool htmlParser::findTag(const QString &txt, const QString &tagRef)
{
    //    qDebug()<<"size of tag<<"<<txt.size();
    int i =0;
    QString subTag;
    while(i<txt.size())
    {
        if(txt.at(i).toLatin1()=='<')
        {
            while(!txt.at(i).isSpace() && txt.at(i).toLatin1()!='>')
            {
                subTag+=txt.at(i);
                i++;
                if(i>txt.size()) break;
            }
            subTag+=">";
        }

        i++;
        if(i>txt.size()) break;
    }


    if(tagRef==subTag) return true;
    else return false;
}
