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
    url.append("?method=album.getinfo");
    url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");


}  //

void ArtWork::setData(QString artist, QString album)
{


    if(artist.size()!=0 && album.size()!=0)
    {
        QObject::connect(this,SIGNAL(test()),this,SLOT(dummy()));
        QUrl q_artist (artist);
        q_artist.toEncoded(QUrl::FullyEncoded);
        QUrl q_album (album);
        q_album.toEncoded(QUrl::FullyEncoded);

        if (!q_artist.isEmpty()) url.append("&artist=" + q_artist.toString());
        if (!q_album.isEmpty()) url.append("&album=" + q_album.toString());

        QNetworkAccessManager manager;

        QNetworkReply *reply  = manager.get(QNetworkRequest(QUrl(url)));

        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
        QObject::connect(&manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(xmlInfo(QNetworkReply*)));
        loop.exec();


       //qDebug()<<str;
        delete reply;
    }

}

QByteArray ArtWork::getCover()
{

     return coverArray;
}


void ArtWork::dummy()
{
    qDebug()<<"QQQQQQQQQQQQQQQQQQQQ on DUMMYT";
    emit bitch();

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
      // gotAlbumInfo(xmlData);
    //emit prueba(&str);


    QString coverUrl;
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
    const QDomNodeList list =
            doc.documentElement().namedItem("album").childNodes();
    for (int i = 0; i < list.count(); i++) {
        QDomNode n = list.item(i);
        if (n.nodeName() != "image") continue;
        if (!n.hasAttributes()) continue;

        QString imageSize =
                n.attributes().namedItem("size").nodeValue();
        if (imageSize == "extralarge")
            if (n.isElement()) {
                coverUrl = n.toElement().text();
                break;
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

    //emit gotCover(coverUrl);
   this->coverArray = selectCover(coverUrl);

    //qDebug()<<"FINISHED!!!!!!!!!!!!!!";

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



