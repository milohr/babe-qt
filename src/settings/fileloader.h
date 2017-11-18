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
        moveToThread(&t);

        qRegisterMetaType<Bae::DB>("Bae::DB");
        qRegisterMetaType<Bae::TABLE>("Bae::TABLE");
        qRegisterMetaType<QMap<Bae::TABLE,bool>>("QMap<Bae::TABLE,bool>");
        t.start();
    }

    ~FileLoader()
    {
        go=false;
        t.quit();
        t.wait();
    }

    void requestPath(QString path)
    {
        QMetaObject::invokeMethod(this, "getTracks", Q_ARG(QString, path));
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
            QDirIterator it(path, Bae::formats, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) urls<<it.next();
        } else if (QFileInfo(path).isFile()) urls<<path;

        emit collectionSize(urls.size());

        if(urls.size()>0)
        {
            for(auto url : urls)            
                if(go)
                {
                    if(!connection.check_existance(Bae::TABLEMAP[Bae::TABLE::TRACKS],Bae::KEYMAP[Bae::KEY::URL],url))
                    {
                        TagInfo info(url);
                        auto  album = Bae::fixString(info.getAlbum());
                        auto track= info.getTrack();
                        auto title = Bae::fixString(info.getTitle()); /* to fix*/
                        auto artist = Bae::fixString(info.getArtist());
                        auto genre = info.getGenre();
                        auto sourceUrl = QFileInfo(url).dir().path();
                        int duration = info.getDuration();
                        auto year = info.getYear();

                        qDebug()<<"FILE LOADER:"<< title << album << artist <<url;

                        Bae::DB trackMap = {
                            {Bae::KEY::URL,url},
                            {Bae::KEY::TRACK,QString::number(track)},
                            {Bae::KEY::TITLE,title},
                            {Bae::KEY::ARTIST,artist},
                            {Bae::KEY::ALBUM,album},
                            {Bae::KEY::DURATION,QString::number(duration)},
                            {Bae::KEY::GENRE,genre},
                            {Bae::KEY::SOURCES_URL,sourceUrl},
                            {Bae::KEY::BABE, url.startsWith(Bae::YoutubeCachePath)?"1":"0"},
                            {Bae::KEY::RELEASE_DATE,QString::number(year)}};

                        emit trackReady(trackMap);
                        //                            while(this->wait){t.msleep(100);}
                        //                            this->wait=!this->wait;
                    }

                }else break;

        }
        t.msleep(100);
        emit finished();
    }

signals:
    void trackReady(Bae::DB track);
    void finished();
    void collectionSize(int size);

private:
    QThread t;
    CollectionDB connection;
    bool go=true;
    bool wait=true;
};


#endif // FILELOADER_H
