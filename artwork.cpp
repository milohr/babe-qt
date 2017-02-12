#include "artwork.h"
#include <QApplication>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkRequest>
#include <QUrl>

ArtWork::ArtWork(QObject *parent) : QObject(parent) {
    url = "http://ws.audioscrobbler.com/2.0/";
} //

void ArtWork::setDataCover(QString artist, QString album,QString title, QString path) {
    this->artist = artist;
    this->album = album;
    this->title=title;
    this->path = path;

    qDebug()<<"Going to try and get the art cover for: "<< album <<"by"<<artist<<title;


    if(!album.contains("UNKNOWN"))
    {

        if (!artist.isEmpty() && !album.isEmpty()) {
            url.append("?method=album.getinfo");
            url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");
            QUrl q_artist(artist.replace("&", "and"));
            q_artist.toEncoded(QUrl::FullyEncoded);
            QUrl q_album(album.replace("&", "and"));
            q_album.toEncoded(QUrl::FullyEncoded);

            if (!q_artist.isEmpty())
                url.append("&artist=" + q_artist.toString());
            if (!q_album.isEmpty())
                url.append("&album=" + q_album.toString());
            type = ALBUM;
            // qDebug()<<"on setDataCover:"<<url;
            startConnection();
        }else if(!title.isEmpty()&&!artist.isEmpty())
        {
            setDataCover_title(artist,title);
        }
    }else if(!title.isEmpty()&&!artist.isEmpty())
    {
        setDataCover_title(artist,title);
    }
}



QString ArtWork::getAlbumTitle(QString artist, QString title) {

     QString title_album;
    this->artist = artist;

    this->title=title;

    qDebug()<<"Going to try and get the album name for: "<< artist <<"as"<<title;

    url = "http://ws.audioscrobbler.com/2.0/";

    title=title.contains("(")?fixTitle(title,"(",")"):title;

    title=title.contains("[")?fixTitle(title,"[","]"):title;
    title=title.contains("ft")?removeFeat(title):title;
    title=title.contains("feat")?removeFeat(title):title;
    title=title.contains("official video")?removeOfficial(title):title;

    qDebug()<<"fixing the title string in order to get album title:"<<title;
    if (!artist.isEmpty() && !title.isEmpty()) {
        url.append("?method=track.getinfo");
        url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");
        QUrl q_artist(artist.replace("&", "and"));
        q_artist.toEncoded(QUrl::FullyEncoded);
        QUrl q_title(title.replace("&", "and"));
        q_title.toEncoded(QUrl::FullyEncoded);

        if (!q_artist.isEmpty())
            url.append("&artist=" + q_artist.toString());
        if (!q_title.isEmpty())
            url.append("&track=" + q_title.toString());
        type = ALBUM_TITLE;
        qDebug()<<"trying to get album name by_title:"<<url;



        QNetworkAccessManager manager;

        QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
                         SLOT(quit()));

        loop.exec();



        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray bts = reply->readAll();
            QString xmlData(bts);
            QDomDocument doc;

            if (!doc.setContent(xmlData)) {
                qDebug() << "The XML obtained from last.fm "
                            "is invalid.";
            } else {

                const QDomNodeList list =
                        doc.documentElement().namedItem("track").childNodes();

                for (int i = 0; i < list.count(); i++) {
                    QDomNode n = list.item(i);
                    if (n.isElement()) {
                        if (n.nodeName() == "album")
                        {
                            title_album = n.childNodes().item(1).toElement().text();

                        }
                    }
                }
                //qDebug()<<coverUrl;
                if (title_album.isEmpty()) {
                    qDebug() << "Could not find "
                             << " album title "
                                "for \""
                             << title << artist<<"\".";
                }else
                {
                    qDebug()<<title_album;
                }


            }
        }


        delete reply;

    }

    return title_album;
}



QString ArtWork::fixTitle(QString title,QString s,QString e)
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

QString ArtWork::removeFeat(QString newTitle)
{
    QString result;

    if(newTitle.contains("ft"))
    {
        qDebug()<<"new title still constains unfixed string ft";
        for(int i=0; i<newTitle.size();i++)
        {

            if(newTitle.at(i)=="f"&&newTitle.at(i+1)=="t")
            {
                break;

            }else
            {
                result+=newTitle.at(i);
            }
        }

    }else if (newTitle.contains("feat"))
    {
        for(int i=0; i<newTitle.size();i++)
        {

            if(newTitle.at(i)=="f"&&newTitle.at(i+1)=="e"&&newTitle.at(i+2)=="a"&&newTitle.at(i+3)=="t")
            {
                break;

            }else
            {
                result+=newTitle.at(i);
            }
        }
    }
    return result.simplified();
}

QString ArtWork::removeOfficial(QString newTitle)
{
    QString result;

    if(newTitle.contains("official video"))
    {
        qDebug()<<"new title still constains unfixed string official video";

        result=newTitle.section("official video",0,-2);

    }
    return result.simplified();
}

void ArtWork::setDataCover_title(QString artist, QString title) {
    qDebug()<<"Going to try and get the art cover from title: "<< title <<"by"<<artist;
    url = "http://ws.audioscrobbler.com/2.0/";




    title=title.contains("(")?fixTitle(title,"(",")"):title;

    title=title.contains("[")?fixTitle(title,"[","]"):title;
    title=title.contains("ft")?removeFeat(title):title;
    title=title.contains("feat")?removeFeat(title):title;
    title=title.contains("official video")?removeOfficial(title):title;
    qDebug()<<"fixing the title string:"<<title;
    if (artist.size() != 0 && title.size() != 0) {
        url.append("?method=track.getinfo");
        url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");
        QUrl q_artist(artist.replace("&", "and"));
        q_artist.toEncoded(QUrl::FullyEncoded);
        QUrl q_title(title.replace("&", "and"));
        q_title.toEncoded(QUrl::FullyEncoded);

        if (!q_artist.isEmpty())
            url.append("&artist=" + q_artist.toString());
        if (!q_title.isEmpty())
            url.append("&track=" + q_title.toString());
        type = ALBUM_by_TITLE;
        qDebug()<<"trying to get cover by_title:"<<url;
        startConnection();
    }
}

void ArtWork::setDataHead_asCover(QString artist, QString path) {
    this->artist = artist;
    this->path = path;
    url = "http://ws.audioscrobbler.com/2.0/";
    if (artist.size() != 0) {
        url.append("?method=artist.getinfo");
        url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");

        QUrl q_artist(artist);
        q_artist.toEncoded(QUrl::FullyEncoded);

        if (!q_artist.isEmpty())
            url.append("&artist=" + q_artist.toString());
        type = ARTIST_COVER;
        qDebug()<<url;

        startConnection();
    }
}

void ArtWork::setDataHead(QString artist, QString path) {
    this->artist = artist;
    this->path = path;

    if (artist.size() != 0) {
        url.append("?method=artist.getinfo");
        url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");

        QUrl q_artist(artist);
        q_artist.toEncoded(QUrl::FullyEncoded);

        if (!q_artist.isEmpty())
            url.append("&artist=" + q_artist.toString());
        type = ARTIST;
        startConnection();
    }
}

void ArtWork::setDataCoverInfo(QString artist, QString album) {
    this->artist = artist;
    this->album = album;

    if (artist.size() != 0 && album.size() != 0) {
        url.append("?method=album.getinfo");
        url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");
        QUrl q_artist(artist.replace("&", "and"));
        q_artist.toEncoded(QUrl::FullyEncoded);
        QUrl q_album(album.replace("&", "and"));
        q_album.toEncoded(QUrl::FullyEncoded);

        if (!q_artist.isEmpty())
            url.append("&artist=" + q_artist.toString());
        if (!q_album.isEmpty())
            url.append("&album=" + q_album.toString());
        type = ALBUM_INFO;
        startConnection();
    }
}

void ArtWork::setDataHeadInfo(QString artist) {
    this->artist = artist;

    if (artist.size() != 0) {
        url.append("?method=artist.getinfo");
        url.append("&api_key=ba6f0bd3c887da9101c10a50cf2af133");

        QUrl q_artist(artist);
        q_artist.toEncoded(QUrl::FullyEncoded);

        if (!q_artist.isEmpty())
            url.append("&artist=" + q_artist.toString());
        type = ARTIST_INFO;
        startConnection();
    }
}

void ArtWork::startConnection() {

    QNetworkAccessManager manager;

    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
                     SLOT(quit()));
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), this,
                     SLOT(xmlInfo(QNetworkReply *)));
    loop.exec();

    // qDebug()<<url;
    delete reply;
}

QByteArray ArtWork::getCover() { return coverArray; }

void ArtWork::dummy() { qDebug() << "QQQQQQQQQQQQQQQQQQQQ on DUMMYT"; }

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

void ArtWork::saveArt(QByteArray array) {
    QImage img;
    img.loadFromData(array);
    QString name = album.size() > 0 ? artist + "_" + album : artist;
    name.replace("/", "-");
    name.replace("&", "-");
    QString format = "JPEG";
    if (img.save(path + name + ".jpg", format.toLatin1(), 100)) {
        if (album.isEmpty())
            emit artSaved(path + name + ".jpg", {artist});
        else
            emit artSaved(path + name + ".jpg", {album, artist});
    } else {
        qDebug() << "couldn't save artwork";

        if (album.isEmpty())
            emit artSaved("", {artist});
        else
            emit artSaved("", {album, artist});
    }
}

void ArtWork::xmlInfo(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bts = reply->readAll();
        QString xmlData(bts);
        // qDebug()<<xmlData;
        QString coverUrl;
        QString artistHead;
        // QString info;
        // qDebug()<<xmlData;
        QDomDocument doc;

        if (!doc.setContent(xmlData)) {
            qDebug() << "The XML obtained from last.fm "
                        "is invalid.";
        } else {
            // Let's just admit that Qt's XML parsing is heinously ugly.
            // What we're looking for is something like
            // 	<album><image size="extralarge"> ... </image></album>

            if (type == ALBUM) {

                const QDomNodeList list =
                        doc.documentElement().namedItem("album").childNodes();

                for (int i = 0; i < list.count(); i++) {
                    QDomNode n = list.item(i);
                    if (n.nodeName() != "image")
                        continue;
                    if (!n.hasAttributes())
                        continue;

                    QString imageSize = n.attributes().namedItem("size").nodeValue();
                    if (imageSize == "extralarge")
                        if (n.isElement()) {
                            coverUrl = n.toElement().text();
                            break;
                        }
                }

                if (coverUrl.isEmpty()) {
                    qDebug() << "Could not find "
                             << " cover "
                                "for \""
                             << album << "\".";
                    setDataCover_title(artist,title);
                }else
                {
                    qDebug() << "the cover art url is" << coverUrl;

                    this->coverArray = selectCover(coverUrl);
                }
                // selectInfo(info);

            }else if(type == ALBUM_by_TITLE)
            {

                const QDomNodeList list =
                        doc.documentElement().namedItem("track").childNodes();

                for (int i = 0; i < list.count(); i++) {
                    QDomNode n = list.item(i);
                    if (n.isElement()) {
                        if (n.nodeName() == "album")
                        {
                            auto list2=n.childNodes();
                            for(int j=0; j<list2.size();j++)
                            {
                                auto m= list2.item(j);
                                //qDebug()<<m.nodeName();
                                if(m.nodeName().contains("image"))
                                {
                                    QString imageSize = m.attributes().namedItem("size").nodeValue();
                                    if (imageSize == "extralarge")
                                        if (m.isElement()) {
                                            coverUrl = m.toElement().text();
                                            break;
                                        }
                                }
                            }
                        }
                    }
                }
                //qDebug()<<coverUrl;
                if (coverUrl.isEmpty()) {
                    qDebug() << "Could not find "
                             << " cover by title "
                                "for \""
                             << album << "\".";

                    setDataHead_asCover(artist);


                }else{

                    // qDebug() << "the cover art url is" << coverUrl;

                    this->coverArray = selectCover(coverUrl);
                }


            }else if (type == ALBUM_INFO) {
                const QDomNodeList list2 =
                        doc.documentElement().namedItem("album").childNodes();

                for (int i = 0; i < list2.count(); i++) {
                    QDomNode n = list2.item(i);
                    if (n.isElement()) {
                        if (n.nodeName() == "wiki") {
                            // qDebug()<<n.nodeName();
                            info = n.childNodes().item(1).toElement().text();
                            // qDebug()<<n.firstChildElement().toElement().text();
                            // <<n.toElement().text();
                        }
                    }
                }

                if (info.isEmpty()) {
                    qDebug() << "Could not find "
                             << " info "
                                "for \""
                             << album << "\".";
                }

                emit infoReady(info);

            } else if (type == ARTIST_COVER)
            {
                qDebug()<<"trying to get cover now by artistHead"<<artist;
                const QDomNodeList list3 =
                        doc.documentElement().namedItem("artist").childNodes();
                for (int i = 0; i < list3.count(); i++) {
                    QDomNode n = list3.item(i);
                    if (n.nodeName() != "image")
                        continue;
                    if (!n.hasAttributes())
                        continue;

                    QString imageSize = n.attributes().namedItem("size").nodeValue();
                    if (imageSize == "extralarge")
                        if (n.isElement()) {
                            artistHead = n.toElement().text();
                            break;
                        }
                }

                if (artistHead.isEmpty()) {
                    qDebug() << "Could not find "
                             << " head "
                                "for \""
                             << artist << "\".";
                }
                selectCover(artistHead);
            }else if (type == ARTIST) {

                const QDomNodeList list3 =
                        doc.documentElement().namedItem("artist").childNodes();
                for (int i = 0; i < list3.count(); i++) {
                    QDomNode n = list3.item(i);
                    if (n.nodeName() != "image")
                        continue;
                    if (!n.hasAttributes())
                        continue;

                    QString imageSize = n.attributes().namedItem("size").nodeValue();
                    if (imageSize == "extralarge")
                        if (n.isElement()) {
                            artistHead = n.toElement().text();
                            break;
                        }
                }

                if (artistHead.isEmpty()) {
                    qDebug() << "Could not find "
                             << " head "
                                "for \""
                             << artist << "\".";
                }
                selectHead(artistHead);


            } else if (type == ARTIST_INFO) {
                const QDomNodeList list4 =
                        doc.documentElement().namedItem("artist").childNodes();

                for (int i = 0; i < list4.count(); i++) {
                    QDomNode n = list4.item(i);
                    if (n.isElement()) {
                        if (n.nodeName() == "bio") {
                            // qDebug()<<n.nodeName();
                            bio = n.childNodes().item(2).toElement().text();
                            // qDebug()<<n.firstChildElement().toElement().text();
                            // <<n.toElement().text();
                        }
                    }
                }

                if (bio.isEmpty()) {
                    qDebug() << "Could not find "
                             << " head info "
                                "for \""
                             << artist << "\".";
                }
                emit bioReady(bio);
                // selectHead(artistHead);
            }
        }

        // No cover URL?  This could be a problem.
        // Better let the user know what's going on.

    } else {
        qDebug() << "Error in parser :(";
        if (type == ALBUM) {
            if (album.isEmpty())
                emit artSaved("", {artist});
            else
                emit artSaved("", {album, artist});
        }
    }
}

QByteArray ArtWork::selectCover(QString url) {
    // qDebug()<<"trying to get the cover"<<url;
    QNetworkAccessManager manager;

    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

    QEventLoop loop;

    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
                     SLOT(quit()));
    loop.exec();
    QByteArray downloaded(reply->readAll());
    // emit coverReady(downloaded);
    delete reply;
    emit coverReady(downloaded);
    return downloaded;
}

void ArtWork::selectHead(QString url) {
    // qDebug()<<"trying to get the head";
    QNetworkAccessManager manager;

    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

    QEventLoop loop;

    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
                     SLOT(quit()));
    loop.exec();
    QByteArray downloaded(reply->readAll());
    // emit coverReady(downloaded);
    delete reply;
    emit headReady(downloaded);
}

void ArtWork::selectInfo(QString info) { this->info = info; }

QString ArtWork::getInfo() { return info; }

QString ArtWork::getAlbumTitle(QString info)
{

}
