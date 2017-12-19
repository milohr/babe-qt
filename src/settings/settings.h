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

#include "../utils/bae.h"
class FileLoader;
class Pulpo;
class YouTube;
class Socket;
class About;
class CollectionDB;
class Brain;

using namespace BAE;

namespace Ui { class settings; }

class settings : public QWidget
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();

    About *about_ui;
    Brain *brainDeamon;

    void checkCollection();

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
    FileLoader *fileLoader;
    CollectionDB *connection;
    const QString notifyDir = BAE::NotifyDir;

    YouTube *ytFetch;

    QMovie *movie;
    QString pathToRemove;

    QStringList dirs;
    QFileSystemWatcher *watcher;
    Socket *babeSocket;

    void refreshCollectionPaths();
    void collectionWatcher();
    void addToWatcher(QStringList paths);

signals:
    void collectionPathChanged(QString newPath);
    void refreshTables(const QMap<BAE::TABLE,bool> &reset);
    void albumArtReady(const DB &album);

};

#endif // SETTINGS_H
