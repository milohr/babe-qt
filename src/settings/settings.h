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

class Socket;

class settings : public QWidget
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();

    const About *about_ui;

    bool checkCollection();
    void createCollectionDB();

    Deamon::Brain brainDeamon;

    void setSettings(QStringList setting);
    void readSettings();
    void removeSettings(QStringList setting);
    void refreshCollectionPaths();
    void collectionWatcher();
    void addToWatcher(QStringList paths);

    QStringList getCollectionPath() {return collectionPaths;}

private slots:
    void on_open_clicked();
    void handleDirectoryChanged(const QString &dir);
    void on_collectionPath_clicked(const QModelIndex &index);
    void on_remove_clicked();

public slots:
    void populateDB(const QString &path);
    void fetchArt();

private:
    Ui::settings *ui;
    FileLoader fileSaver;
    CollectionDB collection_db;

    const QString notifyDir = Bae::NotifyDir;
    const QString collectionDBName = "collection.db";
    const QString settingsName = "settings.conf";

    Notify nof;
    YouTube *ytFetch;

    QStringList collectionPaths={};
    QMovie *movie;
    QString pathToRemove;

    QStringList dirs;
    QFileSystemWatcher *watcher;
    Socket *babeSocket;

signals:
    void collectionPathChanged(QString newPath);
    void refreshTables(const QMap<Bae::TABLE,bool> &reset);
    void albumArtReady(const DB &album);

};

#endif // SETTINGS_H
