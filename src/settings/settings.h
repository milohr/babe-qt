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
#include "fileloader.h"

class Pulpo;
class YouTube;
class Socket;
class About;

namespace Ui { class settings; }

class settings : public QWidget
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();

    About *about_ui;
    void checkCollection();

    Brain brainDeamon;

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

    const QString notifyDir = BAE::NotifyDir;
    const QString settingsName = "settings.conf";

    YouTube *ytFetch;

    QStringList collectionPaths = {};
    QMovie *movie;
    QString pathToRemove;

    QStringList dirs;
    QFileSystemWatcher *watcher;
    Socket *babeSocket;

signals:
    void collectionPathChanged(QString newPath);
    void refreshTables(const QMap<BAE::TABLE,bool> &reset);
    void albumArtReady(const DB &album);

};

#endif // SETTINGS_H
