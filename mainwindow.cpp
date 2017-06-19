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


#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(const QStringList &files, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    this->setWindowIcon(QIcon(":Data/data/babe_48.svg"));
    this->setWindowIconText("Babe...");
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->defaultWindowFlags = this->windowFlags();

    connect(this, &MainWindow::finishRefresh,[=]()
    {
        qDebug()<<"FINISHED REFRESHING";
    });
    album_art = new Album(this);

    connect(album_art,&Album::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(album_art,&Album::changedArt,this,&MainWindow::changedArt);
    connect(album_art,&Album::babeAlbum_clicked,this,&MainWindow::babeAlbum);

    album_art->createAlbum("","",":Data/data/babe.png",BaeUtils::BIG_ALBUM,0,false);

    ALBUM_SIZE = album_art->getSize();

    album_art->setFixedSize(ALBUM_SIZE,ALBUM_SIZE);
    album_art->setTitleGeometry(0,0,ALBUM_SIZE,static_cast<int>(ALBUM_SIZE*0.15));
    album_art->showTitle(false);
    album_art->showPlayBtn=false;

    ui->controls->installEventFilter(this);

    this->setMinimumSize(ALBUM_SIZE*3,0);

    this->defaultGeometry = (QStyle::alignedRect(
                                 Qt::LeftToRight,
                                 Qt::AlignCenter,
                                 qApp->desktop()->availableGeometry().size()*0.7,
                                 qApp->desktop()->availableGeometry()
                                 ));

    this->setGeometry( this->loadSettings("GEOMETRY","MAINWINDOW",defaultGeometry).toRect());
    this->saveSettings("GEOMETRY",this->geometry(),"MAINWINDOW");

    player = new QMediaPlayer();
    //mpris = new Mpris(this);

    connect(this, &MainWindow::finishedPlayingSong, this, &MainWindow::addToPlayed);
    connect(this,&MainWindow::fetchCover,this,&MainWindow::setCoverArt);
    connect(this,&MainWindow::collectionChecked,this,&MainWindow::refreshTables);

    //* SETUP BABE PARTS *//
    this->setUpViews();
    this->setUpPlaylist();
    this->setUpRightFrame();
    this->setUpCollectionViewer();

    settings_widget->setToolbarIconSize(this->loadSettings("TOOLBAR","MAINWINDOW",QVariant(16)).toInt());

    //* CHECK FOR DATABASE *//
    if(settings_widget->checkCollection())
    {
        settings_widget->getCollectionDB().setCollectionLists();
        this->addToPlaylist(connection.getTrackData(loadSettings("PLAYLIST","MAINWINDOW").toStringList()),false, APPENDBOTTOM);
        if(this->mainList->rowCount()==0) populateMainList();
        emit collectionChecked();

    } else settings_widget->createCollectionDB();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]()
    {
        timer->stop();
        if(!current_song.isEmpty())
            infoTable->getTrackInfo(current_song[BabeTable::TITLE],current_song[BabeTable::ARTIST],current_song[BabeTable::ALBUM]);
        qDebug()<<"GETTING SONG INFO";
    });

    connect(&nof,&Notify::babeSong,this,&MainWindow::babeIt);
    connect(&nof,&Notify::skipSong,this,&MainWindow::next);
    connect(updater, &QTimer::timeout, this, &MainWindow::update);
    player->setVolume(100);

    /*LOAD THE STYLE*/
    QFile styleFile(stylePath);
    if(styleFile.exists())
    {
        qDebug()<<"A Babe style file exists";
        styleFile.open(QFile::ReadOnly);
        QString style(styleFile.readAll());
        this->setStyleSheet(style);
    }

    if(!files.isEmpty())
    {
        this->appendFiles(files, APPENDTOP);
        current_song_pos = 0;
    }else  current_song_pos = this->loadSettings("PLAYLIST_POS","MAINWINDOW",QVariant(0)).toInt();



    if(mainList->rowCount()>0)
    {
        mainList->setCurrentCell(current_song_pos>=mainList->rowCount()? 0 : current_song_pos,BabeTable::TITLE);
        loadTrack();
        collectionView();
        go_playlistMode();

    }else if(collectionTable->rowCount()>0) collectionView();
    else settingsView();

    updater->start(100);

    if(this->loadSettings("MINIPLAYBACK","MAINWINDOW",false).toBool())
        emit ui->miniPlaybackBtn->clicked();
    else ui->controls->setVisible(false);

    movePanel(this->loadSettings("PANEL_POS","MAINWINDOW",RIGHT).toInt());


}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadTrackAt(const int &pos)
{
    mainList->setCurrentCell(pos,BabeTable::TITLE);
    loadTrack();
    mainList->item(prev_song_pos+1,BabeTable::TITLE)->setIcon(QIcon());//this needs to get fixed

}

void MainWindow::saveSettings(const QString &key, const QVariant &value, const QString &group)
{
    QSettings setting("Babe","babe");
    setting.beginGroup(group);
    setting.setValue(key,value);
    setting.endGroup();
}

QVariant MainWindow::loadSettings(const QString &key, const QString &group, const QVariant &defaultValue)
{
    QVariant variant;
    QSettings setting("Babe","babe");
    setting.beginGroup(group);
    variant = setting.value(key,defaultValue);
    setting.endGroup();

    return variant;
}

//*HERE THE MAIN VIEWS GET SETUP WITH THEIR SIGNALS AND SLOTS**//
void MainWindow::setUpViews()
{
    settings_widget = new settings(this); //this needs to go first

    playlistTable = new PlaylistsView(this);
    connect(playlistTable->table,&BabeTable::tableWidget_doubleClicked, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(playlistTable->table,&BabeTable::removeIt_clicked,this,&MainWindow::removeSong);
    connect(playlistTable->table,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(playlistTable->table,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(playlistTable->table,&BabeTable::infoIt_clicked,this,&MainWindow::infoIt);
    connect(playlistTable->table,&BabeTable::previewStarted,this,&MainWindow::pause);
    connect(playlistTable->table,&BabeTable::previewFinished,this,&MainWindow::play);
    connect(playlistTable->table,&BabeTable::playItNow,this,&MainWindow::playItNow);
    connect(playlistTable->table,&BabeTable::appendIt, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(playlistTable->table,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    collectionTable = new BabeTable(this);
    collectionTable->showColumn(BabeTable::STARS);
    collectionTable->showColumn(BabeTable::GENRE);
    //connect(collectionTable, &BabeTable::tableWidget_doubleClicked, this, &MainWindow::addToPlaylist);
    connect(collectionTable,&BabeTable::tableWidget_doubleClicked, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(collectionTable,&BabeTable::finishedPopulating,[this]()
    {
        collectionTable->setTableOrder(BabeTable::ARTIST,BabeTable::ASCENDING);
    });
    connect(collectionTable,&BabeTable::removeIt_clicked,this,&MainWindow::removeSong);
    connect(collectionTable,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(collectionTable,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(collectionTable,&BabeTable::infoIt_clicked,this,&MainWindow::infoIt);
    connect(collectionTable,&BabeTable::previewStarted,this,&MainWindow::pause);
    connect(collectionTable,&BabeTable::previewFinished,this,&MainWindow::play);
    connect(collectionTable,&BabeTable::playItNow,this,&MainWindow::playItNow);
    connect(collectionTable,&BabeTable::appendIt, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(collectionTable,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    mainList = new BabeTable(this);
    mainList->setObjectName("mainList");
    mainList->hideColumn(BabeTable::ALBUM);
    mainList->hideColumn(BabeTable::ARTIST);
    mainList->horizontalHeader()->setVisible(false);
    mainList->enableRowColoring(true);
    mainList->enableRowDragging(true);
    mainList->enablePreview(false);
    //    mainList->setBackgroundRole(QPalette::Dark);
    //mainList->setSelectionMode(QAbstractItemView::SingleSelection);

    mainList->setAddMusicMsg("\nDrag and drop music here!","face-ninja");
    connect(mainList,&BabeTable::indexesMoved,[this](int  row, int newRow)
    {
        if(row>current_song_pos && newRow<current_song_pos )
        {
            current_song_pos++; prev_song_pos++;
            if(prev_queued_song_pos!=-1) prev_queued_song_pos++;
        }
        else if(row<current_song_pos && newRow>=current_song_pos)
        {
            current_song_pos--; prev_song_pos--;
            if(prev_queued_song_pos!=-1) prev_queued_song_pos--;

        }
    });

    connect(mainList,&BabeTable::indexRemoved,[this](int  row)
    {
        if(row<current_song_pos)
        {
            current_song_pos--; prev_song_pos--;
            if(prev_queued_song_pos!=-1) prev_queued_song_pos--;
        }

    });

    connect(mainList,&BabeTable::tableWidget_doubleClicked,this,&MainWindow::on_mainList_clicked);
    connect(mainList,&BabeTable::removeIt_clicked,this,&MainWindow::removeSong);
    connect(mainList,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(mainList,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(mainList,&BabeTable::moodIt_clicked,mainList,&BabeTable::colorizeRow);
    connect(mainList,&BabeTable::infoIt_clicked,this,&MainWindow::infoIt);
    connect(mainList,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);

    filterList = new BabeTable(this);
    filterList->hideColumn(BabeTable::ALBUM);
    filterList->hideColumn(BabeTable::ARTIST);
    filterList->horizontalHeader()->setVisible(false);
    filterList->enableRowColoring(true);

    filterList->setAddMusicMsg("\nDidn't find anything!","face-surprise");
    connect(filterList,&BabeTable::tableWidget_doubleClicked, [this] (QList<QMap<int, QString>> list)
    {
        playItNow(list);
        mainListView->setCurrentIndex(MAINPLAYLIST);
        ui->filter->setText("");


    });
    connect(filterList,&BabeTable::removeIt_clicked,this,&MainWindow::removeSong);
    connect(filterList,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(filterList,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(filterList,&BabeTable::moodIt_clicked,mainList,&BabeTable::colorizeRow);
    connect(filterList,&BabeTable::infoIt_clicked,this,&MainWindow::infoIt);
    connect(filterList,&BabeTable::previewStarted,this,&MainWindow::pause);
    connect(filterList,&BabeTable::previewFinished,this,&MainWindow::play);
    connect(filterList,&BabeTable::appendIt, [this] (QList<QMap<int, QString>> list)
    {
        addToPlaylist(list,false,APPENDAFTER);

    });
    connect(filterList,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    mainListView = new QStackedWidget(this);
    mainListView->setFrameShape(QFrame::NoFrame);
    mainListView->addWidget(mainList);
    mainListView->addWidget(filterList);

    onlineFetcher = new web_jgm90();
    resultsTable=new BabeTable(this);
    //    resultsTable->passStyle("QHeaderView::section { background-color:#575757; color:white; }");
    resultsTable->horizontalHeader()->setHighlightSections(true);
    resultsTable->setVisibleColumn(BabeTable::STARS);
    resultsTable->showColumn(BabeTable::GENRE);
    connect(resultsTable,&BabeTable::tableWidget_doubleClicked, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(resultsTable,&BabeTable::removeIt_clicked,this,&MainWindow::removeSong);
    connect(resultsTable,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(resultsTable,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(resultsTable,&BabeTable::infoIt_clicked,this,&MainWindow::infoIt);
    connect(resultsTable,&BabeTable::previewStarted,this,&MainWindow::pause);
    connect(resultsTable,&BabeTable::previewFinished,this,&MainWindow::play);
    connect(resultsTable,&BabeTable::playItNow,this,&MainWindow::playItNow);
    connect(resultsTable,&BabeTable::appendIt, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(resultsTable,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    rabbitTable = new RabbitView(this);
    connect(rabbitTable,&RabbitView::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(rabbitTable->getTable(),&BabeTable::tableWidget_doubleClicked, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(rabbitTable->getTable(),&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(rabbitTable->getTable(),&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(rabbitTable->getTable(),&BabeTable::infoIt_clicked,this,&MainWindow::infoIt);
    connect(rabbitTable->getTable(),&BabeTable::previewStarted,this,&MainWindow::pause);
    connect(rabbitTable->getTable(),&BabeTable::previewFinished,this,&MainWindow::play);
    connect(rabbitTable->getTable(),&BabeTable::playItNow,this,&MainWindow::playItNow);
    connect(rabbitTable->getTable(),&BabeTable::appendIt, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(rabbitTable->getTable(),&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    albumsTable = new AlbumsView(false,this);
    connect(albumsTable->albumTable,&BabeTable::tableWidget_doubleClicked, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(albumsTable->albumTable,&BabeTable::removeIt_clicked,this,&MainWindow::removeSong);
    connect(albumsTable->albumTable,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(albumsTable->albumTable,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(albumsTable->albumTable,&BabeTable::infoIt_clicked,this,&MainWindow::infoIt);
    connect(albumsTable,&AlbumsView::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(albumsTable,&AlbumsView::babeAlbum_clicked,this,&MainWindow::babeAlbum);
    connect(albumsTable,&AlbumsView::albumDoubleClicked,this,&MainWindow::albumDoubleClicked);
    connect(albumsTable,&AlbumsView::expandTo,this,&MainWindow::expandAlbumList);
    connect(albumsTable->albumTable,&BabeTable::previewStarted,this,&MainWindow::pause);
    connect(albumsTable->albumTable,&BabeTable::previewFinished,this,&MainWindow::play);
    connect(albumsTable->albumTable,&BabeTable::playItNow,this,&MainWindow::playItNow);
    connect(albumsTable->albumTable,&BabeTable::appendIt, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(albumsTable->albumTable,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    artistsTable = new AlbumsView(true,this);
    artistsTable->expandBtn->setVisible(false);
    artistsTable->albumTable->showColumn(BabeTable::ALBUM);
    connect(artistsTable->albumTable,&BabeTable::tableWidget_doubleClicked, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(artistsTable->albumTable,&BabeTable::removeIt_clicked,this,&MainWindow::removeSong);
    connect(artistsTable->albumTable,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(artistsTable->albumTable,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(artistsTable->albumTable,&BabeTable::infoIt_clicked,this,&MainWindow::infoIt);
    connect(artistsTable,&AlbumsView::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(artistsTable,&AlbumsView::babeAlbum_clicked,this,&MainWindow::babeAlbum);
    connect(artistsTable,&AlbumsView::albumDoubleClicked,this,&MainWindow::albumDoubleClicked);
    connect(artistsTable->albumTable,&BabeTable::previewStarted,this,&MainWindow::pause);
    connect(artistsTable->albumTable,&BabeTable::previewFinished,this,&MainWindow::play);
    connect(artistsTable->albumTable,&BabeTable::playItNow,this,&MainWindow::playItNow);
    connect(artistsTable->albumTable,&BabeTable::appendIt, [this] (QList<QMap<int, QString>> list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(artistsTable->albumTable,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    infoTable = new InfoView(this);
    connect(infoTable,&InfoView::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(infoTable,&InfoView::similarBtnClicked,[this](QStringList queries) { this->ui->search->setText(queries.join(",")); });
    connect(infoTable,&InfoView::tagsBtnClicked,[this](QStringList queries) { this->ui->search->setText(queries.join(",")); });
    connect(infoTable,&InfoView::tagClicked,[this](QString query) { this->ui->search->setText(query);});
    connect(infoTable,&InfoView::similarArtistTagClicked,[this](QString query) { this->ui->search->setText(query);});
    connect(infoTable,&InfoView::artistSimilarReady, [this] (QMap<QString,QByteArray> info)
    {
        feedRabbit();
        calibrateBtn_menu->actions().at(3)->setEnabled(true);
        rabbitTable->flushSuggestions(RabbitView::SIMILAR);
        qDebug()<<"&InfoView::artistSimilarReady:"<<info.keys();
        rabbitTable->populateArtistSuggestion(info);
        QStringList query;
        for (auto tag : info.keys()) query << QString("artist:"+tag).trimmed();
        auto searchResults = this->searchFor(query);
        if(!searchResults.isEmpty()) rabbitTable->populateGeneralSuggestion(searchResults);
    });

    connect(infoTable,&InfoView::albumTagsReady, [this] (QStringList tags)
    {
        auto searchResults = this->searchFor(tags);
        if(!searchResults.isEmpty()) rabbitTable->populateGeneralSuggestion(searchResults);
    });


    settings_widget->readSettings();
    connect(settings_widget,&settings::toolbarIconSizeChanged, this, &MainWindow::setToolbarIconSize);
    connect(settings_widget,&settings::collectionDBFinishedAdding, this, &MainWindow::collectionDBFinishedAdding);
    connect(settings_widget,&settings::dirChanged,this, &MainWindow::scanNewDir);
    //connect(settings_widget, SIGNAL(collectionPathRemoved(QString)),&settings_widget->getCollectionDB(), SLOT(removePath(QString)));
    connect(settings_widget,&settings::refreshTables,this,  &MainWindow::refreshTables);

    /* THE BUTTONS VIEWS */
    connect(ui->tracks_view,&QToolButton::clicked, this, &MainWindow::collectionView);
    connect(ui->albums_view,&QToolButton::clicked, this, &MainWindow::albumsView);
    connect(ui->artists_view,&QToolButton::clicked, this, &MainWindow::artistsView);
    connect(ui->playlists_view,&QToolButton::clicked, this, &MainWindow::playlistsView);
    connect(ui->rabbit_view,&QToolButton::clicked, this, &MainWindow::rabbitView);
    connect(ui->info_view,&QToolButton::clicked, this, &MainWindow::infoView);
    connect(ui->settings_view,&QToolButton::clicked, this,&MainWindow::settingsView);

    views = new QStackedWidget(this);
    views->setFrameShape(QFrame::NoFrame);
    views->addWidget(collectionTable);
    views->addWidget(albumsTable);
    views->addWidget(artistsTable);
    views->addWidget(playlistTable);
    views->addWidget(rabbitTable);
    views->addWidget(infoTable);
    views->addWidget(settings_widget);
    views->addWidget(resultsTable);

}

void MainWindow::setUpCollectionViewer()
{
    mainLayout = new QHBoxLayout();

    leftFrame_layout = new QGridLayout();
    leftFrame_layout->setContentsMargins(0,0,0,0);
    leftFrame_layout->setSpacing(0);
    leftFrame_layout->setMargin(0);

    leftFrame = new QFrame(this);
    leftFrame->setObjectName("leftFrame");
    leftFrame->setFrameShape(QFrame::StyledPanel);
    leftFrame->setFrameShadow(QFrame::Raised);
    leftFrame->setLayout(leftFrame_layout);


    line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setMaximumHeight(1);


    ui->viewsUtils->setContextMenuPolicy(Qt::ActionsContextMenu);

    auto showText = new QAction("Hide text");
    ui->viewsUtils->addAction(showText);
    connect(showText,&QAction::triggered,[showText,this]()
    {

        for(auto btn : ui->viewsUtils->children())
        {
            if(qobject_cast<QToolButton *>(btn)!=NULL)
            {
                if(qobject_cast<QToolButton *>(btn)->toolButtonStyle()==Qt::ToolButtonTextBesideIcon)
                {
                    qobject_cast<QToolButton *>(btn)->setToolButtonStyle(Qt::ToolButtonIconOnly);
                    showText->setText("Hide text");

                }
                else
                {
                    qobject_cast<QToolButton *>(btn)->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                    showText->setText("Show text");
                }

            }
        }

    });


    ui->search->setClearButtonEnabled(true);
    ui->collectionUtils->setVisible(false);
    connect(ui->saveResults,&QToolButton::clicked, this, &MainWindow::saveResultsTo);


    leftFrame_layout->addWidget(views,0,0);
    leftFrame_layout->addWidget(line,1,0);
    leftFrame_layout->addWidget(ui->viewsUtils,2,0);

    mainLayout->addWidget(leftFrame);
    mainLayout->addWidget(rightFrame,Qt::AlignRight);
    mainLayout->setContentsMargins(0,0,0,0);
    mainWidget= new QWidget(this);
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);
}

void MainWindow::setUpPlaylist()
{

    auto *playlistWidget_layout = new QGridLayout();
    playlistWidget_layout->setContentsMargins(0,0,0,0);
    playlistWidget_layout->setSpacing(0);

    playlistWidget = new QWidget(this);

    playlistWidget->setLayout(playlistWidget_layout);
    playlistWidget->setFixedWidth(ALBUM_SIZE);

    //    ui->controls->setParent(album_art);
    //    ui->controls->setGeometry(0,0,ALBUM_SIZE,ALBUM_SIZE);
    ui->controls->setMinimumSize(ALBUM_SIZE,ALBUM_SIZE);
    ui->controls->setMaximumSize(ALBUM_SIZE,ALBUM_SIZE);

    auto moveIt= new QAction("Move to left");
    moveIt->setShortcut(QKeySequence("Ctrl+m"));
    connect (moveIt, &QAction::triggered,[moveIt,this]()
    {
        if(playlistPos==RIGHT)
        {
            movePanel(LEFT);
            moveIt->setText("Move to right");
        }
        else if(playlistPos==LEFT)
        {
            movePanel(RIGHT);
            moveIt->setText("Move to left");

        }

    });
    ui->controls->addAction(moveIt);

    auto popPanel = new QAction("Pop panel out");
    popPanel->setShortcut(QKeySequence("Ctrl+p"));
    connect (popPanel, &QAction::triggered,[popPanel,this]()
    {
        if(playlistSta==OUT)
        {
            movePanel(IN);
            popPanel->setText("Pop panel out");
        }
        else if (playlistSta==IN)
        {
            movePanel(OUT);
            popPanel->setText("Pop panel in");

        }

    });
    ui->controls->addAction(popPanel);

    auto hideTimeLabels = new QAction("Hide time labels");
    connect (hideTimeLabels, &QAction::triggered,[hideTimeLabels,this]()
    {
        if(ui->time->isVisible()&&ui->duration->isVisible())
        {
            ui->time->setVisible(false);
            ui->duration->setVisible(false);
            hideTimeLabels->setText("Show time labels");
        }else
        {
            ui->time->setVisible(true);
            ui->duration->setVisible(true);
            hideTimeLabels->setText("Hide time labels");

        }
    });
    ui->controls->addAction(hideTimeLabels);
    ui->controls->setContextMenuPolicy(Qt::ActionsContextMenu);


    auto controlsColor = this->palette().color(QPalette::Window);
    ui->controls->setStyleSheet(QString("QWidget#controls{background-color: rgba(%1,%2,%3,70%);}").arg(QString::number(controlsColor.red()),QString::number(controlsColor.green()),QString::number(controlsColor.blue())));
    ui->miniPlaybackBtn->setVisible(false);

    seekBar = new QSlider(this);
    seekBar->installEventFilter(this);

    connect(seekBar,&QSlider::sliderMoved,this,&MainWindow::on_seekBar_sliderMoved);

    seekBar->setMaximum(1000);
    seekBar->setOrientation(Qt::Horizontal);
    seekBar->setContentsMargins(0,0,0,0);
    seekBar->setFixedHeight(5);
    seekBar->setStyleSheet(QString("QSlider { background:transparent;} QSlider::groove:horizontal {border: none; background: transparent; height: 5px; border-radius: 0; } QSlider::sub-page:horizontal { background: %1;border: none; height: 5px;border-radius: 0;} QSlider::add-page:horizontal {background: transparent; border: none; height: 5px; border-radius: 0; } QSlider::handle:horizontal {background: %1; width: 8px; } QSlider::handle:horizontal:hover {background: qlineargradient(x1:0, y1:0, x2:1, y2:1,stop:0 #fff, stop:1 #ddd);border: 1px solid #444;border-radius: 4px;}QSlider::sub-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::add-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::handle:horizontal:disabled {background: transparent;border: 1px solid #aaa;border-radius: 4px;}").arg(this->palette().color(QPalette::Highlight).name()));

    //    ui->playlistUtils->setBackgroundRole(QPalette::Button);

    ui->filterBox->setVisible(false);
    ui->filter->setClearButtonEnabled(true);
    ui->filterBtn->setChecked(false);

    ui->hide_sidebar_btn->setToolTip("Go Mini");
    ui->shuffle_btn->setToolTip("Shuffle");
    ui->tracks_view_2->setVisible(false);

    calibrateBtn_menu = new QMenu(this);
    ui->calibrateBtn->setMenu(calibrateBtn_menu);
    ui->calibrateBtn->setPopupMode(QToolButton::InstantPopup);

    auto refreshIt = new QAction("Calibrate...");
    connect(refreshIt, &QAction::triggered, [this]() { calibrateMainList(); });
    calibrateBtn_menu->addAction(refreshIt);

    auto clearIt = new QAction("Clear...");
    connect(clearIt, &QAction::triggered, [this]()
    {
        this->clearMainList();
        if(mainList->rowCount()>0)
        {
            this->mainList->setCurrentCell(current_song_pos,BabeTable::TITLE);
            this->mainList->item(current_song_pos,BabeTable::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));
        }
    });

    calibrateBtn_menu->addAction(clearIt);

    auto cleanIt = new QAction("Clean...");
    connect(cleanIt, &QAction::triggered, [this]() { mainList->removeRepeated(); /*this->removequeuedTracks();*/ });
    calibrateBtn_menu->addAction(cleanIt);

    auto similarIt = new QAction("Append Similar...");
    connect(similarIt, &QAction::triggered, [this]()
    {
        auto results = searchFor(infoTable->getSimilarArtistTags());

        int i = 1;
        if(mainList->rowCount()==0) i=0;

        for(auto track : results)
        {
            mainList->addRowAt(current_song_pos+i,track,true);
            //            mainList->item(current_song_pos+i,BabeTable::TITLE)->setIcon(QIcon::fromTheme("filename-space-amarok"));
            mainList->colorizeRow({current_song_pos+i},"#000");

            i++;
        }


        //        this->addToPlaylist(searchFor(infoTable->getTags()),true);
    });
    calibrateBtn_menu->addAction(similarIt);

    auto open = new QAction("Open...");
    connect(open, &QAction::triggered,this,&MainWindow::on_open_btn_clicked);
    calibrateBtn_menu->addAction(open);


    playlistWidget_layout->addWidget(album_art, 0,0,Qt::AlignTop);
    playlistWidget_layout->addWidget(ui->controls, 0,0,Qt::AlignTop);
    playlistWidget_layout->addWidget(ui->frame_4,1,0);
    playlistWidget_layout->addWidget(seekBar,2,0);
    playlistWidget_layout->addWidget(ui->frame_5,3,0);
    playlistWidget_layout->addWidget(mainListView,4,0);
    playlistWidget_layout->addWidget(ui->frame_6,5,0);
    playlistWidget_layout->addWidget(ui->playlistUtils,6,0);

}


void MainWindow::movePanel(const int &pos)
{

    auto position = QWidget::mapToGlobal(leftFrame->pos());


    switch(pos)
    {
    case RIGHT:
        this->mainLayout->removeWidget(rightFrame);
        this->mainLayout->insertWidget(1,rightFrame);
        playlistPos=RIGHT;
        break;
    case LEFT:
        this->mainLayout->removeWidget(rightFrame);
        this->mainLayout->insertWidget(0,rightFrame);
        playlistPos=LEFT;
        break;
    case OUT:
        if(viewMode != FULLMODE) expand();
        this->mainLayout->removeWidget(rightFrame);
        rightFrame->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        rightFrame->setWindowTitle("Playlist");
        //        rightFrame->setFrameShape(QFrame::NoFrame);
        rightFrame->show();
        //                rightFrame->window()->setContentsMargins(6,6,6,6);
        rightFrame->window()->setFixedWidth(rightFrame->minimumSizeHint().width());
        rightFrame->window()->move(position.x()+this->size().width(),this->pos().y());
        playlistSta=OUT;
        break;
    case IN:
        rightFrame->setWindowFlags(Qt::Widget);
        this->mainLayout->insertWidget(playlistPos==RIGHT?1:0,rightFrame);
        rightFrame->setFixedWidth(rightFrame->minimumSizeHint().width());

        rightFrame->show();
        playlistSta=IN;
        break;

    }
}

void MainWindow::setUpRightFrame()
{


    auto *rightFrame_layout = new QGridLayout();
    rightFrame_layout->setContentsMargins(0,0,0,0);
    rightFrame_layout->setSpacing(0);

    rightFrame = new QFrame(this);
    rightFrame->setObjectName("rightFrame");
    rightFrame->installEventFilter(this);
    rightFrame->setAcceptDrops(true);
    rightFrame->setLayout(rightFrame_layout);
    rightFrame->setFrameShadow(QFrame::Raised);
    rightFrame->setFrameShape(QFrame::StyledPanel);
    rightFrame->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    //    rightFrame->setMinimumHeight(ALBUM_SIZE*2);
    rightFrame_layout->addWidget(playlistWidget,0,0);
    rightFrame->setFixedWidth(rightFrame->minimumSizeHint().width());

}


void MainWindow::changedArt(const QMap<int, QString> &info)
{
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];
    QString path = info[Album::ART];
    settings_widget->getCollectionDB().execQuery(QString("UPDATE albums SET art = \"%1\" WHERE title = \"%2\" AND artist = \"%3\"").arg(path,album,artist) );
}

void MainWindow::albumDoubleClicked(const QMap<int, QString> &info)
{
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];

    QList<QMap<int,QString>> mapList;

    if(album.isEmpty())
        mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\" ORDER by album asc, track asc"));
    else if(!album.isEmpty()&&!artist.isEmpty())
        mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\" AND album = \""+album+"\" ORDER by track asc"));

    if(!mapList.isEmpty()) addToPlaylist(mapList,false, APPENDBOTTOM);

}



void MainWindow::playItNow(const QList<QMap<int,QString>> &list)
{
    if(!list.isEmpty())
    {

        if(list.size()==1)
        {
            auto it = this->mainList->getAllTableContent().indexOf(list.first());

            if( it!=-1)
                mainList->setCurrentCell(it,BabeTable::TITLE);
            else
            {
                addToPlaylist(list,false,APPENDBOTTOM);
                mainList->setCurrentCell(mainList->rowCount()-list.size(),BabeTable::TITLE);
            }

        }else if(list.size()>1)
        {
            addToPlaylist(list,false,APPENDBOTTOM);
            mainList->setCurrentCell(mainList->rowCount()-list.size(),BabeTable::TITLE);
        }

        this->loadTrack();
    }

}

void MainWindow::putAlbumOnPlay(const QMap<int,QString> &info)
{
    if(!info.isEmpty())
    {
        QString artist =info[Album::ARTIST];
        QString album = info[Album::ALBUM];

        if(!artist.isEmpty()||!album.isEmpty())
        {
            qDebug()<<"put on play<<"<<artist<<album;

            QList<QMap<int,QString>> mapList;

            if(album.isEmpty())
                mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\" ORDER by album asc, track asc"));
            else if(!album.isEmpty()&&!artist.isEmpty())
                mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\" AND album = \""+album+"\" ORDER by track asc"));

            if(!mapList.isEmpty()) this->putOnPlay(mapList);
        }
    }

}

void MainWindow::putOnPlay(const QList<QMap<int,QString>> &mapList)
{
    if(!mapList.isEmpty())
    {
        mainList->flushTable();
        currentList.clear();

        addToPlaylist(mapList,false,APPENDBOTTOM);

        if(mainList->rowCount()>0)
        {
            current_song_pos=0;
            prev_song_pos=current_song_pos;
            mainList->setCurrentCell(current_song_pos,BabeTable::TITLE);

            loadTrack();
        }
    }
}

void MainWindow::addToPlayed(const QString &url)
{
    if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM tracks WHERE location = \""+url+"\""))
    {
        qDebug()<<"Song totally played"<<url;

        QSqlQuery query = settings_widget->getCollectionDB().getQuery("SELECT * FROM tracks WHERE location = \""+url+"\"");

        int played = 0;
        while (query.next()) played = query.value(BabeTable::PLAYED).toInt();
        qDebug()<<played;

        if(settings_widget->getCollectionDB().insertInto("tracks","played",url,played+1))
            qDebug()<<played;

    }
}


bool MainWindow::eventFilter(QObject *object, QEvent *event)
{    

    if(object == this->ui->controls)
    {
        if(event->type()==QEvent::Enter)
            ui->miniPlaybackBtn->setVisible(true);

        else if(event->type()==QEvent::Leave && !miniPlayback)
            if(ui->miniPlaybackBtn->isVisible()) ui->miniPlaybackBtn->setVisible(false);
    }


    if (object == seekBar && seekBar->isEnabled())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            auto mevent = static_cast<QMouseEvent *>(event);
            qreal value = seekBar->minimum() + (seekBar->maximum() - seekBar->minimum()) * mevent->localPos().x() / seekBar->width();
            if (mevent->button() == Qt::LeftButton)
                emit seekBar->sliderMoved(qRound(value));

            event->accept();
            return true;
        }

        if (event->type() == QEvent::MouseMove)
        {
            auto mevent = static_cast<QMouseEvent *>(event);
            qreal value = seekBar->minimum() + (seekBar->maximum() - seekBar->minimum()) * mevent->localPos().x() / seekBar->width();
            if (mevent->buttons() & Qt::LeftButton)
                emit seekBar->sliderMoved(qRound(value));

            event->accept();
            return true;
        }
        if (event->type() == QEvent::MouseButtonDblClick)
        {
            event->accept();
            return true;
        }
    }

    //    if(object==ui->search)
    //    {
    //        if(event->type()==QEvent::Resize)
    //        {
    //            ui->playAll->setGeometry(ui->search->size().width()-48,(ui->search->size().height()/2)-8,16,16);

    //        }
    //    }


    if(object == rightFrame)
    {
        if(event->type()==QEvent::Enter && !stopped)
            this->showControls(true);


        if(event->type()==QEvent::Leave)
            this->showControls(false);

        if(event->type()==QEvent::DragEnter)
        {
            qDebug()<<"playlistWidget event filter DragEnter";

            event->accept();
        }

        if(event->type()==QEvent::DragLeave)
        {
            qDebug()<<"playlistWidget event filter DragLeave";

            event->accept();
        }

        if(event->type()==QEvent::DragMove)
        {
            qDebug()<<"playlistWidget event filter DragMove";

            event->accept();
        }

        if(event->type()==QEvent::Drop)
        {


            event->accept();
            QDropEvent* dropEvent = static_cast<QDropEvent*>(event);

            QList<QMap<int, QString>> mapList;
            QList<QUrl> urls = dropEvent->mimeData()->urls();
            qDebug()<< urls;

            if(urls.isEmpty())
            {
                auto info = dropEvent->mimeData()->text();
                auto infoList = info.split("/by/");

                if(infoList.size()==2)
                {
                    //qDebug()<<"album: " << infoList.at(0) << "artist: "<< infoList.at(1);
                    QString artist_ = infoList.at(1).simplified();
                    QString album_ = infoList.at(0).simplified();
                    mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist_+"\" and album = \""+album_+"\" ORDER by track asc "));

                }else
                    mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+info+"\" ORDER by album asc, track asc "));

                addToPlaylist(mapList,false,APPENDBOTTOM);

            }else
            {
                QList<QUrl> urls = dropEvent->mimeData()->urls();
                QStringList list;
                for(auto url : urls) list<<url.path();

                appendFiles(list);
            }



        }



    }
    return QMainWindow::eventFilter(object, event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{

    if(viewMode == FULLMODE )
    {
        this->saveSettings("GEOMETRY",this->geometry(),"MAINWINDOW");
        qDebug()<<"saved geometry: "<<this->geometry();
    }


    this->saveSettings("PLAYLIST",mainList->getTableColumnContent(BabeTable::LOCATION),"MAINWINDOW");
    this->saveSettings("PLAYLIST_POS", current_song_pos,"MAINWINDOW");
    this->saveSettings("TOOLBAR", this->iconSize,"MAINWINDOW");
    this->saveSettings("MINIPLAYBACK",miniPlayback,"MAINWINDOW");
    this->saveSettings("PANEL_POS",playlistPos,"MAINWINDOW");
    this->saveSettings("TIME_LABEL",ui->time->isVisible()&&ui->duration->isVisible(),"MAINWINDOW");

    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{

    //    if(views->currentIndex()==ALBUMS)
    //    {
    //        auto scrollSize = albumsTable->grid->verticalScrollBar()->size().width()+1;
    //        auto gridSize = albumsTable->grid->size().width()-scrollSize;
    //        auto amount = (gridSize/(albumsTable->getAlbumSize()+25));
    //        auto leftSpace = gridSize-amount*albumsTable->getAlbumSize();
    //        if(albumsTable->grid->isVisibleTo(this)) albumsTable->grid->setGridSize(QSize(albumsTable->getAlbumSize()+(leftSpace/amount),albumsTable->getAlbumSize()+25));
    //        qDebug()<<"gridSize:"<<gridSize<<"amount:"<<amount<<"left space: "<<leftSpace<<"scroll width:"<<scrollSize;
    //    }

    //    if(this->viewMode==MINIMODE)
    //    {

    //        this->setMaximumHeight(event->size().width()+5);
    //        this->setMinimumHeight(event->size().width()+5);

    //        album_art->setSize(event->size().width());
    //        int ALBUM_SIZE_ = album_art->getSize();
    //        ui->controls->setMaximumSize(ALBUM_SIZE_,ALBUM_SIZE_);
    //        ui->controls->setMinimumSize(ALBUM_SIZE_,ALBUM_SIZE_);

    //    }
    QMainWindow::resizeEvent(event);
}

void MainWindow::refreshTables() //tofix
{

    collectionTable->flushTable();
    collectionTable->populateTableView("SELECT * FROM tracks",false);

    albumsTable->populateTableView();
    albumsTable->hideAlbumFrame();

    artistsTable->populateTableViewHeads();
    artistsTable->hideAlbumFrame();

    playlistTable->list->clear();
    playlistTable->setDefaultPlaylists();
    playlistTable->setPlaylists(connection.getPlaylists());

    emit finishRefresh();

}


void MainWindow::keyPressEvent(QKeyEvent *event) //todo
{

    QMainWindow::keyPressEvent(event);
    /*switch (event->key())
    {
    case Qt::Key_Return :
    {
        lCounter = getIndex();
        if(lCounter != -1)
        {
            //ui->play->setChecked(false);
            ui->play_btn->setIcon(QIcon(":Data/data/media-playback-pause.svg"));
            ui->search->clear();
            loadTrack();
        }
        break;
    }
    case Qt::Key_Up :
    {
        int ind = getIndex() - 1;if(ind < 0)ind = ui->listWidget->count() - 1;
        ui->listWidget->setCurrentRow(ind);
        break;
    }
    case Qt::Key_Down :
    {
        int ind = getIndex() + 1;if(ind >= ui->listWidget->count())ind = 0;
        ui->listWidget->setCurrentRow(ind);
        break;
    }
    default :
    {
        //ui->search->setFocusPolicy(Qt::StrongFocus);
        //ui->search->setFocus();
        qDebug()<<"trying to focus the serachbar";
        break;
    }
    }*/
}


void MainWindow::enterEvent(QEvent *event)
{
    event->accept();
}

void MainWindow::leaveEvent(QEvent *event)
{
    event->accept();
}



void MainWindow::showControls(const bool &state)
{

    if(state)

    {
        if(!ui->controls->isVisible())
        {



            if(!miniPlayback) this->blurWidget(*album_art,15);
            else this->blurWidget(*album_art,28);

            ui->controls->setVisible(true);

        }
    }else
    {
        if(ui->controls->isVisible() && !miniPlayback)
        {
            this->blurWidget(*album_art,0);


            ui->controls->setVisible(false);



        }
    }



}

void MainWindow::dummy() { qDebug()<<"TEST on DUMMYT"; }

void MainWindow::setCoverArt(const QString &artist, const QString &album,const QString &title)
{
    qDebug()<<"Trying to retieve the cover art from Pulpo for"<< title << artist << album;
    Pulpo coverArt(title,artist,album);
    connect(&coverArt,&Pulpo::albumArtReady,this,&MainWindow::putPixmap);
    if (coverArt.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::LastFm)) qDebug()<<"using lastfm";
    else if(coverArt.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::Spotify)) qDebug()<<"using spotify";
    else if(coverArt.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::GeniusInfo)) qDebug()<<"using genius";
    else coverArt.albumArtReady(QByteArray());
}

void MainWindow::putPixmap(const QByteArray &array)
{
    if(!array.isEmpty()) album_art->putPixmap(array);
    else  album_art->putDefaultPixmap();
}

void MainWindow::setToolbarIconSize(const int &iconSize) //tofix
{

    this->iconSize = iconSize;

    for(auto obj: ui->controls->children())
        if(qobject_cast<QToolButton *>(obj)!=NULL)
            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(iconSize,iconSize));

    for(auto obj: ui->playlistUtils->children())
        if(qobject_cast<QToolButton *>(obj)!=NULL)
            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(iconSize,iconSize));

    //    for(auto obj: ui->collectionUtils->children())
    //        if(qobject_cast<QToolButton *>(obj)!=NULL)
    //            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(iconSize,iconSize));

    for(auto obj: ui->viewsUtils->children())
        if(qobject_cast<QToolButton *>(obj)!=NULL)
            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(iconSize,iconSize));

    //    for(auto obj: infoTable->infoUtils->children())
    //        if(qobject_cast<QToolButton *>(obj)!=NULL)
    //            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(iconSize,iconSize));

    //    for(auto obj: playlistTable->btnContainer->children())
    //        if(qobject_cast<QToolButton *>(obj)!=NULL)
    //            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(iconSize,iconSize));


    ui->controls->update();
    ui->playlistUtils->update();
    //    infoTable->infoUtils->update();
    //    playlistTable->btnContainer->update();
    //    ui->collectionUtils->update();
    ui->viewsUtils->update();

}

void MainWindow::collectionView()
{

    if(resultsTable->rowCount()>0) views->setCurrentIndex(RESULTS);
    else views->setCurrentIndex(COLLECTION);

    ui->tracks_view->setChecked(true);

    if(this->viewMode != FULLMODE) expand();

    ui->tracks_view->setChecked(true);
    prevIndex=views->currentIndex();
}

void MainWindow::albumsView()
{
    views->setCurrentIndex(ALBUMS);
    ui->albums_view->setChecked(true);

    if(this->viewMode != FULLMODE) expand();


    prevIndex = views->currentIndex();
}

void MainWindow::playlistsView()
{
    views->setCurrentIndex(PLAYLISTS);
    ui->playlists_view->setChecked(true);
    if(this->viewMode != FULLMODE) expand();


    prevIndex = views->currentIndex();
}

void MainWindow::rabbitView()
{
    views->setCurrentIndex(RABBIT);
    ui->rabbit_view->setChecked(true);
    if(this->viewMode != FULLMODE) expand();


    prevIndex = views->currentIndex();
}

void MainWindow::infoView()
{
    views->setCurrentIndex(INFO);
    ui->info_view->setChecked(true);

    if(this->viewMode != FULLMODE) expand();


    prevIndex = views->currentIndex();
}

void MainWindow::artistsView()
{
    views->setCurrentIndex(ARTISTS);
    ui->artists_view->setChecked(true);
    if(this->viewMode != FULLMODE) expand();

    prevIndex = views->currentIndex();
}


void MainWindow::settingsView()
{
    views->setCurrentIndex(SETTINGS);
    ui->settings_view->setChecked(true);
    if(this->viewMode != FULLMODE) expand();

    prevIndex = views->currentIndex();
}

void MainWindow::expand()
{

    this->viewMode=FULLMODE;

    if(album_art->getSize()!=ALBUM_SIZE)
    {
        album_art->setSize(ALBUM_SIZE);
        //        this->seekBar->setMaximumWidth(ALBUM_SIZE);
        ui->controls->setMaximumSize(ALBUM_SIZE,ALBUM_SIZE);
        ui->controls->setMinimumSize(ALBUM_SIZE,ALBUM_SIZE);

    }

    ui->tracks_view_2->setVisible(false);
    if(!leftFrame->isVisible()) leftFrame->setVisible(true);
    if(!ui->frame_4->isVisible()) ui->frame_4->setVisible(true);
    if(!mainList->isVisible()) mainListView->setVisible(true);
    if(!ui->frame_5->isVisible()) ui->frame_5->setVisible(true);
    if(!ui->frame_6->isVisible()) ui->frame_6->setVisible(true);
    if(!ui->playlistUtils->isVisible()) ui->playlistUtils->setVisible(true);

    album_art->borderColor=false;

    //    rightFrame->setFrameShadow(QFrame::Raised);
    //    rightFrame->setFrameShape(QFrame::StyledPanel);
    //    mainLayout->setContentsMargins(6,6,6,6);

    this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    this->setMinimumHeight(0);
    this->setMinimumSize(ALBUM_SIZE*3,0);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);
    animation->setStartValue(this->geometry());
    animation->setEndValue(this->loadSettings("GEOMETRY","MAINWINDOW",this->geometry()).toRect());

    animation->start();

    ui->hide_sidebar_btn->setToolTip("Go Mini");

//    this->setWindowFlags(this->windowFlags() & ~Qt::Tool & ~Qt::FramelessWindowHint & ~Qt::WindowStaysOnTopHint);
//    this->show();

}

void MainWindow::go_mini()
{

    this->viewMode=MINIMODE;

    //    this->miniPlayback =false;

    //    this->blurWidget(*album_art,15);
    //    album_art->restoreSaturation();

    //    ui->miniPlaybackBtn->setVisible(false);
    //    ui->miniPlaybackBtn->setIcon(QIcon::fromTheme("go-top"));

    leftFrame->setVisible(false);
    ui->frame_4->setVisible(false);
    mainListView->setVisible(false);
    ui->frame_5->setVisible(false);
    ui->frame_6->setVisible(false);
    ui->playlistUtils->setVisible(false);

    //album_art->borderColor=true;
    //        rightFrame->setFrameShape(QFrame::NoFrame);

    //    this->setStyleSheet("QMainWindow{border: 1px solid red; border-radius:2px;}");

    //    rightFrame->layout()->margin(0);
    //    rightFrame->layout()->spacing(0);
    //    this->setMinimumSize(ALBUM_SIZE/2,ALBUM_SIZE/2);
    this->setMaximumWidth(rightFrame->minimumSizeHint().width());
    QPropertyAnimation *animation = new QPropertyAnimation(this, "maximumHeight");
    animation->setDuration(200);
    animation->setStartValue(this->size().height());
    animation->setEndValue(ui->controls->height());

    animation->start();


//    this->setWindowFlags(this->windowFlags() | Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//    this->show();
    //this->updateGeometry();
    //this->setfix(minimumSizeHint());
    //this->adjustSize();
    ui->hide_sidebar_btn->setToolTip("Expand");


}

void MainWindow::go_playlistMode()
{

    if(playlistSta!=OUT)
    {
        this->saveSettings("GEOMETRY",this->geometry(),"MAINWINDOW");
        this->viewMode=PLAYLISTMODE;
        QString icon;
        switch(prevIndex)
        {
        case COLLECTION: icon = ui->tracks_view->icon().name(); break;
        case ALBUMS:  icon = ui->albums_view->icon().name(); break;
        case ARTISTS:  icon = ui->artists_view->icon().name(); break;
        case PLAYLISTS:  icon = ui->playlists_view->icon().name(); break;
        case RABBIT:  icon = ui->rabbit_view->icon().name(); break;
        case INFO: icon = ui->info_view->icon().name(); break;

        case SETTINGS:  icon= ui->settings_view->icon().name(); break;
        default:  icon="go-back";
        }
        ui->tracks_view_2->setIcon(QIcon::fromTheme(icon));


        if(!ui->frame_4->isVisible()) ui->frame_4->setVisible(true);
        if(!mainList->isVisible()) mainListView->setVisible(true);
        if(!ui->frame_5->isVisible()) ui->frame_5->setVisible(true);
        if(!ui->frame_6->isVisible()) ui->frame_6->setVisible(true);
        if(!ui->playlistUtils->isVisible()) ui->playlistUtils->setVisible(true);
        ui->tracks_view_2->setVisible(true);
        //        album_art->borderColor=false;
        this->setMinimumWidth(rightFrame->minimumSizeHint().width());
        QPropertyAnimation *animation = new QPropertyAnimation(this, "maximumWidth");
        animation->setDuration(200);
        animation->setStartValue(this->size().width());
        animation->setEndValue(rightFrame->minimumSizeHint().width());

        animation->start();

        leftFrame->setVisible(false);

        ui->hide_sidebar_btn->setToolTip("Go Mini");
    }


}

void MainWindow::keepOnTop(bool state)
{
    if (state) this->setWindowFlags(Qt::WindowStaysOnTopHint);
    else this->setWindowFlags(defaultWindowFlags);

    this->show();
}





void MainWindow::on_hide_sidebar_btn_clicked()
{
    if(playlistSta==OUT) emit ui->controls->actions().at(1)->triggered();

    switch(this->viewMode)
    {
    case FULLMODE:
        go_playlistMode(); break;

    case PLAYLISTMODE: go_mini(); break;

    case MINIMODE: expand(); break;
    }

}

void MainWindow::on_shuffle_btn_clicked() //tofix
{

    if(shuffle_state == REGULAR)
    {
        shuffle = true;
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-shuffle"));
        ui->shuffle_btn->setToolTip("Repeat");
        shuffle_state = SHUFFLE;

    }else if (shuffle_state == SHUFFLE)
    {

        shuffle = false;
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-repeat"));
        ui->shuffle_btn->setToolTip("Consecutive");
        shuffle_state = REGULAR;


    }

    //    else if(shuffle_state == REPEAT)
    //    {
    //        repeat = false;
    //        shuffle = false;
    //        ui->shuffle_btn->setIcon(QIcon::fromTheme("view-media-playlist"));
    //        ui->shuffle_btn->setToolTip("Shuffle");
    //        shuffle_state = REGULAR;
    //    }
}

void MainWindow::on_open_btn_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),QDir().homePath()+"/Music/", tr("Audio (*.mp3 *.wav *.mp4 *.flac *.ogg *.m4a)"));
    if(!files.isEmpty()) appendFiles(files);

}

void MainWindow::appendFiles(const QStringList &paths,const appendPos &pos)
{
    if(!paths.isEmpty())
    {
        QStringList trackList;
        for( auto url  : paths)
        {
            if(QFileInfo(url).isDir())
            {
                QDirIterator it(url, settings_widget->formats, QDir::Files, QDirIterator::Subdirectories);

                while (it.hasNext()) trackList<<it.next();

            }else if (QFileInfo(url).isFile())
                trackList<<url;
        }

        auto tracks = new Playlist();
        tracks->add(trackList);
        addToPlaylist(tracks->getTracksData(),false,pos);
    }
}

void MainWindow::populateMainList()
{
    auto results = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE babe = 1 ORDER by played desc"));
    mainList->populateTableView(results,true);
    mainList->resizeRowsToContents();
    currentList = mainList->getAllTableContent();
}

void MainWindow::updateList()
{
    mainList->flushTable();
    for(auto list: currentList)
        mainList->addRow(list,true);
}


void MainWindow::on_mainList_clicked(const QList<QMap<int, QString> > &list)
{
    Q_UNUSED(list);

    loadTrack();
}

void MainWindow::removeSong(const int &index)
{
    QObject* obj = sender();

    if(index != -1)
    {

        if(obj == mainList)
        {

            if(removeQueuedTrack(mainList->getRowData(index),index))
            {
                qDebug()<<"removedQueued track";
                if(current_song_pos>index) current_song_pos--;
            }
            else
            {
                mainList->removeRow(index);
                currentList.removeAt(index);
                if(current_song_pos>index) current_song_pos--;
            }

        }

    }
}

void MainWindow::feedRabbit()
{
    rabbitTable->flushSuggestions(RabbitView::GENERAL);
    rabbitTable->populateGeneralSuggestion(connection.getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+current_song[BabeTable::ARTIST]+"\"")));
    //    rabbitTable->populateGeneralSuggestion(connection.getTrackData(QString("SELECT * FROM tracks WHERE genre = \""+current_song[BabeTable::GENRE]+"\"")));
}

void MainWindow::expandAlbumList(const QString &artist, const QString &album)
{
    if(!artist.isEmpty())

    {
        emit ui->artists_view->clicked();
        artistsTable->getArtistInfo({{Album::ARTIST,artist},{Album::ALBUM,album}});

    }

}

void MainWindow::loadTrack()
{
    if(stopped) updater->start(100);

    prev_song = current_song;
    prev_song_pos = current_song_pos;

    if(prev_song_pos<this->mainList->rowCount() && mainList->item(current_song_pos,BabeTable::TITLE)->icon().name()!="clock")
        mainList->item(prev_song_pos,BabeTable::TITLE)->setIcon(QIcon());

    calibrateBtn_menu->actions().at(3)->setEnabled(false);

    current_song_pos = mainList->getIndex();
    current_song = mainList->getRowData(current_song_pos);

    if(mainList->item(current_song_pos,BabeTable::TITLE)->icon().name()!="clock")
        mainList->item(current_song_pos,BabeTable::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));

    mainList->scrollTo(mainList->model()->index(current_song_pos,BabeTable::TITLE));

    qDebug()<<"in mainlist="<<current_song[BabeTable::LOCATION];

    if(BaeUtils::fileExists(current_song[BabeTable::LOCATION]))
    {
        player->setMedia(QUrl::fromLocalFile(current_song[BabeTable::LOCATION]));
        wasPlaying = true;
        this->play();

        timer->start(3000);

        album_art->setTitle(current_song[BabeTable::ARTIST],current_song[BabeTable::ALBUM]);



        //CHECK IF THE SONG IS BABED IT OR IT ISN'T
        if(isBabed(current_song)) babedIcon(true);
        else babedIcon(false);

        loadMood();

        loadCover(current_song[BabeTable::ARTIST],current_song[BabeTable::ALBUM],current_song[BabeTable::TITLE]);

        if(miniPlayback)
        {
            this->blurWidget(*album_art,28);
            album_art->saturatePixmap(100);
        }


        if(!this->isActiveWindow())
            nof.notifySong(current_song,QPixmap(current_artwork));


    }else removeSong(current_song_pos);

}



bool MainWindow::isBabed(const QMap<int, QString> &track)
{
    if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM tracks WHERE location = \""+track[BabeTable::LOCATION]+"\" AND babe = \"1\""))
        return true;
    else return false;

}

void MainWindow::loadMood()
{
    QString color;
    QSqlQuery query = settings_widget->getCollectionDB().getQuery("SELECT * FROM tracks WHERE location = \""+ current_song[BabeTable::LOCATION]+"\"");
    if(query.exec())
        while (query.next())
            color=query.value(BabeTable::ART).toString();

    auto seekbarEffect = new QGraphicsColorizeEffect(this);
    //    auto controlsEffect = new QGraphicsColorizeEffect(this);

    if(!color.isEmpty())
    {
        seekbarEffect->setColor(QColor(color));
        seekbarEffect->setStrength(1.0);

        //        controlsEffect->setColor(QColor(color));
        //        controlsEffect->setStrength(0.2);

        //        seekBar->setStyleSheet(QString("QSlider\n{\nbackground:transparent;}\nQSlider::groove:horizontal {border: none; background: transparent; height: 5px; border-radius: 0; } QSlider::sub-page:horizontal {\nbackground: %1 ;border: none; height: 5px;border-radius: 0;} QSlider::add-page:horizontal {\nbackground: transparent; border: none; height: 5px; border-radius: 0; } QSlider::handle:horizontal {background: %1; width: 8px; } QSlider::handle:horizontal:hover {background: qlineargradient(x1:0, y1:0, x2:1, y2:1,stop:0 #fff, stop:1 #ddd);border: 1px solid #444;border-radius: 4px;}QSlider::sub-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::add-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::handle:horizontal:disabled {background: transparent;border: 1px solid #aaa;border-radius: 4px;}").arg(color));
        //        //mainList->setStyleSheet(QString("QTableWidget::item:selected {background:rgba( %1, %2, %3, 40); color: %4}").arg(QString::number(QColor(color).toRgb().red()),QString::number(QColor(color).toRgb().green()),QString::number(QColor(color).toRgb().blue()),mainList->palette().color(QPalette::WindowText).name()));
        //        ui->mainToolBar->setStyleSheet(QString("QToolBar { background-color:rgba( %1, %2, %3, 20); background-image:url('%4');} ").arg(QString::number(QColor(color).toRgb().red()),QString::number(QColor(color).toRgb().green()),QString::number(QColor(color).toRgb().blue()),":Data/data/pattern.png",this->palette().color(QPalette::BrightText).name()));


    }else
    {
        seekbarEffect->setStrength(0);
        //        controlsEffect->setStrength(0);

        //        seekBar->setStyleSheet(QString("QSlider { background:transparent;} QSlider::groove:horizontal {border: none; background: transparent; height: 5px; border-radius: 0; } QSlider::sub-page:horizontal { background: %1;border: none; height: 5px;border-radius: 0;} QSlider::add-page:horizontal {background: transparent; border: none; height: 5px; border-radius: 0; } QSlider::handle:horizontal {background: %1; width: 8px; } QSlider::handle:horizontal:hover {background: qlineargradient(x1:0, y1:0, x2:1, y2:1,stop:0 #fff, stop:1 #ddd);border: 1px solid #444;border-radius: 4px;}QSlider::sub-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::add-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::handle:horizontal:disabled {background: transparent;border: 1px solid #aaa;border-radius: 4px;}").arg(this->palette().color(QPalette::Highlight).name()));
        //        //mainList->setStyleSheet(QString("QTableWidget::item:selected {background:%1; color: %2}").arg(this->palette().color(QPalette::Highlight).name(),this->palette().color(QPalette::BrightText).name()));
        //        ui->mainToolBar->setStyleSheet(QString("QToolBar {background-color:rgba( 0, 0, 0, 0); background-image:url('%1');}").arg(":Data/data/pattern.png",this->palette().color(QPalette::Highlight).name(),this->palette().color(QPalette::BrightText).name()));

    }

    seekBar->setGraphicsEffect(seekbarEffect);
    //    ui->controls->setGraphicsEffect(controlsEffect);
}


bool MainWindow::loadCover(const QString &artist, const QString &album, const QString &title) //tofix separte getalbumcover from get artisthead
{
    Q_UNUSED(title);
    QString artistHead;

    //IF CURRENT SONG EXISTS IN THE COLLECTION THEN GET THE COVER FROM DB
    if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM tracks WHERE location = \""+current_song[BabeTable::LOCATION]+"\""))
    {
        artistHead = settings_widget->getCollectionDB().getArtistArt(artist);

        if(!artistHead.isEmpty())
        {
            infoTable->setArtistArt(artistHead);
            infoTable->artist->setArtist(artist);

        }else infoTable->setArtistArt(QString(":Data/data/cover.svg"));


        current_artwork = settings_widget->getCollectionDB().getAlbumArt(album, artist);

        if(!current_artwork.isEmpty())
            album_art->putPixmap(current_artwork);
        else  if (!artistHead.isEmpty())
        {
            current_artwork = artistHead;
            album_art->putPixmap(current_artwork);
        }
        else album_art->putDefaultPixmap();

        return true;

    }else
    {
        qDebug()<<"Song path does not exits in db so going to get artwork somehowelse <<"<<album<<artist;

        if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM albums WHERE title = \""+album+"\" AND artist = \""+artist+"\""))
        {
            artistHead = settings_widget->getCollectionDB().getArtistArt(artist);

            current_artwork = settings_widget->getCollectionDB().getAlbumArt(album, artist);

            if(!current_artwork.isEmpty())
                album_art->putPixmap(current_artwork);
            else  if (!artistHead.isEmpty())
            {
                current_artwork = artistHead;
                album_art->putPixmap(current_artwork);
            }
            else album_art->putDefaultPixmap();

            return true;
        }else
        {
            emit fetchCover(artist,album,title);
            return false;
        }

    }
}

void MainWindow::addToQueue(const QList<QMap<int, QString>> &tracks)
{
    prev_queued_song_pos=current_song_pos;

    QStringList queuedList;
    for(auto track : tracks)
    {
        if(!queued_songs.contains(track[BabeTable::LOCATION]))
        {
            mainList->addRowAt(queued_songs.size(),track,true);
            mainList->item(queued_songs.size(),BabeTable::TITLE)->setIcon(QIcon::fromTheme("clock"));
            mainList->colorizeRow({queued_songs.size()},"#333");
            queued_songs.insert(track[BabeTable::LOCATION],track);
            queuedList<<track[BabeTable::TITLE]+" by "+track[BabeTable::ARTIST];
            current_song_pos++;
        }

    }
    //mainList->addRowAt(current_song_pos+1,track,true);
    qDebug()<<"saving track pos to restore after queued is empty"<<prev_queued_song_pos;

    mainList->scrollToItem(mainList->item(0,BabeTable::TITLE),QAbstractItemView::PositionAtTop);

    nof.notify("Song added to Queue",queuedList.join("\n"));

}

void MainWindow::on_seekBar_sliderMoved(const int &position)
{
    player->setPosition(player->duration() / 1000 * position);
}

QString MainWindow::transformTime(const qint64 &value)
{
    QString tStr;
    if (value)
    {
        QTime time((value/3600)%60, (value/60)%60, value%60, (value*1000)%1000);
        QString format = "mm:ss";
        if (value > 3600)
            format = "hh:mm:ss";
        tStr = time.toString(format);
    }
    return tStr.isEmpty()?"00:00":tStr;
}

void MainWindow::update()
{

    if(mainList->rowCount()==0) stop();

    if(!current_song.isEmpty())
    {
        if(!seekBar->isEnabled()) seekBar->setEnabled(true);

        if(!seekBar->isSliderDown())
            seekBar->setValue(static_cast<int>(static_cast<double>(player->position())/player->duration()*1000));
        ui->time->setText(this->transformTime(player->position()/1000));
        ui->duration->setText(this->transformTime(player->duration()/1000));

        //        QToolTip::showText( seekBar->mapToGlobal( QPoint( 0, 0 ) ),this->transformTime(player->position()/1000) );
        if(player->state() == QMediaPlayer::StoppedState)
        {
            if(!queued_songs.isEmpty()) removeQueuedTrack(current_song,current_song_pos);

            prev_song = current_song;
            qDebug()<<"finished playing song: "<<prev_song[BabeTable::LOCATION];

            emit finishedPlayingSong(prev_song[BabeTable::LOCATION]);
            next();
        }


    }else
    {
        //        qDebug()<<"no song to play";
        seekBar->setValue(0);
        seekBar->setEnabled(false);
    }
}


bool MainWindow::removeQueuedTrack(const QMap<int, QString> &track,const int &pos)
{
    if(queued_songs.contains(track[BabeTable::LOCATION]))
        if(mainList->item(pos,BabeTable::TITLE)->icon().name()=="clock")
        {
            mainList->removeRow(pos);
            queued_songs.remove(track[BabeTable::LOCATION]);
            return true;
        }

    return false;
}

void MainWindow::removequeuedTracks()
{
    QList<QMap<int, QString>> newList;

    for(auto row=0;row<this->mainList->rowCount();row++)

        if(mainList->item(row,BabeTable::TITLE)->icon().name()=="clock")
        {
            mainList->removeRow(row);
            queued_songs.remove(mainList->getRowData(row)[BabeTable::LOCATION]);
            current_song_pos--;
        }

}


void MainWindow::next()
{
    int nextSong=0;

    if(!queued_songs.isEmpty())
    {
        removeQueuedTrack(current_song,current_song_pos);
        nextSong=current_song_pos;

    }else nextSong= current_song_pos+1;

    if(queued_songs.isEmpty() && prev_queued_song_pos!=-1)
    {
        nextSong= prev_queued_song_pos+1;
        prev_queued_song_pos=-1;
    }

    //    if(repeat) nextSong--;

    if(nextSong >= mainList->rowCount()) nextSong = 0;

    if(!queued_songs.isEmpty())
        nextSong=0;

    mainList->setCurrentCell((shuffle&&queued_songs.isEmpty()) ? shuffleNumber():nextSong, BabeTable::TITLE);

    loadTrack();
}


void MainWindow::back()
{
    auto lCounter = current_song_pos-1;

    if(lCounter < 0)
        lCounter = mainList->rowCount() - 1;

    mainList->setCurrentCell(!shuffle ? lCounter : shuffleNumber(), BabeTable::TITLE);

    loadTrack();
}

int MainWindow::shuffleNumber()
{     
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0,mainList->rowCount()-1); // guaranteed unbiased

    auto random_integer = uni(rng);
    qDebug()<<"random number:"<<random_integer;
    if (current_song_pos !=random_integer)return random_integer;
    else return random_integer+1;
}

void MainWindow::on_play_btn_clicked()
{
    if(mainList->rowCount() > 0 || !current_song.isEmpty())
    {
        if(player->state() == QMediaPlayer::PlayingState) this->pause();
        else
        {
            wasPlaying = true;
            this->play();
        }
    }
}

void MainWindow::play()
{
    if(wasPlaying)
    {
        player->play();
        ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));
        this->setWindowTitle(current_song[BabeTable::TITLE]+" \xe2\x99\xa1 "+current_song[BabeTable::ARTIST]);
    }

}

void MainWindow::pause()
{
    wasPlaying = player->state() == QMediaPlayer::PlayingState? true : false;
    player->pause();
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-start"));

}

void MainWindow::stop()
{
    album_art->putPixmap(QString(":/Data/data/empty.svg"));
    ui->controls->setVisible(false);
    album_art->setVisible(false);
    ui->frame_4->setVisible(false);
    seekBar->setVisible(false);
    ui->frame_5->setVisible(false);
    ui->frame_6->setVisible(false);

    current_song.clear();
    prev_song = current_song;
    current_song_pos = 0;
    prev_song_pos =current_song_pos;
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-stop"));

    player->stop();
    updater->stop();

    this->setWindowTitle(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    stopped = true;
}

void MainWindow::on_backward_btn_clicked()
{
    if(mainList->rowCount() > 0)
    {
        if(player->position() > 3000) player->setPosition(0);
        else back();
    }
}

void MainWindow::on_foward_btn_clicked()
{
    if(mainList->rowCount() > 0) next();
}



void MainWindow::collectionDBFinishedAdding()
{
    if(!ui->fav_btn->isEnabled()) ui->fav_btn->setEnabled(true);
    qDebug()<<"now it i time to put the tracks in the table ;)";
    //settings_widget->getCollectionDB().closeConnection();
    albumsTable->flushGrid();
    artistsTable->flushGrid();
    refreshTables();

}


void MainWindow::babedIcon(const bool &state)
{
    auto effect = new QGraphicsColorizeEffect(this);
    effect->setColor(QColor("#dc042c"));

    if(state)  effect->setStrength(1.0);
    else effect->setStrength(0);

    ui->fav_btn->setGraphicsEffect(effect);
}

void MainWindow::on_fav_btn_clicked()
{
    if(!current_song.isEmpty())
    {
        if(!isBabed(current_song))
        {
            if(babeTrack(current_song))  babedIcon(true);
        }else
        {
            if(unbabeIt(current_song)) babedIcon(false);
        }
    }
}

void MainWindow::babeAlbum(const QMap<int, QString> &info)
{
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];

    QList<QMap<int,QString>> mapList;
    if(album.isEmpty())
        mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\""));
    else if(!artist.isEmpty())
        mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\" and album = \""+album+"\""));

    if(!mapList.isEmpty())
        babeIt(mapList);

}

bool MainWindow::unbabeIt(const QMap<int, QString> &track)
{
    if(settings_widget->getCollectionDB().insertInto("tracks","babe",track[BabeTable::LOCATION],0))
    {
        nof.notify("Song unBabe'd it",track[BabeTable::TITLE]+" by "+track[BabeTable::ARTIST]);
        return  true;
    } else return false;

}

bool MainWindow::babeTrack(const QMap<int, QString> &track)
{

    QString url = track[BabeTable::LOCATION];
    if(isBabed(track))
    {
        if(unbabeIt(track)) return true;
        else return false;

    }else
    {

        if(settings_widget->getCollectionDB().check_existance("tracks","location",url))
        {
            if(settings_widget->getCollectionDB().insertInto("tracks","babe",url,1))
            {
                nof.notify("Song Babe'd it",track[BabeTable::TITLE]+" by "+track[BabeTable::ARTIST]);
                addToPlaylist({track},true,APPENDBOTTOM);
                return true;
            } else return false;

        }else
        {
            ui->fav_btn->setEnabled(false);

            addToCollectionDB({url},"1");

            ui->fav_btn->setEnabled(true);
            return true;

        }

    }

}
void MainWindow::babeIt(const QList<QMap<int, QString>> &tracks)
{
    for(auto track : tracks)
        if(!babeTrack(track)) qDebug()<<"couldn't Babe track:"<<track[BabeTable::LOCATION];

}

void  MainWindow::infoIt(const QString &title, const QString &artist, const QString &album)
{
    //views->setCurrentIndex(INFO);
    infoView();
    infoTable->getTrackInfo(title, artist,album);

}

void MainWindow::scanNewDir(const QString &url, const QString &babe)
{
    QStringList list;
    qDebug()<<"scanning new dir: "<<url;
    QDirIterator it(url, settings_widget->formats, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString song = it.next();
        if(!settings_widget->getCollectionDB().check_existance("tracks","location",song))
            list<<song;
    }

    if (!list.isEmpty())
    {
        addToCollectionDB(list,babe);

    }else
    {
        refreshTables();
        settings_widget->refreshWatchFiles(); qDebug()<<"a folder probably got removed or changed";
    }
}


void MainWindow::addToCollectionDB(const QStringList &url, const QString &babe)
{

    settings_widget->getCollectionDB().addTrack(url,babe.toInt());
    if(babe.contains("1"))
        addToPlaylist(settings_widget->getCollectionDB().getTrackData(url),true,APPENDBOTTOM);


}


void MainWindow::addToPlaylist(const QList<QMap<int, QString> > &mapList, const bool &notRepeated, const appendPos &pos)
{
    //currentList.clear();
    qDebug()<<"Adding mapList to mainPlaylist";

    if(!mapList.isEmpty())
    {

        emit ui->filter->textChanged("");
        if(notRepeated)
        {
            QList<QMap<int,QString>> newList;
            QStringList alreadyInList = mainList->getTableColumnContent(BabeTable::LOCATION);
            for(auto track: mapList)
            {
                if(!alreadyInList.contains(track[BabeTable::LOCATION]))
                {
                    newList<<track;
                    switch(pos)
                    {
                    case APPENDBOTTOM:
                        mainList->addRow(track,true);
                        mainList->scrollToBottom();

                        break;
                    case APPENDTOP:
                        mainList->addRowAt(0,track,true);
                        mainList->scrollToItem(mainList->item(0,BabeTable::TITLE),QAbstractItemView::PositionAtTop);

                        break;
                    case APPENDAFTER:
                        mainList->addRowAt(current_song_pos+1,track,true);
                        mainList->scrollToItem(mainList->item(current_song_pos+1,BabeTable::TITLE),QAbstractItemView::PositionAtTop);

                        break;
                    case APPENDBEFORE:
                        mainList->addRowAt(current_song_pos,track,true);
                        mainList->scrollToItem(mainList->item(current_song_pos,BabeTable::TITLE),QAbstractItemView::PositionAtTop);

                        break;
                    case APPENDINDEX:
                        mainList->addRowAt(mainList->getIndex(),track,true);
                        mainList->scrollToItem(mainList->item(mainList->getIndex(),BabeTable::TITLE),QAbstractItemView::PositionAtTop);

                        break;
                    }

                }
            }

            currentList+=newList;


        }else
        {
            for(auto track:mapList)
                switch(pos)
                {
                case APPENDBOTTOM:
                    mainList->addRow(track,true);
                    mainList->scrollToBottom();

                    break;
                case APPENDTOP:
                    mainList->addRowAt(0,track,true);
                    mainList->scrollToItem(mainList->item(0,BabeTable::TITLE),QAbstractItemView::PositionAtTop);

                    break;
                case APPENDAFTER:
                    mainList->addRowAt(current_song_pos+1,track,true);
                    mainList->scrollToItem(mainList->item(current_song_pos+1,BabeTable::TITLE),QAbstractItemView::PositionAtTop);

                    break;
                case APPENDBEFORE:
                    mainList->addRowAt(current_song_pos,track,true);
                    mainList->scrollToItem(mainList->item(current_song_pos,BabeTable::TITLE),QAbstractItemView::PositionAtTop);

                    break;
                case APPENDINDEX:
                    mainList->addRowAt(mainList->getIndex(),track,true);
                    mainList->scrollToItem(mainList->item(mainList->getIndex(),BabeTable::TITLE),QAbstractItemView::PositionAtTop);

                    break;
                }
            currentList+=mapList;

        }

        if(stopped)
        {
            mainList->setCurrentCell(0,BabeTable::TITLE);
            album_art->setVisible(true);
            ui->frame_4->setVisible(true);
            seekBar->setVisible(true);
            ui->frame_5->setVisible(true);
            ui->frame_6->setVisible(true);
            ui->controls->setVisible(true);

            loadTrack();
            stopped=!stopped;
        }
    }
}


void  MainWindow::clearCurrentList()
{
    currentList.clear();
    mainList->flushTable();
}

void MainWindow::on_search_returnPressed()
{

    if(views->currentIndex()!=ALBUMS && views->currentIndex()!=ARTISTS)
    {

        if(resultsTable->rowCount()<1)
        {
            views->setCurrentIndex(prevIndex);
            resultsTable->flushTable();
            // ui->saveResults->setEnabled(false);
        }else views->setCurrentIndex(RESULTS);
    }
}

void MainWindow::on_search_textChanged(const QString &arg1)
{

    if(!ui->search->text().isEmpty())
    {
        QStringList searchList=arg1.split(",");
        auto searchResults = searchFor(searchList);

        if(!searchResults.isEmpty())
        {
            //            ui->playAll->setVisible(true);

            albumsTable->filter(searchResults,BabeTable::ALBUM);
            artistsTable->filter(searchResults,BabeTable::ARTIST);

            populateResultsTable(searchResults);
        }


    }else
    {


        albumsTable->hide_all(false);

        artistsTable->hide_all(false);
        resultsTable->flushTable();


        if(views->currentIndex()!=ALBUMS||views->currentIndex()!=ARTISTS)
        {
            if(prevIndex==RESULTS) views->setCurrentIndex(COLLECTION);
            else  views->setCurrentIndex(prevIndex);


        }
    }


}

void MainWindow::populateResultsTable(const QList<QMap<int, QString> > &mapList)
{
    if(views->currentIndex()!=ALBUMS&&views->currentIndex()!=ARTISTS)
        views->setCurrentIndex(RESULTS);
    resultsTable->flushTable();
    resultsTable->populateTableView(mapList,false);
}

QList<QMap<int, QString> > MainWindow::searchFor(const QStringList &queries)
{
    QList<QMap<int,QString>> mapList;

    for(auto searchQuery : queries)
    {

        QString key;

        for(auto k : this->searchKeys)
            if(searchQuery.contains(k)) { key=k; searchQuery=searchQuery.replace(k,"").trimmed(); }

        searchQuery=searchQuery.trimmed();
        qDebug()<<"Searching for: "<<searchQuery;

        if(!searchQuery.isEmpty())
        {

            if(key == "location:")
                mapList += settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE location LIKE \"%"+searchQuery+"%\""));

            else if(key == "artist:")
                mapList += settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist LIKE \"%"+searchQuery+"%\""));

            else if(key == "album:")
                mapList += settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE album LIKE \"%"+searchQuery+"%\""));

            else if(key == "title:")
                mapList += settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE title LIKE \"%"+searchQuery+"%\""));

            else if(key==  "genre:")
                mapList += settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE genre LIKE \"%"+searchQuery+"%\""));

            //            else if(key==  "online:")
            //                mapList += onlineFetcher->fetch(searchQuery); //to be implemented;

            else if(key==  "playlist:")
                mapList += settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE playlist LIKE \"%"+searchQuery+"%\""));

            else if(key==  "stars:")
                mapList += settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE stars = "+searchQuery));

            else
                mapList += settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE title LIKE \"%"+searchQuery+"%\" OR artist LIKE \"%"+searchQuery+"%\" OR album LIKE \"%"+searchQuery+"%\"OR genre LIKE \"%"+searchQuery+"%\"OR location LIKE \"%"+searchQuery+"%\""));

        }
    }

    return mapList;
}



void MainWindow::on_rowInserted(QModelIndex model ,int x,int y)
{
    Q_UNUSED(model);
    Q_UNUSED(y);
    mainList->scrollTo(mainList->model()->index(x,BabeTable::TITLE),QAbstractItemView::PositionAtCenter);

}


void MainWindow::clearMainList()
{
    //    this->album_art->putDefaultPixmap();

    //    this->current_song.clear();



    QList<QMap<int,QString>> mapList;
    if (!current_song.isEmpty()) mapList<<current_song;
    for(auto row : mainList->getSelectedRows(false))
        mapList<<mainList->getRowData(row);


    this->currentList.clear();
    this->mainList->flushTable();
    this->addToPlaylist(mapList,true,APPENDBOTTOM);
    mainList->removeRepeated();

    this->current_song_pos=0;
    this->prev_song_pos=current_song_pos;

    //    this->player->stop();
}

void MainWindow::on_tracks_view_2_clicked()
{
    expand();
}

void MainWindow::on_addAll_clicked()
{

    switch(views->currentIndex())
    {
    case COLLECTION:
        addToPlaylist(collectionTable->getAllTableContent(),false, APPENDBOTTOM); break;
    case ALBUMS:
        addToPlaylist(albumsTable->albumTable->getAllTableContent(),false, APPENDBOTTOM); break;
    case ARTISTS:
        addToPlaylist(artistsTable->albumTable->getAllTableContent(),false,APPENDBOTTOM); break;
    case PLAYLISTS:
        addToPlaylist(playlistTable->table->getAllTableContent(),false,APPENDBOTTOM); break;
    case RABBIT:
        addToPlaylist(rabbitTable->getTable()->getAllTableContent(),false, APPENDBOTTOM); break;
    case RESULTS:
        addToPlaylist(resultsTable->getAllTableContent(),false, APPENDBOTTOM); break;
    }

}


void MainWindow::saveResultsTo()
{
    switch(views->currentIndex())
    {
    case COLLECTION: emit collectionTable->saveToPlaylist(collectionTable->getAllTableContent()); break;
    case ALBUMS: emit albumsTable->albumTable->saveToPlaylist(albumsTable->albumTable->getAllTableContent()); break;
    case ARTISTS: emit artistsTable->albumTable->saveToPlaylist(artistsTable->albumTable->getAllTableContent()); break;
    case PLAYLISTS: emit playlistTable->table->saveToPlaylist(playlistTable->table->getAllTableContent()); break;
    case RESULTS: emit resultsTable->saveToPlaylist(resultsTable->getAllTableContent()); break;

    }
}

void MainWindow::on_filterBtn_clicked()
{
    if(ui->filterBtn->isChecked())
    {
        ui->filterBtn->setChecked(true);
        ui->filterBox->setVisible(true);
        ui->calibrateBtn->setVisible(false);
        ui->playlists_view->setVisible(false);
        if(ui->tracks_view_2->isVisible()) ui->tracks_view_2->setVisible(false);
        mainListView->setCurrentIndex(FILTERLIST);
        ui->filter->setFocus();
    }else
    {
        ui->filterBtn->setChecked(false);
        ui->filterBox->setVisible(false);
        ui->calibrateBtn->setVisible(true);
        ui->playlists_view->setVisible(true);
        if(!ui->tracks_view_2->isVisible() && viewMode==PLAYLISTMODE)ui->tracks_view_2->setVisible(true);
        mainListView->setCurrentIndex(MAINPLAYLIST);

    }

}

void MainWindow::on_filter_textChanged(const QString &arg1)
{

    QString query = arg1;
    if(!query.isEmpty())
    {
        mainListView->setCurrentIndex(FILTERLIST);

        QStringList searchList=query.split(",");

        auto searchResults = searchFor(searchList);

        if(!searchResults.isEmpty())
        {
            filterList->flushTable();
            filterList->populateTableView(searchResults,true);
        }else  filterList->flushTable();


    }else
    {
        filterList->flushTable();
        ui->filterBtn->setChecked(false);
        emit  ui->filterBtn->clicked();
        ui->filterBox->setVisible(false);
        mainListView->setCurrentIndex(MAINPLAYLIST);
    }



}

void MainWindow::calibrateMainList()
{
    clearMainList();
    populateMainList();
    currentList = mainList->getAllTableContent();
    mainList->scrollToTop();

    if(mainList->rowCount()>0)
    {
        this->mainList->setCurrentCell(current_song_pos,BabeTable::TITLE);
        this->mainList->item(current_song_pos,BabeTable::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));
        this->mainList->removeRepeated();
    }
}

void MainWindow::on_playAll_clicked()
{

    switch(views->currentIndex())
    {
    case COLLECTION:
        putOnPlay(collectionTable->getAllTableContent()); break;
    case ALBUMS:
        putOnPlay(albumsTable->albumTable->getAllTableContent()); break;
    case ARTISTS:
        putOnPlay(artistsTable->albumTable->getAllTableContent()); break;
    case PLAYLISTS:
        putOnPlay(playlistTable->table->getAllTableContent()); break;
    case RABBIT:
        putOnPlay(rabbitTable->getTable()->getAllTableContent()); break;
    case RESULTS:
        putOnPlay(resultsTable->getAllTableContent()); break;
    }
}

void MainWindow::on_miniPlaybackBtn_clicked()
{
    auto controlsColor = this->palette().color(QPalette::Window);

    if(!miniPlayback)
    {
        if(!ui->miniPlaybackBtn->isVisible()) ui->miniPlaybackBtn->setVisible(true);
        ui->controls->setFixedHeight(ui->controls->minimumSizeHint().height()-40);
        album_art->setFixedHeight(ui->controls->minimumSizeHint().height()-40);
        ui->miniPlaybackBtn->setIcon(QIcon::fromTheme("go-bottom"));
        miniPlayback=!miniPlayback;
        this->blurWidget(*album_art,28);
        album_art->saturatePixmap(100);

    }else
    {
        ui->controls->setFixedHeight(ALBUM_SIZE);
        album_art->setFixedHeight(ALBUM_SIZE);
        ui->miniPlaybackBtn->setIcon(QIcon::fromTheme("go-top"));
        miniPlayback=!miniPlayback;
        this->blurWidget(*album_art,15);
        album_art->restoreSaturation();

    }

    ui->controls->update();
}


void MainWindow::blurWidget(Album &widget, const int &radius )
{
    QGraphicsBlurEffect* effect	= new QGraphicsBlurEffect();

    effect->setBlurRadius(radius);

    widget.setGraphicsEffect(effect);

}
