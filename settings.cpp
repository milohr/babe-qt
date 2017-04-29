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


settings::settings(QWidget *parent) : QWidget(parent), ui(new Ui::settings) {
    ui->setupUi(this);

    qDebug() << "Getting collectionDB info from: " << collectionDBPath;
    qDebug() << "Getting settings info from: " << settingPath;
    qDebug() << "Getting artwork files from: " << cachePath;
    qDebug() << "Getting extension files files from: " << extensionFetchingPath;

    ui->ytLineEdit->setText(extensionFetchingPath);
    ui->frame_4->setEnabled(false);


    if(!BaeUtils::fileExists(notifyDir+"/Babe.notifyrc"))
    {
        qDebug()<<"The Knotify file does not exists, going to create it";
        QFile knotify(":Data/data/Babe.notifyrc");

        if(knotify.copy(notifyDir+"/Babe.notifyrc"))
        {
            qDebug()<<"the knotify file got copied";
        }

    }

    QDir collectionDBPath_dir(collectionDBPath);
    QDir settingsPath_dir(settingPath);
    QDir cachePath_dir(cachePath);
    QDir youtubeCache_dir(youtubeCachePath);

    if (!collectionDBPath_dir.exists())
        collectionDBPath_dir.mkpath(".");
    if (!settingsPath_dir.exists())
        settingsPath_dir.mkpath(".");
    if (!cachePath_dir.exists())
        cachePath_dir.mkpath(".");
    if (!youtubeCache_dir.exists())
        youtubeCache_dir.mkpath(".");
    ytFetch = new YouTube(this);
    connect(ytFetch,SIGNAL(youtubeTrackReady(bool)),this,SLOT(youtubeTrackReady(bool)));
    ytFetch->searchPendingFiles();
    extensionWatcher = new QFileSystemWatcher();
    extensionWatcher->addPath(extensionFetchingPath);
    connect(extensionWatcher, SIGNAL(directoryChanged(QString)), this,
            SLOT(handleDirectoryChanged_extension()));

    connect(this, SIGNAL(collectionPathChanged(QString)), this,
            SLOT(populateDB(QString)));
    connect(&collection_db, SIGNAL(DBactionFinished(bool)), this,
            SLOT(finishedAddingTracks(bool)));
    connect(&collection_db, SIGNAL(progress(int)), ui->progressBar,
            SLOT(setValue(int)));

    ui->remove->setEnabled(false);
    ui->progressBar->hide();
    about_ui = new About();

    movie = new QMovie(":Data/data/ajax-loader.gif");
    ui->label->setMovie(movie);
    ui->label->hide();

    watcher = new QFileSystemWatcher(this);

    connect(watcher, SIGNAL(directoryChanged(QString)), this,
            SLOT(handleDirectoryChanged(QString)));
}

settings::~settings() {
}


void settings::youtubeTrackReady(bool state)
{


    if(state)
    {
        qDebug()<<"the youtube track is ready";
        emit dirChanged(youtubeCachePath,"1");

    }
}

void settings::handleDirectoryChanged_cache(QString dir)
{
    Q_UNUSED(dir);
    qDebug()<<"the cache youtube dir has some changes but...";
    if(youtubeTrackDone)
    {
        qDebug()<<"youtubeTrackDone";
    }
}

void settings::handleDirectoryChanged_extension()
{
    QStringList urls, ids;

    QDirIterator it(extensionFetchingPath, QStringList() << "*.babe", QDir::Files);
    while (it.hasNext())
    {
        QString song = it.next();
        urls<<song;
    }

    for(auto url:urls)
    {
        QFileInfo fileInfo(QFile(url).fileName());
        QString id=fileInfo.fileName().section(".",0,-2);
        ids<<id;
    }

    if (!urls.isEmpty())
    {

        nof.notify("Song received!","wait a sec while the track ["+ids.join("\n")+"] is added to your collection :)");

        ytFetch->fetch(ids,urls);
    }
}

void settings::on_collectionPath_clicked(const QModelIndex &index) {
    ui->remove->setEnabled(true);
    pathToRemove = index.data().toString();
}

void settings::on_remove_clicked() {
    qDebug() << pathToRemove;
    if (!pathToRemove.isEmpty()) {

        collection_db.removePath(pathToRemove);
        removeSettings({"collectionPath=", pathToRemove});
        collectionPaths.removeAll(pathToRemove);

        refreshCollectionPaths();
        refreshWatchFiles();
        ui->remove->setEnabled(false);
        emit refreshTables();
    }
}

void settings::refreshCollectionPaths() {
    ui->collectionPath->clear();
    ui->collectionPath->addItems(collectionPaths);
}

void settings::refreshWatchFiles()
{
    qDebug()<<"refreshing watched files";

    dirs.clear();

    QSqlQuery query = collection_db.getQuery("SELECT * FROM tracks");

    while (query.next())
    {
        if(!query.value(CollectionDB::LOCATION).toString().contains(youtubeCachePath))
        {
            if (!dirs.contains(QFileInfo(query.value(CollectionDB::LOCATION).toString()).dir().path())&&QFileInfo(query.value(CollectionDB::LOCATION).toString()).exists())
            {

                QString dir =QFileInfo(query.value(CollectionDB::LOCATION).toString()).dir().path();

                dirs << dir;
                QDirIterator it(dir,QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

                while (it.hasNext())
                {
                    QString subDir = it.next();
                    subDir=QFileInfo(subDir).path();
                    if(QFileInfo(subDir).isDir()&&QFileInfo(subDir).exists())
                    {
                        if(!dirs.contains(subDir))
                            dirs <<subDir;
                    }
                }

            }
        }

    }
    watcher->removePaths(watcher->directories());

    addToWatcher(dirs);
}

CollectionDB &settings::getCollectionDB() { return collection_db; }

void settings::on_toolbarIconSize_activated(const QString &arg1) {
    iconSize = arg1.toInt();
    setSettings({"toolbarIconSize=", arg1});

    emit toolbarIconSizeChanged(iconSize);
}

void settings::on_open_clicked() {
    QString url = QFileDialog::getExistingDirectory(this,"Select folder...", QDir().homePath()+"/Music/");

    if (!collectionPaths.contains(url) && !url.isEmpty()) {
        ui->collectionPath->addItem(url);
        collectionPaths << url;
        qDebug() << "Collection dir added: " << url;
        setSettings({"collectionPath=", url});
        emit collectionPathChanged(url);
    }
}

void settings::setSettings(QStringList setting) {
    std::string strNew;
    strNew = setting.at(0).toStdString() + setting.at(1).toStdString();
    bool replace = false;

    std::ifstream settings(settingPath.toStdString() +
                           settingsName.toStdString());
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

        for (auto ln : newline) {
            write << ln.toStdString() << std::endl;
        }

    } else {
        std::ofstream write(settingPath.toStdString() + settingsName.toStdString(),
                            std::ios::app);
        write << strNew << std::endl;
    }
}

void settings::removeSettings(QStringList setting) {
    std::string strNew;
    strNew = "";
    bool replace = false;
    std::ifstream settings(settingPath.toStdString() +
                           settingsName.toStdString());
    QStringList newline;
    std::string line;
    while (std::getline(settings, line)) {
        if (QString::fromStdString(line).contains(setting.at(0)) &&
                QString::fromStdString(line).contains(setting.at(1))) {

            replace = true;
            newline << QString::fromStdString(strNew);

        } else {
            newline << QString::fromStdString(line);
        }
    }

    if (replace) {
        std::ofstream write(settingPath.toStdString() + settingsName.toStdString());

        for (auto ln : newline) {
            write << ln.toStdString() << std::endl;
        }

    } else {
        std::ofstream write(settingPath.toStdString() + settingsName.toStdString(),
                            std::ios::app);
        write << strNew << std::endl;
    }
}

void settings::addToWatcher(QStringList paths)
{
    qDebug()<<"duplicated paths in watcher removd: "<<paths.removeDuplicates();

    if(!paths.isEmpty()) watcher->addPaths(paths);
}

void settings::collectionWatcher()
{
    QSqlQuery query = collection_db.getQuery("SELECT * FROM tracks");
    while (query.next())
    {
        QString location = query.value(CollectionDB::LOCATION).toString();
        if(!location.contains(youtubeCachePath)) //exclude the youtube cache folder
        {
            if (!dirs.contains(QFileInfo(location).dir().path()) && BaeUtils::fileExists(location)) //check if parent dir isn't already in list and it exists
            {
                QString dir =QFileInfo(location).dir().path();
                dirs << dir;

                QDirIterator it(dir, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories); // get all the subdirectories to watch
                while (it.hasNext())
                {
                    QString subDir = it.next();
                    subDir = QFileInfo(subDir).path();

                    if(QFileInfo(subDir).isDir())
                        if(!dirs.contains(subDir))
                            dirs <<subDir;

                }

            }
        }

    }

    addToWatcher(dirs);
}


void settings::handleFileChanged(QString file) {
    qDebug() << "this file changed: " << file;
    refreshWatchFiles();
    emit fileChanged(file);
}

void settings::handleDirectoryChanged(QString dir) {
    qDebug() << "this directory changed: " << dir;

    emit dirChanged(dir);
}

void settings::readSettings() {
    std::ifstream settings(settingPath.toStdString() +
                           settingsName.toStdString());
    std::string line;
    while (std::getline(settings, line)) {
        auto get_setting = QString::fromStdString(line);

        if (get_setting.contains("collectionPath=")) {
            collectionPaths << get_setting.replace("collectionPath=", "");
            qDebug() << "Setting the cPath: "
                     << get_setting.replace("collectionPath=", "");
            ui->collectionPath->addItem(get_setting.replace("collectionPath=", ""));

        }

        if (get_setting.contains("toolbarIconSize=")) {
            iconSize = get_setting.replace("toolbarIconSize=", "").toInt();
            qDebug() << "Setting the tSize: " << iconSize;

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

bool settings::checkCollection() {


    if (BaeUtils::fileExists(collectionDBPath + collectionDBName))
    {
        qDebug() << "The CollectionDB does exists.";

        collection_db.openCollection(collectionDBPath + collectionDBName);
        collectionWatcher();

        return true;

    } else { return false; }

}


void settings::createCollectionDB()
{
    qDebug() << "The CollectionDB doesn't exists. Going to create the database "
                "and tables";

    collection_db.openCollection(collectionDBPath + collectionDBName);
    collection_db.prepareCollectionDB();

}

void settings::populateDB(QString path) {

    qDebug() << "Function Name: " << Q_FUNC_INFO
             << "new path for database action: " << path;

    QStringList urlCollection;
    // QDir dir = new QDir(url);

    if (QFileInfo(path).isDir())

    {
        QDirIterator it(path, formats, QDir::Files, QDirIterator::Subdirectories);

        while (it.hasNext()) {
            urlCollection << it.next();
        }
    } else if (QFileInfo(path).isFile()) {
        urlCollection << path;
        qDebug() << path;
    }

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(urlCollection.size());
    ui->progressBar->show();
    collection_db.addTrack(urlCollection);
}

void settings::finishedAddingTracks(bool state) {
    if (state) {


        ui->progressBar->hide();
        ui->progressBar->setValue(0);

        nof.notify("Songs added to collection","finished writting new songs to the collection :)");

        qDebug() << "good to hear it finished yay! now going to fetch artwork";

        collectionWatcher();
        emit refreshTables();
        fetchArt();

    }else
    {
        emit collectionDBFinishedAdding(true);

    }
}

void settings::fetchArt() {


    nof.notify("Fetching art","this might take some time depending on your collection size and internet connection speed...");


    ui->label->show();
    movie->start();
    QSqlQuery query_Covers =
            collection_db.getQuery("SELECT * FROM albums WHERE art = ''");
    QSqlQuery query_Heads =
            collection_db.getQuery("SELECT * FROM artists WHERE art = ''");

    while (query_Covers.next())
    {

        QString artist = query_Covers.value(1).toString();
        QString album = query_Covers.value(0).toString();
        QString title;
        QSqlQuery query_Title =
                collection_db.getQuery("SELECT * FROM tracks WHERE artist = \""+artist+"\" AND album = \""+album+"\"");
        if(query_Title.next()) title=query_Title.value(CollectionDB::TITLE).toString();


        ArtWork coverArt;
        connect(&coverArt, &ArtWork::coverReady, &coverArt,
                &ArtWork::saveArt);
        connect(&coverArt, &ArtWork::artSaved, &collection_db,
                &CollectionDB::insertCoverArt);

        qDebug() << artist << album;
        coverArt.setDataCover(artist, album,title, cachePath);


    }

    while (query_Heads.next())
    {

        ArtWork artistHead;

        connect(&artistHead, &ArtWork::headReady, &artistHead,
                &ArtWork::saveArt);
        connect(&artistHead, &ArtWork::artSaved, &collection_db,
                &CollectionDB::insertHeadArt);

        QString artist = query_Heads.value(0).toString();

        artistHead.setDataHead(artist, cachePath);
    }

    nof.notify("Finished fetching art","the artwork for your collection is now ready :)");
    movie->stop();
    ui->label->hide();


    emit collectionDBFinishedAdding(true);
}

void settings::on_pushButton_clicked() {
    about_ui->show();
}

void settings::on_debugBtn_clicked()
{
    qDebug()<<"Current dirs being watched:";
    for(auto dir: watcher->directories()) qDebug()<<dir;
}

void settings::on_ytBtn_clicked()
{

}

void settings::on_fetchBtn_clicked()
{
    if(!ui->fetch->text().isEmpty())
    {
        ytFetch->fetch({ui->fetch->text()});
        ui->fetch->clear();
    }
}

void settings::on_checkBox_stateChanged(int arg1)
{
    if(arg1==0)
    {
        ui->frame_4->setEnabled(false);
    }else
    {
        ui->frame_4->setEnabled(true);
    }
}
