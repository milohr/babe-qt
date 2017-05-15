#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <iostream>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QGridLayout>
#include <QLabel>
#include <QMovie>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QWidget>

#include "about.h"
#include "baeUtils.h"
#include "collectionDB.h"
#include "notify.h"
#include "playlist.h"
#include "pulpo/pulpo.h"
#include "youtube.h"

namespace Ui {
class settings;
}

class settings : public QWidget
{
    Q_OBJECT
public:
    explicit settings(QWidget *parent = 0);
    ~settings();

    const QString settingPath = BaeUtils::getSettingPath();
    const QString collectionDBPath = BaeUtils::getCollectionDBPath();
    const QString cachePath = BaeUtils::getCachePath();
    const QString youtubeCachePath = BaeUtils::getYoutubeCachePath();
    const QString extensionFetchingPath = BaeUtils::getExtensionFetchingPath();
    const QStringList formats {"*.mp4", "*.mp3", "*.wav", "*.flac", "*.ogg", "*.m4a"};

    bool checkCollection();
    void createCollectionDB();
    CollectionDB &getCollectionDB();

    int getToolbarIconSize();

    void setSettings(const QStringList &setting);
    void readSettings();
    void removeSettings(const QStringList &setting);
    void refreshCollectionPaths();
    void collectionWatcher();
    void addToWatcher(QStringList paths);
    QStringList getCollectionPath();
    CollectionDB collection_db;
    bool youtubeTrackDone = false;

    enum iconSizes
    {
        s16, s22, s24
    };

private slots:
    void on_open_clicked();
    void on_toolbarIconSize_activated(const QString &arg1);
    void finishedAddingTracks(bool state);
    void on_pushButton_clicked();
    void handleFileChanged(const QString &file);
    void handleDirectoryChanged(const QString &dir);
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
    void youtubeTrackReady(bool state);
    void handleDirectoryChanged_cache(const QString &dir);
    void handleDirectoryChanged_extension();

private:
    const QString notifyDir = BaeUtils::getNotifyDir();
    const QString collectionDBName = "Babe.db";
    const QString settingsName = "settings.conf";
    int iconSize = 16;
    Notify nof;
    QStringList dirs;
    QStringList files;
    QString pathToRemove;
    QStringList collectionPaths = {};

    QMovie *movie;
    About *about_ui;
    Ui::settings *ui;
    YouTube *ytFetch;
    QTimer *cacheTimer;
    QLabel *artFetcherNotice;
    QFileSystemWatcher *watcher;
    QFileSystemWatcher *extensionWatcher;

signals:
    void toolbarIconSizeChanged(int newSize);
    void collectionPathChanged(const QString &newPath);
    void collectionDBFinishedAdding(bool state);
    void fileChanged(const QString &url);
    void dirChanged(const QString &url, const QString &babe = "0");
    void collectionPathRemoved(const QString &url);
    void refreshTables();
};

#endif // SETTINGS_H
