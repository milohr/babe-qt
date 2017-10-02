#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QMovie>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QGridLayout>
#include <fstream>
#include <iostream>

#include "notify.h"
#include "baeUtils.h"
#include "youtube.h"
#include "playlist.h"
#include "about.h"
#include "collectionDB.h"
#include "pulpo/pulpo.h"



namespace Ui {class settings;}

class TrackSaver : public QObject
{
    Q_OBJECT

public:
    TrackSaver() : QObject()
    {
        qRegisterMetaType<Bae::DB>("Bae::DB");
        moveToThread(&t);
        t.start();
    }

    ~TrackSaver()
    {
        go=false;
        t.quit();
        t.wait();
    }

    void requestPath(QString path)
    {
        QMetaObject::invokeMethod(this, "getTracks", Q_ARG(QString, path));
    }

    void next()
    {
        this->nextTrack = !this->nextTrack;
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

        for(auto url : urls)
        {
            if(go)
            {
                TagInfo info(url);
                QString  album;
                int track;
                QString title = Bae::fixString(info.getTitle()); /* to fix*/
                QString artist = Bae::fixString(info.getArtist());
                QString genre = info.getGenre();
                QString sourceUrl = QFileInfo(url).dir().path();
                int duration = info.getDuration();
                auto year = info.getYear();

                if(info.getAlbum().isEmpty())
                {
                    qDebug()<<"the album has not title, so i'm going to try and get it.";
                    info.writeData(); // actually means to search the name and load it into metadata
                    album=info.getAlbum();
                    track=info.getTrack();
                }else
                {
                    album=info.getAlbum();
                    track=info.getTrack();
                }

                album=Bae::fixString(album);
                Bae::DB trackMap
                {
                    {Bae::DBCols::URL,url},
                    {Bae::DBCols::TRACK,QString::number(track)},
                    {Bae::DBCols::TITLE,title},
                    {Bae::DBCols::ARTIST,artist},
                    {Bae::DBCols::ALBUM,album},
                    {Bae::DBCols::DURATION,QString::number(duration)},
                    {Bae::DBCols::GENRE,genre},
                    {Bae::DBCols::SOURCES_URL,sourceUrl},
                    {Bae::DBCols::BABE,"0"},
                    {Bae::DBCols::RELEASE_DATE,QString::number(year)}
                };
                qDebug()<<"TRACK READY"<<url;

                while(!this->nextTrack){ }
                this->nextTrack=!this->nextTrack;
                emit trackReady(trackMap);
            }else break;
        }

        emit finished();
    }

signals:
    void trackReady(Bae::DB track);
    void finished();
    void collectionSize(int size);

private:
    QThread t;
    bool go=true;
    bool nextTrack=true;
};




class settings : public QWidget
{
    Q_OBJECT

public:

    explicit settings(QWidget *parent = 0);
    ~settings();

    const QString settingPath= Bae::SettingPath;
    const QString collectionDBPath=Bae::CollectionDBPath;
    const QString cachePath=Bae::CachePath;
    const QString youtubeCachePath=Bae::YoutubeCachePath;
    const QString extensionFetchingPath=Bae::ExtensionFetchingPath;
    bool checkCollection();
    void createCollectionDB();

    int getToolbarIconSize()  {return iconSize;}
    void setToolbarIconSize(const int &iconSize);

    void setSettings(QStringList setting);
    void readSettings();
    void removeSettings(QStringList setting);
    void refreshCollectionPaths();
    void collectionWatcher();
    void addToWatcher(QStringList paths);
    QStringList getCollectionPath() {return collectionPaths;}
    CollectionDB collection_db;
    bool youtubeTrackDone=false;

    enum iconSizes
    {
        s16,s22,s24
    };
    //enum albums { ALBUM_TITLE, ARTIST, ART};
    // enum artists { ARTIST_TITLE, ART};

private slots:

    void on_open_clicked();
    void on_toolbarIconSize_activated(const QString &arg1);
    void finishedAddingTracks();
    void on_pushButton_clicked();
    void handleDirectoryChanged(QString dir);
    void on_collectionPath_clicked(const QModelIndex &index);
    void on_remove_clicked();

    void on_debugBtn_clicked();

    void on_ytBtn_clicked();

    void on_fetchBtn_clicked();

    void on_checkBox_stateChanged(int arg1);

public slots:

    void populateDB(const QString &path);
    void fetchArt();
    void refreshWatchFiles();
    void youtubeTrackReady(const bool &state);
    void handleDirectoryChanged_cache(QString dir);
    void handleDirectoryChanged_extension();


private:
    Ui::settings *ui;
    TrackSaver trackSaver;

    const QString notifyDir= Bae::NotifyDir;
    const QString collectionDBName = "collection.db";
    const QString settingsName = "settings.conf";

    Notify nof;
    YouTube *ytFetch;

    int iconSize = 16;
    QStringList collectionPaths={};
    QLabel *artFetcherNotice;
    QMovie *movie;
    QString pathToRemove;
    // QFileSystemWatcher watcher;
    //QThread* thread;
    About *about_ui;
    QStringList files;
    QStringList dirs;
    QFileSystemWatcher *watcher;
    QFileSystemWatcher *extensionWatcher;
    QTimer *cacheTimer;

signals:

    void toolbarIconSizeChanged(int newSize);
    void collectionPathChanged(QString newPath);
    void collectionDBFinishedAdding();
    void fileChanged(QString url);
    void dirChanged(QString url,QString babe="0");
    void collectionPathRemoved(QString url);
    void refreshTables();
    void DBactionFinished();

};

#endif // SETTINGS_H
