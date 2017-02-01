#include "artwork.h"
#include <QUrlQuery>
#include <QToolButton>
#include <QNetworkRequest>
#include <QHttpPart>
#include <QNetworkReply>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QDomDocument>


ArtWork::ArtWork(QObject *parent) : QObject(parent)
{
    url = "http://ws.audioscrobbler.com/2.0/";
}  //



void ArtWork::setData(QString artist, QString album)
{

     if(artist.size()!=0 && album.size()!=0)
     {
         url.append("?method=album.getinfo");
         url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");
         QUrl q_artist (artist);
         q_artist.toEncoded(QUrl::FullyEncoded);
         QUrl q_album (album);
         q_album.toEncoded(QUrl::FullyEncoded);

         if (!q_artist.isEmpty()) url.append("&artist=" + q_artist.toString());
         if (!q_album.isEmpty()) url.append("&album=" + q_album.toString());

         startConnection();
     }else if(artist.size()!=0)
     {
         url.append("?method=artist.getinfo");
         url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");

         QUrl q_artist (artist);
         q_artist.toEncoded(QUrl::FullyEncoded);

         if (!q_artist.isEmpty()) url.append("&artist=" + q_artist.toString());
         type=ALBUM;
         startConnection();

     }

}

void ArtWork::setData(QString artist)
{

    if(artist.size()!=0)
     {
         url.append("?method=artist.getinfo");
         url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");

         QUrl q_artist (artist);
         q_artist.toEncoded(QUrl::FullyEncoded);

         if (!q_artist.isEmpty()) url.append("&artist=" + q_artist.toString());
         type=ARTIST;
         startConnection();

     }

}

void ArtWork::startConnection()
{


        QNetworkAccessManager manager;

        QNetworkReply *reply  = manager.get(QNetworkRequest(QUrl(url)));

        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
        QObject::connect(&manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(xmlInfo(QNetworkReply*)));
        loop.exec();


       qDebug()<<url;
        delete reply;

}

QByteArray ArtWork::getCover()
{

     return coverArray;
}


void ArtWork::dummy()
{
    qDebug()<<"QQQQQQQQQQQQQQQQQQQQ on DUMMYT";


}


/*void ArtWork::onFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
       QByteArray bts = reply->readAll();
       QString xmlData(bts);
      // gotAlbumInfo(xmlData);
    //emit prueba(&str);
    }
}*/

void ArtWork::xmlInfo(QNetworkReply *reply)
{

    if (reply->error() == QNetworkReply::NoError)
    {
       QByteArray bts = reply->readAll();
       QString xmlData(bts);
       //qDebug()<<xmlData;
       QString coverUrl;
       QString artistHead;
       //QString info;
       //qDebug()<<xmlData;
       QDomDocument doc;

       if (!doc.setContent(xmlData))
       {
         qDebug()<<"The XML obtained from last.fm "
                            "is invalid.";
       }else
       {
        // Let's just admit that Qt's XML parsing is heinously ugly.
        // What we're looking for is something like
        // 	<album><image size="extralarge"> ... </image></album>

           if(type==ALBUM)
           {

                const QDomNodeList list = doc.documentElement().namedItem("album").childNodes();

                for (int i = 0; i < list.count(); i++)
                {
                    QDomNode n = list.item(i);
                    if (n.nodeName() != "image") continue;
                    if (!n.hasAttributes()) continue;

                    QString imageSize =  n.attributes().namedItem("size").nodeValue();
                    if (imageSize == "extralarge")
                    if (n.isElement())
                    {
                        coverUrl = n.toElement().text();
                        break;
                    }
                 }

                const QDomNodeList list2 = doc.documentElement().namedItem("album").childNodes();

                for (int i = 0; i < list2.count(); i++)
                {
                    QDomNode n = list2.item(i);
                    if(n.isElement())
                    {
                        if(n.nodeName()=="wiki")
                        {
                            qDebug()<<n.nodeName();
                            info=n.childNodes().item(1).toElement().text();
                            //qDebug()<<n.firstChildElement().toElement().text();
                             // <<n.toElement().text();
                        }

                     }
                }
           }else if(type==ARTIST)
           {

               const QDomNodeList list3 = doc.documentElement().namedItem("artist").childNodes();
               for (int i = 0; i < list3.count(); i++)
               {
                   QDomNode n = list3.item(i);
                   if (n.nodeName() != "image") continue;
                   if (!n.hasAttributes()) continue;

                   QString imageSize = n.attributes().namedItem("size").nodeValue();
                   if (imageSize == "extralarge")
                       if (n.isElement())
                       {
                           artistHead = n.toElement().text();
                           break;
                       }
               }

               const QDomNodeList list4 = doc.documentElement().namedItem("artist").childNodes();

               for (int i = 0; i < list4.count(); i++)
               {
                   QDomNode n = list4.item(i);
                   if(n.isElement())
                   {
                       if(n.nodeName()=="bio")
                       {
                           qDebug()<<n.nodeName();
                           bio=n.childNodes().item(2).toElement().text();
                           //qDebug()<<n.firstChildElement().toElement().text();
                            // <<n.toElement().text();
                       }

                    }
               }
           }

        }

    // No cover URL?  This could be a problem.
    // Better let the user know what's going on.
        if (coverUrl.isEmpty())
        {
            qDebug()<<"Could not find " <<
                        " cover "
                        "for \"" << album << "\".";
        }else
        {
            qDebug()<<"the cover art url is"<< coverUrl;
            this->coverArray = selectCover(coverUrl);
            selectInfo(info);
        }


        if(artistHead.isEmpty())
        {
               qDebug()<<"Could not find " <<
                           " head "
                           "for \"" << artist << "\".";
        }else
        {
            qDebug()<<"the head art url is"<< artistHead;
            if(!bio.isEmpty()) emit bioReady(bio);
          selectHead(artistHead);

        }

    }
}

QByteArray ArtWork::selectCover(QString url)
{
    qDebug()<<"trying to get the cover";
    QNetworkAccessManager manager;

     QNetworkReply *reply  = manager.get(QNetworkRequest(QUrl(url)));

     QEventLoop loop;

      QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
      QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
      loop.exec();
       QByteArray downloaded (reply->readAll());
      //emit coverReady(downloaded);
     delete reply;
 emit coverReady(downloaded);
       return downloaded;
}

void ArtWork::selectHead(QString url)
{
    qDebug()<<"trying to get the cover";
    QNetworkAccessManager manager;

     QNetworkReply *reply  = manager.get(QNetworkRequest(QUrl(url)));

     QEventLoop loop;

      QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
      QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
      loop.exec();
       QByteArray downloaded (reply->readAll());
      //emit coverReady(downloaded);
     delete reply;
 emit headReady(downloaded);

}

void ArtWork::selectInfo(QString info)
{
   this->info=info;

}

QString ArtWork::getInfo()
{
    return info;
}



