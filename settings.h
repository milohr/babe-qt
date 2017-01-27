#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include<QString>
#include<QStringList>
#include "collectionDB.h"
#include<QDebug>
#include <QThread>
#include "playlist.h"
#include <about.h>
#include <QDir>
#include <QFileSystemWatcher>

namespace Ui {
class settings;
}




class settings : public QWidget
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = 0);
    ~settings();
    bool checkCollection();
    CollectionDB &getCollectionDB();
    int getToolbarIconSize()  {return iconSize;}
    QStringList getCollectionPath() {return collectionPaths;}
    void setSettings(QStringList setting);
    void readSettings();
    void collectionWatcher();
    void addToWatcher(QStringList paths);
    enum iconSizes
    {
        s16,s22,s24
    };
private slots:

    void on_open_clicked();
    void on_toolbarIconSize_activated(const QString &arg1);
    void finishedAddingTracks(bool state);
    void on_pushButton_clicked();
    void handleFileChanged(QString file);
    void handleDirectoryChanged(QString dir);

    void on_collectionPath_clicked(const QModelIndex &index);

    void on_remove_clicked();

public slots:

void populateDB(QString path);

private:
    Ui::settings *ui;
    const QString settingPath= QDir().homePath()+"/.config/babe/";
    const QString collectionDBPath=QDir().homePath()+"/.local/share/babe/";
    const QString collectionDBName = "collection.db";
    const QString settingsName = "settings.conf";
    int iconSize = 16;
    QStringList collectionPaths={};
    CollectionDB collection_db;
    QString pathToRemove;
   // QFileSystemWatcher watcher;

 QThread* thread;
 About *about_ui;
signals:
    void toolbarIconSizeChanged(int newSize);
    void collectionPathChanged(QString newPath);
    void collectionDBFinishedAdding(bool state);
    void fileChanged(QString url);
    void dirChanged(QString url);
    void collectionPathRemoved(QString url);
    void refreshTables();



};





#endif // SETTINGS_H
