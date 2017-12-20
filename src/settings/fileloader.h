#ifndef FILELOADER_H
#define FILELOADER_H

#include <QThread>
#include <QObject>
#include <QDirIterator>

#include "../services/local/taginfo.h"
#include "../db/collectionDB.h"

class FileLoader : public QObject
{
    Q_OBJECT

public:
    FileLoader() : QObject()
    {
        qRegisterMetaType<BAE::DB>("BAE::DB");
        qRegisterMetaType<BAE::TABLE>("BAE::TABLE");
        qRegisterMetaType<QMap<BAE::TABLE, bool>>("QMap<BAE::TABLE,bool>");

        this->con = new CollectionDB(this);
        this->moveToThread(&t);
        t.start();
    }

    ~FileLoader()
    {
        this->go = false;
        this->t.quit();
        this->t.wait();
    }

    void requestPath(QString path)
    {
        this->queue << path;
        for(auto url : this->queue)
        {
            if(!go)
            {
                this->go = true;
                QMetaObject::invokeMethod(this, "getTracks", Q_ARG(QString, url));
                this->queue.removeOne(url);
            }
        }
    }

    void nextTrack()
    {
        this->wait = !this->wait;
    }

public slots:

    void getTracks(QString path)
    {
        qDebug()<<"GETTING TRACKS FROM SETTINGS";

        QStringList urls;

        if (QFileInfo(path).isDir())
        {
            QDirIterator it(path, BAE::formats, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) urls<<it.next();

        } else if (QFileInfo(path).isFile()) urls<<path;

        emit collectionSize(urls.size());

        if(urls.size()>0)
        {
            for(auto url : urls)
            {
                if(go)
                {
                    if(!con->check_existance(BAE::TABLEMAP[BAE::TABLE::TRACKS],BAE::KEYMAP[BAE::KEY::URL],url))
                    {
                        TagInfo info(url);
                        auto album = BAE::fixString(info.getAlbum());
                        auto track= info.getTrack();
                        auto title = BAE::fixString(info.getTitle()); /* to fix*/
                        auto artist = BAE::fixString(info.getArtist());
                        auto genre = info.getGenre();
                        auto sourceUrl = QFileInfo(url).dir().path();
                        auto duration = info.getDuration();
                        auto year = info.getYear();

                        BAE::DB trackMap =
                        {
                            {BAE::KEY::URL,url},
                            {BAE::KEY::TRACK,QString::number(track)},
                            {BAE::KEY::TITLE,title},
                            {BAE::KEY::ARTIST,artist},
                            {BAE::KEY::ALBUM,album},
                            {BAE::KEY::DURATION,QString::number(duration)},
                            {BAE::KEY::GENRE,genre},
                            {BAE::KEY::SOURCES_URL,sourceUrl},
                            {BAE::KEY::BABE, url.startsWith(BAE::YoutubeCachePath)?"1":"0"},
                            {BAE::KEY::RELEASE_DATE,QString::number(year)}
                        };

                        this->con->addTrack(trackMap);

                        //                        emit trackReady(trackMap);
                        //                            while(this->wait){t.msleep(100);}
                        //                            this->wait=!this->wait;
                    }

                }else break;
            }
        }

        this->t.msleep(100);
        emit this->finished();
        this->go = false;
    }

signals:
    void trackReady(BAE::DB track);
    void finished();
    void collectionSize(int size);

private:
    QThread t;
    bool go = false;
    bool wait = true;
    QStringList queue;
    CollectionDB *con;
};


#endif // FILELOADER_H
