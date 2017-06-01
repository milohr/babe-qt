/*
   Babe - tiny music player
   Copyright (C) 2017  Camilo Higuita
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "settings.h"
#include "ui_settings.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QMovie>
#include <QString>
#include <QStringList>
#include <QTimer>

settings::settings(QWidget *parent) : QWidget(parent)
  ,ui(new Ui::settings)
{
    ui->setupUi(this);

    ui->ytLineEdit->setText(extensionFetchingPath);
    ui->frame_4->setEnabled(false);

    qDebug() << "notifyDir is: " << notifyDir;

    if (!BaeUtils::fileExists(notifyDir + "/babe.notifyrc")) {
        qDebug() << "The Knotify file does not exists, going to create it";
        QFile knotify(":Data/data/babe.notifyrc");
        if (knotify.copy(notifyDir + "/babe.notifyrc"))
            qDebug() << "the knotify file got copied";
    }

    QDir cachePath_dir(cachePath);
    QDir settingsPath_dir(settingPath);
    QDir youtubeCache_dir(youtubeCachePath);

    if (!settingsPath_dir.exists())
        settingsPath_dir.mkpath(".");
    if (!cachePath_dir.exists())
        cachePath_dir.mkpath(".");
    if (!youtubeCache_dir.exists())
        youtubeCache_dir.mkpath(".");

    ytFetch = new YouTube(this);
    connect(ytFetch, SIGNAL(youtubeTrackReady(bool)), this, SLOT(youtubeTrackReady(bool)));
    ytFetch->searchPendingFiles();
    extensionWatcher = new QFileSystemWatcher();
    extensionWatcher->addPath(extensionFetchingPath);

    connect(extensionWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleDirectoryChanged_extension()));
    connect(this, SIGNAL(collectionPathChanged(QString)), this, SLOT(populateDB(QString)));
    connect(&m_collectionDB, SIGNAL(dbActionFinished(bool)), this, SLOT(finishedAddingTracks(bool)));
    connect(&m_collectionDB, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));

    ui->remove->setEnabled(false);
    ui->progressBar->hide();
    about_ui = new About();

    movie = new QMovie(":Data/data/ajax-loader.gif");
    ui->label->setMovie(movie);
    ui->label->hide();

    watcher = new QFileSystemWatcher(this);
    connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleDirectoryChanged(QString)));
}

settings::~settings()
{
}

void settings::youtubeTrackReady(bool state)
{
    if (state) {
        qDebug() << "the youtube track is ready";
        emit dirChanged(youtubeCachePath, 1);
    }
}

void settings::handleDirectoryChanged_cache(const QString &dir)
{
    Q_UNUSED(dir);
    qDebug() << "the cache youtube dir has some changes but...";
    if (youtubeTrackDone)
        qDebug() << "youtubeTrackDone";
}

void settings::handleDirectoryChanged_extension()
{
    QStringList urls, ids;
    QDirIterator it(extensionFetchingPath, QStringList() << " *.babe", QDir::Files);
    while (it.hasNext())
        urls << it.next();

    for (auto url : urls) {
        QFileInfo fileInfo(QFile(url).fileName());
        QString id = fileInfo.fileName().section(".", 0, -2);
        ids << id;
    }

    if (!urls.isEmpty()) {
        nof.notify("Song recived!", "Wait while the track [" + ids.join("\n") + "] is added to your collection :)");
        ytFetch->fetch(ids, urls);
    }
}

void settings::on_collectionPath_clicked(const QModelIndex &index)
{
    ui->remove->setEnabled(true);
    pathToRemove = index.data().toString();
}

void settings::on_remove_clicked()
{
    qDebug() << pathToRemove;
    if (!pathToRemove.isEmpty()) {
        m_collectionDB.removeTrack(pathToRemove);
        removeSettings({"collectionPath=", pathToRemove});
        collectionPaths.removeAll(pathToRemove);
        refreshCollectionPaths();
        refreshWatchFiles();
        ui->remove->setEnabled(false);
        emit refreshTables();
    }
}

void settings::refreshCollectionPaths()
{
    ui->collectionPath->clear();
    ui->collectionPath->addItems(collectionPaths);
}

void settings::refreshWatchFiles()
{
    qDebug() << "refreshing watched files";
    dirs.clear();
    QList<QMap<int, QString>> savedTracks = m_collectionDB.getTrackData(QVariantMap(), -1, 0);
    int sizeSavedTracks = savedTracks.size();
    for (int i = 0; i < sizeSavedTracks; i++) {
        QMap<int, QString> track = savedTracks.at(i);
        if (!track.value(CollectionDB::LOCATION).contains(youtubeCachePath)) {
            if (!dirs.contains(QFileInfo(track.value(CollectionDB::LOCATION)).dir().path()) && QFileInfo(track.value(CollectionDB::LOCATION)).exists()) {
                QString dir = QFileInfo(track.value(CollectionDB::LOCATION)).dir().path();
                dirs << dir;
                QDirIterator it(dir, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
                QString subDir;
                while (it.hasNext()) {
                    subDir = QFileInfo(it.next()).path();
                    if (QFileInfo(subDir).isDir() && QFileInfo(subDir).exists()) {
                        if (!dirs.contains(subDir))
                            dirs << subDir;
                    }
                }
            }
        }
    }
    watcher->removePaths(watcher->directories());
    addToWatcher(dirs);
}

int settings::getToolbarIconSize()
{
    return iconSize;
}

void settings::on_toolbarIconSize_activated(const QString &arg1)
{
    iconSize = arg1.toInt();
    setSettings({"toolbarIconSize=", arg1});
    emit toolbarIconSizeChanged(iconSize);
}

void settings::on_open_clicked()
{
    QString url = QFileDialog::getExistingDirectory(this, "Select folder...", QDir().homePath() + "/Music/");
    if (!collectionPaths.contains(url) && !url.isEmpty()) {
        ui->collectionPath->addItem(url);
        collectionPaths << url;
        qDebug() << "Collection dir added: " << url;
        setSettings({"collectionPath=", url});
        emit collectionPathChanged(url);
    }
}

void settings::setSettings(const QStringList &setting)
{
    std::string strNew;
    strNew = setting.at(0).toStdString() + setting.at(1).toStdString();
    bool replace = false;
    std::ifstream settings(settingPath.toStdString() + settingsName.toStdString());
    QStringList newline;
    std::string line;
    while (std::getline(settings, line)) {
        if (QString::fromStdString(line).contains(setting.at(0))) {
            if (!QString::fromStdString(line).contains("collectionPath=")) {
                replace = true;
                newline << QString::fromStdString(strNew);
            }
        } else {
            newline << QString::fromStdString(line);
        }
    }
    if (replace) {
        std::ofstream write(settingPath.toStdString() + settingsName.toStdString());
        for (auto ln : newline)
            write << ln.toStdString() << std::endl;
    } else {
        std::ofstream write(settingPath.toStdString() + settingsName.toStdString(), std::ios::app);
        write << strNew << std::endl;
    }
}

void settings::removeSettings(const QStringList &setting)
{
    bool replace = false;
    std::string line = "";
    std::string strNew = "";
    QStringList newline;
    std::ifstream settings(settingPath.toStdString() + settingsName.toStdString());
    while (std::getline(settings, line)) {
        if (QString::fromStdString(line).contains(setting.at(0)) && QString::fromStdString(line).contains(setting.at(1))) {
            replace = true;
            newline << QString::fromStdString(strNew);
        } else {
            newline << QString::fromStdString(line);
        }
    }
    if (replace) {
        std::ofstream write(settingPath.toStdString() + settingsName.toStdString());
        for (auto ln : newline)
            write << ln.toStdString() << std::endl;
    } else {
        std::ofstream write(settingPath.toStdString() + settingsName.toStdString(), std::ios::app);
        write << strNew << std::endl;
    }
}

void settings::addToWatcher(QStringList paths)
{
    qDebug() << "duplicated paths in watcher removed: " << paths.removeDuplicates();
    if (!paths.isEmpty())
        watcher->addPaths(paths);
}

QStringList settings::getCollectionPath()
{
    return collectionPaths;
}

void settings::collectionWatcher()
{
    QList<QMap<int, QString>> savedTracks = m_collectionDB.getTrackData(QVariantMap(), -1, 0);
    int sizeSavedTracks = savedTracks.size();
    for (int i = 0; i < sizeSavedTracks; i++) {
        QMap<int, QString> track = savedTracks.at(i);
        QString location(track.value(CollectionDB::LOCATION));
        // exclude the youtube cache folder
        if (!location.contains(youtubeCachePath)) {
            // check if parent dir isn't already in list and it exists
            if (!dirs.contains(QFileInfo(location).dir().path()) && BaeUtils::fileExists(location)) {
                QString dir(QFileInfo(location).dir().path());
                dirs << dir;
                // get all the subdirectories to watch
                QDirIterator it(dir, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
                QString subDir;
                while (it.hasNext()) {
                    subDir = QFileInfo(it.next()).path();
                    if (QFileInfo(subDir).isDir())
                        if (!dirs.contains(subDir))
                            dirs << subDir;
                }
            }
        }
    }
    addToWatcher(dirs);
}

void settings::handleFileChanged(const QString &file)
{
    qDebug() << "this file changed: " << file;
    refreshWatchFiles();
    emit fileChanged(file);
}

void settings::handleDirectoryChanged(const QString &dir)
{
    qDebug() << "this directory changed: " << dir;
    emit dirChanged(dir);
}

void settings::readSettings()
{
    std::ifstream settings(settingPath.toStdString() + settingsName.toStdString());
    std::string line;
    while (std::getline(settings, line)) {
        auto get_setting = QString::fromStdString(line);
        if (get_setting.contains("collectionPath=")) {
            collectionPaths << get_setting.replace("collectionPath=", "");
            ui->collectionPath->addItem(get_setting.replace("collectionPath=", ""));
        }
        if (get_setting.contains("toolbarIconSize=")) {
            iconSize = get_setting.replace("toolbarIconSize=", "").toInt();
            switch (iconSize) {
            case 16:
                ui->toolbarIconSize->setCurrentIndex(0);
                break;
            case 22:
                ui->toolbarIconSize->setCurrentIndex(1);
                break;
            case 24:
                ui->toolbarIconSize->setCurrentIndex(2);
                break;
            default:
                qDebug() << "error setting icon size";
            }
        }
    }
}

void settings::populateDB(const QString &path)
{
    qDebug() << "Function Name: " << Q_FUNC_INFO << " new path for database action: " << path;
    QStringList urlCollection;
    if (QFileInfo(path).isDir()) {
        QDirIterator it(path, formats, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
            urlCollection << it.next();
    } else if (QFileInfo(path).isFile()) {
        urlCollection << path;
        qDebug() << path;
    }
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(urlCollection.size());
    ui->progressBar->show();
    m_collectionDB.addTrack(urlCollection);
}

void settings::finishedAddingTracks(bool state)
{
    if (state) {
        ui->progressBar->hide();
        ui->progressBar->setValue(0);
        nof.notify("Songs added to collection", "finished writting new songs to the collection :)");
        qDebug() << "good to hear it finished yay! now going to fetch artwork";
        collectionWatcher();
        emit refreshTables();
        fetchArt();
    } else {
        emit collectionDBFinishedAdding(true);
    }
}

void settings::fetchArt()
{
    nof.notify("Fetching art", "This might take some time depending on your collection size and internet connection speed...");
    ui->label->show();
    movie->start();

    QVariantMap filterByEmptyArt({{"art", ""}});
    QVariantList albumsData = m_collectionDB.getAlbumsData(filterByEmptyArt);
    QVariantList artistData = m_collectionDB.getArtistData(filterByEmptyArt);
    QVariantMap map;

    foreach (const QVariant &item, albumsData) {
        map = item.toMap();
        QString title;
        QString album(map.value("title").toString());
        QString artist(map.value("artist").toString());

        QList<QMap<int, QString>> tracks = m_collectionDB.getTrackData(QVariantMap({{"artist", artist}, {"album", album}}));
        if (tracks.size())
            title = tracks.at(0).value(CollectionDB::TITLE);

        m_collectionDB.insertCoverArt("", {album, artist});
        Pulpo art(title, artist, album);
        connect(&art, &Pulpo::albumArtReady, [this, &art](QByteArray array){ art.saveArt(array, this->cachePath); });
        connect(&art, &Pulpo::artSaved, &m_collectionDB, &CollectionDB::insertCoverArt);
        art.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::LastFm,true);
    }
    map.clear();
    foreach (const QVariant &item, artistData) {
        map = item.toMap();
        QString artist(map.value("title").toString());
        m_collectionDB.insertHeadArt("", {artist});
        Pulpo art("", artist, "");
        connect(&art, &Pulpo::artistArtReady, [this, &art](QByteArray array){ art.saveArt(array, this->cachePath); });
        connect(&art, &Pulpo::artSaved, &m_collectionDB, &CollectionDB::insertHeadArt);
        art.fetchArtistInfo(Pulpo::ArtistArt, Pulpo::LastFm);
    }

    nof.notify("Finished fetching art", "the artwork for your collection is now ready :)");
    movie->stop();
    ui->label->hide();
    emit collectionDBFinishedAdding(true);
}

void settings::on_pushButton_clicked()
{
    about_ui->show();
}

void settings::on_debugBtn_clicked()
{
    qDebug() << "Current dirs being watched: ";
    for (auto dir : watcher->directories())
        qDebug() << dir;
}

void settings::on_ytBtn_clicked()
{
}

void settings::on_fetchBtn_clicked()
{
    if (!ui->fetch->text().isEmpty()) {
        ytFetch->fetch({ui->fetch->text()});
        ui->fetch->clear();
    }
}

void settings::on_checkBox_stateChanged(int arg1)
{
    ui->frame_4->setEnabled(arg1 == 0 ? false : true);
}
