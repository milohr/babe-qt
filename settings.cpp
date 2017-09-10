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
    qDebug() << "Getting collectionDB info from: " << collectionDBPath;
    qDebug() << "Getting settings info from: " << settingPath;
    qDebug() << "Getting artwork files from: " << cachePath;
    qDebug() << "Getting extension files files from: " << extensionFetchingPath;

    ui->ytLineEdit->setText(extensionFetchingPath);
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

    connect(&collection_db, &CollectionDB::DBactionFinished, this,&settings::finishedAddingTracks);
    connect(&collection_db,&CollectionDB::progress, ui->progressBar,&QProgressBar::setValue);
    connect(this, &settings::collectionPathChanged, this, &settings::populateDB);

    ui->ytLineEdit->setText(extensionFetchingPath);
    ytFetch = new YouTube(this);
    connect(ytFetch,&YouTube::youtubeTrackReady,this,&settings::youtubeTrackReady);
    ytFetch->searchPendingFiles();
    extensionWatcher = new QFileSystemWatcher();
    extensionWatcher->addPath(extensionFetchingPath);
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
    QStringList urls;

    QDirIterator it(extensionFetchingPath, QStringList() << "*.babe", QDir::Files);

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

        if(collection_db.removePath(pathToRemove))
        {
            removeSettings({"collectionPath=", pathToRemove});
            collectionPaths.removeAll(pathToRemove);

            refreshCollectionPaths();
            refreshWatchFiles();
            ui->remove->setEnabled(false);
            collection_db.setCollectionLists();
            collection_db.cleanCollectionLists();
            emit collectionDBFinishedAdding();
        }
        //emit collectionPathRemoved(pathToRemove);
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
        if(!query.value(Bae::DBCols::URL).toString().contains(youtubeCachePath))
        {
            if (!dirs.contains(QFileInfo(query.value(Bae::DBCols::URL).toString()).dir().path())&&QFileInfo(query.value(Bae::DBCols::URL).toString()).exists())
            {

                QString dir =QFileInfo(query.value(Bae::DBCols::URL).toString()).dir().path();

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

CollectionDB &settings::getCollectionDB() { return collection_db; }

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
        ui->collectionPath->addItem(url);
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
    std::ifstream settings(settingPath.toStdString() +
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
    // std::string strReplace;
    strNew = "";
    bool replace = false;
    /**/
    // qDebug()<<setting.at(0);
    std::ifstream settings(settingPath.toStdString() +
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
        std::ofstream write(settingPath.toStdString() + settingsName.toStdString());

        for (auto ln : newline)
            write << ln.toStdString() << std::endl;


    } else
    {
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
    QSqlQuery query = collection_db.getQuery("SELECT url FROM tracks");
    while (query.next())
    {
        QString location = query.value(1).toString();
        if(!location.startsWith(youtubeCachePath,Qt::CaseInsensitive)) //exclude the youtube cache folder
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

void settings::handleDirectoryChanged(QString dir)
{
    qDebug() << "this directory changed: " << dir;

    emit dirChanged(dir);
}

void settings::readSettings()
{
    std::ifstream settings(settingPath.toStdString() +
                           settingsName.toStdString());
    std::string line;
    while (std::getline(settings, line))
    {
        auto get_setting = QString::fromStdString(line);
        // qDebug()<<get_setting;
        if (get_setting.contains("collectionPath="))
        {
            collectionPaths << get_setting.replace("collectionPath=", "");
            qDebug() << "Setting the cPath: "
                     << get_setting.replace("collectionPath=", "");
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


    if (Bae::fileExists(collectionDBPath + collectionDBName))
    {
        qDebug() << "The CollectionDB does exists.";

        collection_db.setUpCollection(collectionDBPath + collectionDBName);
        collectionWatcher();

        return true;

    } else return false;

}


void settings::createCollectionDB()
{
    qDebug() << "The CollectionDB doesn't exists. Going to create the database "
                "and tables";

    collection_db.setUpCollection(collectionDBPath + collectionDBName);
    collection_db.prepareCollectionDB();

}

void settings::populateDB(const QString &path)
{

    qDebug() << "Function Name: " << Q_FUNC_INFO
             << "new path for database action: " << path;

    QStringList urlCollection;

    if (QFileInfo(path).isDir())
    {
        QDirIterator it(path, formats, QDir::Files, QDirIterator::Subdirectories);

        while (it.hasNext()) urlCollection << it.next();

    } else if (QFileInfo(path).isFile()) urlCollection << path;

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(urlCollection.size());
    ui->progressBar->show();
    collection_db.addTrack(urlCollection);

}

void settings::finishedAddingTracks()
{
    ui->progressBar->hide();
    ui->progressBar->setValue(0);

    nof.notify("Songs added to collection","finished writting new songs to the collection :)");
    qDebug() << "good to hear it finished yay! now going to fetch artwork";

    collectionWatcher();
    emit refreshTables();
    fetchArt();

}

void settings::fetchArt()
{

    nof.notify("Fetching art","this might take some time depending on your collection size and internet connection speed...");

    int amountArtists=0;
    int amountAlbums=0;

    ui->label->show();
    movie->start();

    QString queryTxt;
    QSqlQuery query_Covers;
    QSqlQuery query_Heads;

    queryTxt = QString("SELECT %1, %2 FROM %3 WHERE %4 = ''").arg(Bae::DBColsMap[Bae::DBCols::ALBUM],
            Bae::DBColsMap[Bae::DBCols::ARTIST],Bae::DBTablesMap[Bae::DBTables::ALBUMS],Bae::DBColsMap[Bae::DBCols::ARTWORK]);
    query_Covers.prepare(queryTxt);

    qDebug()<<"FETCHART"<<queryTxt;

    queryTxt = QString("SELECT %1 FROM %2 WHERE %3 = ''").arg(Bae::DBColsMap[Bae::DBCols::ARTIST],
            Bae::DBTablesMap[Bae::DBTables::ARTISTS],Bae::DBColsMap[Bae::DBCols::ARTWORK]);
    query_Heads.prepare(queryTxt);

    qDebug()<<"FETCHART"<<queryTxt;

    if(query_Covers.exec())
        while (query_Covers.next())
        {
            QString album = query_Covers.value(Bae::DBColsMap[Bae::DBCols::ALBUM]).toString();
            QString artist = query_Covers.value(Bae::DBColsMap[Bae::DBCols::ARTIST]).toString();
            QString title;
            QSqlQuery query_Title =
                    collection_db.getQuery("SELECT title FROM tracks WHERE artist = \""+artist+"\" AND album = \""+album+"\"");
            if(query_Title.next()) title=query_Title.value(Bae::DBColsMap[Bae::DBCols::TITLE]).toString();

            collection_db.insertCoverArt("",{{Bae::DBCols::ALBUM,album},{Bae::DBCols::ARTIST,artist}});

            Pulpo art({{Bae::DBCols::TITLE,title},{Bae::DBCols::ARTIST,artist},{Bae::DBCols::ALBUM,album}});

            connect(&art, &Pulpo::albumArtReady,[this,&art] (QByteArray array){ art.saveArt(array,this->cachePath); });
            connect(&art, &Pulpo::artSaved, &collection_db, &CollectionDB::insertCoverArt);

            if (art.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::LastFm)) qDebug()<<"using lastfm";
            else if(art.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::Spotify)) qDebug()<<"using spotify";
            else if(art.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::GeniusInfo)) qDebug()<<"using genius";
            else art.albumArtReady(QByteArray());
            amountAlbums++;
        }
    else qDebug()<<"fetchArt queryCover failed";


    if(query_Heads.exec())
        while (query_Heads.next())
        {
            QString artist = query_Heads.value(Bae::DBColsMap[Bae::DBCols::ARTIST]).toString();
            qDebug()<< "QUERYHEAD ARTIOSTSSSSS:"<<artist;
            collection_db.insertHeadArt("",{{Bae::DBCols::ARTIST,artist}});

            Pulpo art({{Bae::DBCols::ARTIST,artist}});

            connect(&art, &Pulpo::artistArtReady,[this,&art] (QByteArray array){ art.saveArt(array,this->cachePath); });
            connect(&art, &Pulpo::artSaved, &collection_db, &CollectionDB::insertHeadArt);

            art.fetchArtistInfo(Pulpo::ArtistArt,Pulpo::LastFm);

            amountArtists++;
        }
    else qDebug()<<"fetchArt queryHeads failed";


    nof.notify("Finished fetching art","the artwork for your collection is now ready :)\n "+QString::number(amountArtists)+" artists and "+QString::number(amountAlbums)+" albums");
    movie->stop();
    ui->label->hide();

    emit collectionDBFinishedAdding();
    // emit refreshTables();
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

void settings::on_ytBtn_clicked()
{

}

void settings::on_fetchBtn_clicked()
{
    //    if(!ui->fetch->text().isEmpty())
    //    {
    //        ytFetch->fetch({ui->fetch->text()});
    //        ui->fetch->clear();
    //    }
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
