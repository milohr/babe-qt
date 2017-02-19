#include "artwork.h"
#include <QApplication>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QVariantMap>

ArtWork::ArtWork(QObject *parent) : QObject(parent) {
    url = "http://ws.audioscrobbler.com/2.0/";
} //

void ArtWork::setDataCover(QString artist, QString album,QString title, QString path) {
    this->artist = artist;
    this->album = album;
    this->title=title;
    this->path = path;

    title = fixString(title);
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
                url.append("&album=" + q_album.toString()); ///maybe this doens't needs to be encoded
            type = ALBUM;
            qDebug()<<"thealbum name is:"<<q_album.toString();
            qDebug()<<"on setDataCover:"<<url;
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


void ArtWork::setDataCover_spotify(QString artist, QString album,QString title)
{

    qDebug()<<"Going to try and get the art cover from title by spotify service: "<< title <<"by"<<artist;
    url = "https://api.spotify.com/v1/search?q=";

    title=fixString(title);

    if (!artist.isEmpty() && !title.isEmpty()) {
        url.append("track:");
        QUrl q_title(title.replace("&", "and"));
        q_title.toEncoded(QUrl::FullyEncoded);
        if (!q_title.isEmpty())
            url.append(q_title.toString());


        url.append("%20artist:");
        QUrl q_artist(artist.replace("&", "and"));
        q_artist.toEncoded(QUrl::FullyEncoded);

        if (!q_artist.isEmpty())
            url.append(q_artist.toString());

        url.append("&type=track");

       // qDebug()<<"spotify api url:"<<url;
        type = ALBUM_by_SPOTIFY;
        //qDebug()<<"trying to get cover by_title:"<<url;
        bool json =true;
        startConnection(json);
    }

}

QString ArtWork::fixString(QString title)
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
    qDebug()<<"fixing the title string in order to get album title:"<<title;

    return title;
}



QString ArtWork::getAlbumTitle_Spotify(QString artist, QString title)
{
    QString title_album;
    this->artist = artist;

    this->title=title;

    qDebug()<<"Going to try and get the albumt title from spotify service: "<< title <<"by"<<artist;
    url = "https://api.spotify.com/v1/search?q=";

    title=fixString(title);

    if (!artist.isEmpty() && !title.isEmpty()) {
        url.append("track:");
        QUrl q_title(title.replace("&", "and"));
        q_title.toEncoded(QUrl::FullyEncoded);
        if (!q_title.isEmpty())
            url.append(q_title.toString());


        url.append("%20artist:");
        QUrl q_artist(artist.replace("&", "and"));
        q_artist.toEncoded(QUrl::FullyEncoded);

        if (!q_artist.isEmpty())
            url.append(q_artist.toString());

        url.append("&type=track");

        qDebug()<<"spotify api url:"<<url;

        qDebug()<<"trying to get cover by_title:"<<url;

    }
    qDebug("getAlbumTitle_Spotify about to connect to network");

    QNetworkAccessManager manager;

    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
                     SLOT(quit()));

    loop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {

        QString strReply = (QString)reply->readAll();
        QJsonParseError jsonParseError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8(), &jsonParseError);


        if (jsonParseError.error != QJsonParseError::NoError) {
            qDebug() << "Error happened:" << jsonParseError.errorString();

        }else
        {

            if (!jsonResponse.isObject()) {
                qDebug() << "The json data is not an object";

            }else
            {

                QJsonObject mainJsonObject(jsonResponse.object());
                auto data = mainJsonObject.toVariantMap();

                auto itemMap = data.value("tracks").toMap().value("items");

                if(!itemMap.isNull())
                {
                    QList<QVariant> items     = itemMap.toList();


                if(!items.isEmpty()) title_album =items.at(0).toMap().value("album").toMap().value("name").toString();
                }

                if(!title_album.isEmpty())
                {
                    qDebug()<<"the album title is: "<<title_album;
                }else

                {
                   qDebug()<<"couldn't find album name from spotify api";
                }


            }
        }
    }

return title_album;

}



QString ArtWork::getAlbumTitle(QString artist, QString title) {

    QString title_album;
    this->artist = artist;

    this->title=title;

    qDebug()<<"Going to try and get the album name for: "<< artist <<"as"<<title;

    url = "http://ws.audioscrobbler.com/2.0/";

    title=fixString(title);


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
                    title_album=getAlbumTitle_Spotify(artist,title);
                }else
                {
                    qDebug()<<title_album;
                }


            }
        }


        delete reply;

    }

    qDebug()<<"the ALBUM TITLE FOR THE TRACK FINAL IS:"<<title_album;
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

QString ArtWork::removeSubstring(QString newTitle, QString subString)
{
    const int indexFt = newTitle.indexOf(subString, 0, Qt::CaseInsensitive);

    if (indexFt != -1) {
        return newTitle.left(indexFt).simplified();
    }else
    {
        return newTitle;
    }
}

QString ArtWork::removeSubstring_old(QString newTitle,QString subString)
{
    QString result;
    newTitle=newTitle.toLower();
    subString=subString.toLower();
    if(newTitle.contains(subString))
    {
        qDebug()<<"new title still constains unfixed string: "<< subString;

        result=newTitle.section(subString,0,-2);

    }
    return result.simplified();
}

void ArtWork::setDataCover_title(QString artist, QString title) {
    qDebug()<<"Going to try and get the art cover from title: "<< title <<"by"<<artist;
    url = "http://ws.audioscrobbler.com/2.0/";

    title=fixString(title);

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
        type = ALBUM_by_TITLE;
        qDebug()<<"trying to get cover by_title:"<<url;
        startConnection();
    }
}

void ArtWork::setDataHead_asCover(QString artist) {
    this->artist = artist;

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

void ArtWork::startConnection(bool json) {

    QNetworkAccessManager manager;

    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop,
                     SLOT(quit()));


    if(!json)
    {QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), this,
                      SLOT(xmlInfo(QNetworkReply *)));
    }else
    {
        qDebug()<<"trying to connect thgrough json";
        QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), this,
                         SLOT(jsonInfo(QNetworkReply *)));
    }
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

    qDebug()<<"trying to save the array";
    if(!array.isNull()&&!array.isEmpty())
    {
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
}



void ArtWork::jsonInfo(QNetworkReply *reply)
{

qDebug()<<"i'm in json";
    if (reply->error() == QNetworkReply::NoError)
    {

        QString strReply = (QString)reply->readAll();
        QJsonParseError jsonParseError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8(), &jsonParseError);


        if (jsonParseError.error != QJsonParseError::NoError) {
            qDebug() << "Error happened:" << jsonParseError.errorString();

        }else
        {

            if (!jsonResponse.isObject()) {
                qDebug() << "The json data is not an object";

            }else
            {

                QJsonObject mainJsonObject(jsonResponse.object());
                auto data = mainJsonObject.toVariantMap();


                if(type== ALBUM_by_SPOTIFY)
                {
                    QString img;
                    qDebug()<<"ALBUM_by_SPOTIFY";

                     auto itemMap = data.value("tracks").toMap().value("items");

                    if(!itemMap.isNull())
                    {
                        QList<QVariant> items     = itemMap.toList();


                    if(!items.isEmpty()) img =items.at(0).toMap().value("album").toMap().value("images").toList().at(0).toMap().value("url").toString();
                    }


                    if(!img.isEmpty())
                    {
                        this->coverArray = selectCover(img);
                        qDebug()<<"ALBUM_by_SPOTIFY placing covver array";
                    }else

                    {
                        setDataHead_asCover(artist);

                    }

                }else if(type== ALBUM_by_ITUNES)
                {

                }
            }
        }
    }else
    {
        qDebug()<<"eror in network reply in jsonInfo";
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
                    setDataCover_spotify(artist,album,title);
                    //setDataHead_asCover(artist);


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
                this->coverArray=  selectCover(artistHead);
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
        qDebug() << "Error in parser :("; //this needs some more work and fixes

            if (album.isEmpty())
                emit artSaved("", {artist});
            else
                emit artSaved("", {album, artist});

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

