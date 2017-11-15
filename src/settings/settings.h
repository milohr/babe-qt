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

#include <fstream>
#include <iostream>

#include "../utils/bae.h"
#include "../utils/brain.h"

#include "../kde/notify.h"

#include "../services/web/youtube.h"
#include "../dialogs/about.h"
#include "../db/collectionDB.h"
#include "../pulpo/pulpo.h"

#include "fileloader.h"

namespace Ui {class settings;}

class settings : public QWidget
{
    Q_OBJECT

public:

    explicit settings(QWidget *parent = nullptr);
    ~settings();
    bool checkCollection();
    void createCollectionDB();

    Deamon::Brain brainDeamon;

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
    void on_pushButton_clicked();
    void handleDirectoryChanged(const QString &dir);
    void on_collectionPath_clicked(const QModelIndex &index);
    void on_remove_clicked();

    void on_debugBtn_clicked();
    void on_checkBox_stateChanged(int arg1);

public slots:

    void populateDB(const QString &path);
    void fetchArt();
    void handleDirectoryChanged_extension();


private:
    Ui::settings *ui;
    FileLoader fileSaver;

    bool busy =false;

    const QString notifyDir= Bae::NotifyDir;
    const QString collectionDBName = "collection.db";
    const QString settingsName = "settings.conf";

    Notify nof;
    YouTube *ytFetch;

    int iconSize = 16;
    QStringList collectionPaths={};
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
    void refreshTables(const Bae::TABLE &reset);
    void finishedTracksInsertion();
    void getArtwork();
    void albumArtReady(const Bae::TABLE &type);

};

#endif // SETTINGS_H
