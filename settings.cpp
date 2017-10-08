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
    // QFrame frame = new QFrame();

    // collectionDBPath=QDir().absolutePath()+collectionDBPath;
    qDebug() << "Getting collectionDB info from: " << Bae::CollectionDBPath;
    qDebug() << "Getting settings info from: " << Bae::SettingPath;
    qDebug() << "Getting artwork files from: " << Bae::CachePath;
    qDebug() << "Getting extension files files from: " <<Bae::ExtensionFetchingPath;

    ui->ytLineEdit->setText(Bae::ExtensionFetchingPath);
    ui->frame_4->setEnabled(false);


    if(!Bae::fileExists(notifyDir+"/Babe.notifyrc"))
    {
        qDebug()<<"The Knotify file does not exists, going to create it";
        QFile knotify(":Data/data/Babe.notifyrc");

        if(knotify.copy(notifyDir+"/Babe.notifyrc"))
        {
            qDebug()<<"the knotify file got copied";
        }

    }

    QDir collectionDBPath_dir(Bae::CollectionDBPath);
    QDir settingsPath_dir(Bae::SettingPath);
    QDir cachePath_dir(Bae::CachePath);
    QDir youtubeCache_dir(Bae::YoutubeCachePath);

    if (!collectionDBPath_dir.exists())
        collectionDBPath_dir.mkpath(".");
    if (!settingsPath_dir.exists())
        settingsPath_dir.mkpath(".");
    if (!cachePath_dir.exists())
        cachePath_dir.mkpath(".");
    if (!youtubeCache_dir.exists())
        youtubeCache_dir.mkpath(".");

    connect(&trackSaver, &TrackSaver::artworkReady,[this](Bae::DB albumMap)
    {
        emit albumArtReady(albumMap);
    });

    connect(&trackSaver, &TrackSaver::finishedFetchingArtwork,[this]()
    {
        movie->stop();
        ui->label->hide();
        this->ui->sourcesFrame->setEnabled(true);

    });

    connect(this, &settings::getArtwork, this, &settings::fetchArt);

    connect(&collection_db,&CollectionDB::trackInserted,[this]()
    {
        trackSaver.nextTrack();
        this->ui->progressBar->setValue(this->ui->progressBar->value()+1);
    });
    connect(&trackSaver,&TrackSaver::collectionSize,[this](int size)
    {
        qDebug()<<"COLLECTION SIZE"<<size;
        if(size>0)
        {
            ui->progressBar->setValue(1);
            ui->progressBar->setMaximum(size);
            ui->progressBar->show();
        }else
        {
            this->refreshWatchFiles();
        }
    });

    connect(&trackSaver,&TrackSaver::finished,[this]()
    {
        qDebug()<<"FINISHED TRACKSAVER";
        //        nof.notify("Songs added to collection","finished writting new songs to the collection :)");
        ui->progressBar->hide();
        ui->progressBar->setValue(0);

        collectionWatcher();
        emit refreshTables(Bae::DBTables::TRACKS);
        emit getArtwork();
    });

    connect(&trackSaver,&TrackSaver::trackReady,&collection_db, &CollectionDB::addTrack);

    connect(this, &settings::collectionPathChanged, this, &settings::populateDB);


    ui->ytLineEdit->setText(Bae::ExtensionFetchingPath);
    ytFetch = new YouTube(this);
    connect(ytFetch,&YouTube::youtubeTrackReady, this, &settings::populateDB);

    //    connect(ytFetch,&YouTube::youtubeTrackReady,[this](){ emit collectionPathChanged(youtubeCachePath);});
    ytFetch->searchPendingFiles();
    extensionWatcher = new QFileSystemWatcher();
    extensionWatcher->addPath(Bae::ExtensionFetchingPath);
    connect(extensionWatcher, SIGNAL(directoryChanged(QString)), this,
            SLOT(handleDirectoryChanged_extension()));


    ui->remove->setEnabled(false);
    ui->progressBar->hide();
    about_ui = new About();

    movie = new QMovie(":Data/data/ajax-loader.gif");
    ui->label->setMovie(movie);
    ui->label->hide();

    watcher = new QFileSystemWatcher(this);
    // watcher->addPath(youtubeCachePath);

    connect(watcher, SIGNAL(directoryChanged(QString)), this,
            SLOT(handleDirectoryChanged(QString)));
}

settings::~settings() {
    //collection_db.closeConnection();

}


void settings::handleDirectoryChanged_extension()
{
    QStringList urls;

    QDirIterator it(Bae::ExtensionFetchingPath, QStringList() << "*.babe", QDir::Files);

    while (it.hasNext()) urls<< it.next();

    if (!urls.isEmpty()) ytFetch->fetch(urls);

}



void settings::on_collectionPath_clicked(const QModelIndex &index) {
    ui->remove->setEnabled(true);
    pathToRemove = index.data().toString();
}

void settings::on_remove_clicked()
{
    qDebug() << pathToRemove;
    if (!pathToRemove.isEmpty())
    {
        if(collection_db.removeSource(pathToRemove))
        {
            removeSettings({"collectionPath=", pathToRemove});
            collectionPaths.removeAll(pathToRemove);

            refreshCollectionPaths();
            this->refreshWatchFiles();
            ui->remove->setEnabled(false);
            emit refreshTables(Bae::DBTables::ALL);
        }
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
    auto queryTxt = QString("SELECT %1 FROM %2").arg(Bae::DBColsMap[Bae::DBCols::URL],Bae::DBTablesMap[Bae::DBTables::TRACKS]);
    QSqlQuery query = collection_db.getQuery(queryTxt);

    while (query.next())
    {
        if(!query.value(Bae::DBColsMap[Bae::DBCols::URL]).toString().contains(Bae::YoutubeCachePath))
        {
            if (!dirs.contains(QFileInfo(query.value(Bae::DBColsMap[Bae::DBCols::URL]).toString()).dir().path())&&QFileInfo(query.value(Bae::DBColsMap[Bae::DBCols::URL]).toString()).exists())
            {
                QString dir =QFileInfo(query.value(Bae::DBColsMap[Bae::DBCols::URL]).toString()).dir().path();

                dirs << dir;
                QDirIterator it(dir,QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

                while (it.hasNext())
                {
                    QString subDir = it.next();
                    subDir=QFileInfo(subDir).path();
                    if(QFileInfo(subDir).isDir()&&QFileInfo(subDir).exists())
                    {
                        //QDir dir = new QDir(url.path());
                        if(!dirs.contains(subDir))
                            dirs <<subDir;
                    }
                }
            }
        }
    }

    /*for(auto path:files) qDebug() << "refreshed watcher -file:"<< path;
    for(auto path:dirs) qDebug() << "refreshed watcher -dir:"<< path;*/
    watcher->removePaths(watcher->directories());
    // watcher->removePaths(watcher->files());

    addToWatcher(dirs);

}


void settings::on_toolbarIconSize_activated(const QString &arg1) {
    // qDebug () <<arg1;
    iconSize = arg1.toInt();
    emit toolbarIconSizeChanged(iconSize);
}

void settings::on_open_clicked()
{
    QString url = QFileDialog::getExistingDirectory(this,"Select folder...", QDir().homePath()+"/Music/");

    if (!collectionPaths.contains(url) && !url.isEmpty())
    {
        ui->collectionPath->addItem(QDir(url).absolutePath());
        collectionPaths << url;
        qDebug() << "Collection dir added: " << url;
        setSettings({"collectionPath=", url});
        emit collectionPathChanged(url);
    }
}

void settings::setSettings(QStringList setting) {
    std::string strNew;
    // std::string strReplace;
    strNew = setting.at(0).toStdString() + setting.at(1).toStdString();
    bool replace = false;
    /**/
    // qDebug()<<setting.at(0);
    std::ifstream settings(Bae::SettingPath.toStdString() +
                           settingsName.toStdString());
    QStringList newline;
    std::string line;
    while (std::getline(settings, line)) {
        // qDebug()<<get_setting;
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
        std::ofstream write(Bae::SettingPath.toStdString() + settingsName.toStdString());

        for (auto ln : newline) {
            write << ln.toStdString() << std::endl;
        }

    } else {
        std::ofstream write(Bae::SettingPath.toStdString() + settingsName.toStdString(),
                            std::ios::app);
        write << strNew << std::endl;
    }
}

void settings::removeSettings(QStringList setting) {
    std::string strNew;
    // std::string strReplace;
    strNew = "";
    bool replace = false;
    /**/
    // qDebug()<<setting.at(0);
    std::ifstream settings(Bae::SettingPath.toStdString() +
                           settingsName.toStdString());
    QStringList newline;
    std::string line;
    while (std::getline(settings, line)) {
        // qDebug()<<get_setting;
        if (QString::fromStdString(line).contains(setting.at(0)) &&
                QString::fromStdString(line).contains(setting.at(1))) {

            replace = true;
            newline << QString::fromStdString(strNew);

        } else {
            newline << QString::fromStdString(line);
        }
    }

    if (replace)
    {
        std::ofstream write(Bae::SettingPath.toStdString() + settingsName.toStdString());

        for (auto ln : newline)
            write << ln.toStdString() << std::endl;


    } else
    {
        std::ofstream write(Bae::SettingPath.toStdString() + settingsName.toStdString(),
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
    QSqlQuery query = collection_db.getQuery("SELECT url FROM tracks");
    while (query.next())
    {
        QString location = query.value(1).toString();
        if(!location.startsWith(Bae::YoutubeCachePath,Qt::CaseInsensitive)) //exclude the youtube cache folder
        {
            if (!dirs.contains(QFileInfo(location).dir().path()) && Bae::fileExists(location)) //check if parent dir isn't already in list and it exists
            {
                QString dir = QFileInfo(location).dir().path();
                dirs << dir;

                QDirIterator it(dir, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories); // get all the subdirectories to watch
                while (it.hasNext())
                {
                    QString subDir = QFileInfo(it.next()).path();

                    if(QFileInfo(subDir).isDir() && !dirs.contains(subDir))
                        dirs <<subDir;
                }

            }
        }
    }

    addToWatcher(dirs);
}

void settings::handleDirectoryChanged(const QString &dir)
{
}

void settings::readSettings()
{
    std::ifstream settings(Bae::SettingPath.toStdString() +
                           settingsName.toStdString());
    std::string line;
    while (std::getline(settings, line))
    {
        auto get_setting = QString::fromStdString(line);
        // qDebug()<<get_setting;
        if (get_setting.contains("collectionPath="))
        {
            collectionPaths << get_setting.replace("collectionPath=", "");
            //            qDebug() << "Setting the cPath: "
            //                     << get_setting.replace("collectionPath=", "");
            ui->collectionPath->addItem(get_setting.replace("collectionPath=", ""));

        }
    }
}

void settings::setToolbarIconSize(const int &iconSize)
{
    qDebug()<<"setToolbarIconSize"<<iconSize;
    switch (iconSize) {
    case 16:
        ui->toolbarIconSize->setCurrentIndex(0);
        break;
    case 22:
        ui->toolbarIconSize->setCurrentIndex(1);
        break;
    case 32:
        ui->toolbarIconSize->setCurrentIndex(2);
        break;
    default:
        qDebug() << "error setting icon size";
    }

    emit toolbarIconSizeChanged(iconSize);

}

bool settings::checkCollection()
{
    if (Bae::fileExists(Bae::CollectionDBPath + collectionDBName))
    {
        qDebug() << "The CollectionDB does exists.";

        collection_db.setUpCollection(Bae::CollectionDBPath + collectionDBName);
        collectionWatcher();
        return true;
    } else return false;
}


void settings::createCollectionDB()
{
    qDebug() << "The CollectionDB doesn't exists. Going to create the database "
                "and tables";

    collection_db.setUpCollection(Bae::CollectionDBPath + collectionDBName);
    collection_db.prepareCollectionDB();

}



void settings::populateDB(const QString &path)
{

    qDebug() << "Function Name: " << Q_FUNC_INFO
             << "new path for database action: " << path;
    trackSaver.requestPath(path);
    this->ui->sourcesFrame->setEnabled(false);
}


void settings::fetchArt()
{

    this->trackSaver.requestArtwork();
    nof.notify("Fetching art","this might take some time depending on your collection size and internet connection speed...");
    ui->label->show();
    movie->start();


}

void settings::on_pushButton_clicked()
{
    // QMessageBox::about(this, "Babe Tiny Music Player","Version: 0.0
    // Alpha\nWritten and designed\nby: Camilo Higuita");
    about_ui->show();
}

void settings::on_debugBtn_clicked()
{
    /*qDebug()<<"Current files being watched:";
    for(auto file: watcher->files()) qDebug()<<file;*/
    qDebug()<<"Current dirs being watched:";
    for(auto dir: watcher->directories()) qDebug()<<dir;

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
