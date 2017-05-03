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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    this->setAcceptDrops(true);
    this->setWindowIcon(QIcon(":Data/data/babe_48.svg"));
    this->setWindowIconText("Babe...");
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    defaultWindowFlags = this->windowFlags();
    //mpris = new Mpris(this);

    connect(this, &MainWindow::finishedPlayingSong, this, &MainWindow::addToPlayed);
    connect(this,&MainWindow::fetchCover,this,&MainWindow::setCoverArt);
    connect(this,&MainWindow::collectionChecked,this,&MainWindow::refreshTables);

    //* SETUP BABE PARTS *//
    this->setUpViews();
    this->setUpSidebar();
    this->setUpPlaylist();
    this->setUpRightFrame();
    this->setUpCollectionViewer();
    this->setUpActions();

    //* CHECK FOR DATABASE *//
    if(settings_widget->checkCollection())
    {
        settings_widget->getCollectionDB().setCollectionLists();
        populateMainList();
        emit collectionChecked();

    } else settings_widget->createCollectionDB();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]()
    {
        timer->stop();
        feedRabbit();
        infoTable->getTrackInfo(current_song[BabeTable::TITLE],current_song[BabeTable::ARTIST],current_song[BabeTable::ALBUM]);

    });

    connect(&nof,&Notify::babeSong,this,&MainWindow::babeIt);
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
    setToolbarIconSize(settings_widget->getToolbarIconSize());
    mainList->setCurrentCell(0,BabeTable::TITLE);

    if(mainList->rowCount()>0)
    {
        loadTrack();
        collectionView();
        go_playlistMode();

    }else collectionView();

    updater->start(1000);
}


MainWindow::~MainWindow()
{
    delete ui;
}


//*HERE THE MAIN VIEWS GET SETUP WITH THEIR SIGNALS AND SLOTS**//
void MainWindow::setUpViews()
{
    settings_widget = new settings(this); //this needs to go first

    playlistTable = new PlaylistsView(this);
    connect(playlistTable,SIGNAL(playlistCreated(QString, QString)),&settings_widget->getCollectionDB(),SLOT(insertPlaylist(QString, QString)));
    connect(playlistTable->table,SIGNAL(tableWidget_doubleClicked(QList<QMap<int, QString>>)),this,SLOT(addToPlaylist(QList<QMap<int, QString>>)));
    connect(playlistTable->table,SIGNAL(removeIt_clicked(int)),this,SLOT(removeSong(int)));
    connect(playlistTable->table,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    //connect(playlistTable->table,SIGNAL(createPlaylist_clicked()),this,SLOT(playlistsView()));
    connect(playlistTable->table,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(playlistTable->table,SIGNAL(moodIt_clicked(QString)),playlistTable,SLOT(createMoodPlaylist(QString)));
    connect(playlistTable->table,SIGNAL(infoIt_clicked(QString, QString, QString)),this,SLOT(infoIt(QString, QString, QString)));

    collectionTable = new BabeTable(this);
    //connect(collectionTable, &BabeTable::tableWidget_doubleClicked, this, &MainWindow::addToPlaylist);
    connect(collectionTable,SIGNAL(tableWidget_doubleClicked(QList<QMap<int, QString>>)),this,SLOT(addToPlaylist(QList<QMap<int, QString>>)));
    connect(collectionTable,SIGNAL(enteredTable()),this,SLOT(hideControls()));
    connect(collectionTable,SIGNAL(leftTable()),this,SLOT(showControls()));
    connect(collectionTable,SIGNAL(finishedPopulating()),this,SLOT(orderTables()));
    connect(collectionTable,SIGNAL(removeIt_clicked(int)),this,SLOT(removeSong(int)));
    connect(collectionTable,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(collectionTable,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(collectionTable,SIGNAL(moodIt_clicked(QString)),playlistTable,SLOT(createMoodPlaylist(QString)));
    connect(collectionTable,SIGNAL(infoIt_clicked(QString, QString, QString)),this,SLOT(infoIt(QString, QString, QString)));

    mainList = new BabeTable(this);
    mainList->hideColumn(BabeTable::ALBUM);
    mainList->hideColumn(BabeTable::ARTIST);
    mainList->horizontalHeader()->setVisible(false);
    //mainList->setFixedWidth(200);
    mainList->setMaximumWidth(200);
    mainList->setMinimumHeight(200);
    mainList->setAddMusicMsg("\nDrag and drop music here!");
    connect(mainList, &BabeTable::tableWidget_doubleClicked, this, &MainWindow::on_mainList_clicked);
    //connect(mainList,SIGNAL(tableWidget_doubleClicked(QList<QMap<int, QString>>)),this,SLOT(on_mainList_clicked(QList<QMap<int, QString>>)));
    connect(mainList, &BabeTable::removeIt_clicked, this, &MainWindow::removeSong);
    //connect(mainList,SIGNAL(removeIt_clicked(int)),this,SLOT(removeSong(int)));
    connect(mainList,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(mainList,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(mainList,SIGNAL(moodIt_clicked(QString)),playlistTable,SLOT(createMoodPlaylist(QString)));
    connect(mainList,SIGNAL(infoIt_clicked(QString, QString, QString)),this,SLOT(infoIt(QString, QString, QString)));
    connect(mainList->model() ,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(on_rowInserted(QModelIndex,int,int)));

    resultsTable=new BabeTable(this);
    resultsTable->passStyle("QHeaderView::section { background-color:#575757; color:white; }");
    resultsTable->setVisibleColumn(BabeTable::STARS);
    resultsTable->showColumn(BabeTable::GENRE);
    connect(resultsTable,SIGNAL(tableWidget_doubleClicked(QList<QMap<int, QString>>)),this,SLOT(addToPlaylist(QList<QMap<int, QString>>)));
    connect(resultsTable,SIGNAL(enteredTable()),this,SLOT(hideControls()));
    connect(resultsTable,SIGNAL(leftTable()),this,SLOT(showControls()));
    connect(resultsTable,SIGNAL(removeIt_clicked(int)),this,SLOT(removeSong(int)));
    connect(resultsTable,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(resultsTable,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(resultsTable,SIGNAL(moodIt_clicked(QString)),playlistTable,SLOT(createMoodPlaylist(QString)));
    connect(resultsTable,SIGNAL(infoIt_clicked(QString, QString, QString)),this,SLOT(infoIt(QString, QString, QString)));

    rabbitTable = new RabbitView(this);
    connect(rabbitTable,&RabbitView::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(rabbitTable->getTable(),SIGNAL(tableWidget_doubleClicked(QList<QMap<int, QString>>)),this,SLOT(addToPlaylist(QList<QMap<int, QString>>)));
    connect(rabbitTable->getTable(),&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(rabbitTable->getTable(),&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(rabbitTable->getTable(),&BabeTable::moodIt_clicked,playlistTable,&PlaylistsView::createMoodPlaylist);
    connect(rabbitTable->getTable(),&BabeTable::infoIt_clicked,this,&MainWindow::infoIt);

    albumsTable = new AlbumsView(false,this);
    connect(albumsTable,SIGNAL(albumOrderChanged(QString)),this,SLOT(AlbumsViewOrder(QString)));
    connect(albumsTable->albumTable,SIGNAL(tableWidget_doubleClicked(QList<QMap<int, QString>>)),this,SLOT(addToPlaylist(QList<QMap<int, QString>>)));
    connect(albumsTable->albumTable,SIGNAL(removeIt_clicked(int)),this,SLOT(removeSong(int)));
    connect(albumsTable->albumTable,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(albumsTable->albumTable,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(albumsTable->albumTable,SIGNAL(moodIt_clicked(QString)),playlistTable,SLOT(createMoodPlaylist(QString)));
    connect(albumsTable->albumTable,SIGNAL(infoIt_clicked(QString, QString, QString)),this,SLOT(infoIt(QString, QString, QString)));
    connect(albumsTable,&AlbumsView::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(albumsTable,&AlbumsView::babeAlbum_clicked,this,&MainWindow::babeAlbum);
    connect(albumsTable,&AlbumsView::albumDoubleClicked,this,&MainWindow::albumDoubleClicked);

    artistsTable = new AlbumsView(true,this);
    artistsTable->order->setVisible(false);
    artistsTable->albumTable->showColumn(BabeTable::ALBUM);
    connect(artistsTable->albumTable,SIGNAL(tableWidget_doubleClicked(QList<QMap<int, QString>>)),this,SLOT(addToPlaylist(QList<QMap<int, QString>>)));
    connect(artistsTable->albumTable,SIGNAL(removeIt_clicked(int)),this,SLOT(removeSong(int)));
    connect(artistsTable->albumTable,&BabeTable::babeIt_clicked,this,&MainWindow::babeIt);
    connect(artistsTable->albumTable,&BabeTable::queueIt_clicked,this,&MainWindow::addToQueue);
    connect(artistsTable->albumTable,SIGNAL(moodIt_clicked(QString)),playlistTable,SLOT(createMoodPlaylist(QString)));
    connect(artistsTable->albumTable,SIGNAL(infoIt_clicked(QString, QString, QString)),this,SLOT(infoIt(QString, QString, QString)));
    connect(artistsTable,&AlbumsView::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(artistsTable,&AlbumsView::babeAlbum_clicked,this,&MainWindow::babeAlbum);
    connect(artistsTable,&AlbumsView::albumDoubleClicked,this,&MainWindow::albumDoubleClicked);

    infoTable = new InfoView(this);
    connect(infoTable,&InfoView::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(infoTable,&InfoView::similarBtnClicked,[this](QStringList queries) { this->ui->search->setText(queries.join(",")); });
    connect(infoTable,&InfoView::tagsBtnClicked,[this](QStringList queries) { this->ui->search->setText(queries.join(",")); });
    connect(infoTable,&InfoView::tagClicked,[this](QString query) { this->ui->search->setText(query);});
    connect(infoTable,&InfoView::similarArtistTagClicked,[this](QString query) { this->ui->search->setText(query);});

    settings_widget->readSettings();
    connect(settings_widget, SIGNAL(toolbarIconSizeChanged(int)), this, SLOT(setToolbarIconSize(int)));
    connect(settings_widget, SIGNAL(collectionDBFinishedAdding(bool)), this, SLOT(collectionDBFinishedAdding(bool)));
    connect(settings_widget, SIGNAL(dirChanged(QString,QString)),this, SLOT(scanNewDir(QString, QString)));
    //connect(settings_widget, SIGNAL(collectionPathRemoved(QString)),&settings_widget->getCollectionDB(), SLOT(removePath(QString)));
    connect(settings_widget, SIGNAL(refreshTables()),this, SLOT(refreshTables()));

    /* THE BUTTONS VIEWS */
    connect(ui->tracks_view, SIGNAL(clicked()), this, SLOT(collectionView()));
    connect(ui->albums_view, SIGNAL(clicked()), this, SLOT(albumsView()));
    connect(ui->artists_view, SIGNAL(clicked()), this, SLOT(artistsView()));
    connect(ui->playlists_view, SIGNAL(clicked()), this, SLOT(playlistsView()));
    connect(ui->rabbit_view, SIGNAL(clicked()), this, SLOT(rabbitView()));
    connect(ui->info_view, SIGNAL(clicked()), this, SLOT(infoView()));
    connect(ui->settings_view, SIGNAL(clicked()), this, SLOT(settingsView()));

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

void MainWindow::setUpSidebar()
{
    auto *left_spacer = new QWidget(this);
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget(this);
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->mainToolBar->setContentsMargins(0,0,0,0);
    ui->mainToolBar->layout()->setMargin(0);
    ui->mainToolBar->layout()->setSpacing(0);
    ui->mainToolBar->setStyleSheet(QString("QToolBar {margin:0; background-color:rgba( 0, 0, 0, 0); background-image:url('%1');} QToolButton{ border-radius:0;}"
                                           " QToolButton:checked{border-radius:0; background: %2}").arg(":Data/data/pattern.png",this->palette().color(QPalette::Highlight).name()));

    //ui->mainToolBar->setStyleSheet(QString("QToolBar{margin:0 background-image:url('%1') repeat; }QToolButton{ border-radius:0;} QToolButton:checked{border-radius:0; background: rgba(0,0,0,50)}").arg(":Data/data/pattern.png"));
    ui->mainToolBar->setOrientation(Qt::Vertical);
    ui->mainToolBar->addWidget(left_spacer);

    ui->tracks_view->setToolTip("Collection");
    ui->mainToolBar->addWidget(ui->tracks_view);

    ui->albums_view->setToolTip("Albums");
    ui->mainToolBar->addWidget(ui->albums_view);

    ui->artists_view->setToolTip("Artists");
    ui->mainToolBar->addWidget(ui->artists_view);

    ui->playlists_view->setToolTip("Playlists");
    ui->mainToolBar->addWidget(ui->playlists_view);

    ui->rabbit_view->setToolTip("Rabbit");
    ui->mainToolBar->addWidget(ui->rabbit_view);

    ui->info_view->setToolTip("Info");
    ui->mainToolBar->addWidget(ui->info_view);

    ui->settings_view->setToolTip("Setings");
    ui->mainToolBar->addWidget(ui->settings_view);

    ui->mainToolBar->addWidget(right_spacer);

}

void MainWindow::setUpCollectionViewer()
{
    mainLayout = new QGridLayout();

    leftFrame_layout = new QGridLayout();
    leftFrame_layout->setContentsMargins(0,0,0,0);
    leftFrame_layout->setSpacing(0);

    leftFrame = new QFrame(this);
    leftFrame->setFrameShape(QFrame::StyledPanel);
    leftFrame->setFrameShadow(QFrame::Raised);
    leftFrame->setLayout(leftFrame_layout);

    line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setMaximumHeight(1);

    lineV = new QFrame(this);
    lineV->setFrameShape(QFrame::VLine);
    lineV->setFrameShadow(QFrame::Plain);
    lineV->setMaximumWidth(1);

    utilsBar = new QToolBar(this);
    utilsBar->setMovable(false);
    utilsBar->setContentsMargins(0,0,0,0);
    utilsBar->setStyleSheet("margin:0;");

    utilsBar->addWidget(infoTable->infoUtils);
    utilsBar->addWidget(playlistTable->btnContainer);
    utilsBar->addWidget(ui->searchWidget);
    utilsBar->addWidget(albumsTable->utilsFrame);
    utilsBar->addWidget(artistsTable->utilsFrame);
    utilsBar->addWidget(ui->collectionUtils);

    utilsBar->actions().at(PLAYLISTS_UB)->setVisible(false); ui->frame_3->setVisible(false);
    utilsBar->actions().at(INFO_UB)->setVisible(false);
    utilsBar->actions().at(ALBUMS_UB)->setVisible(false);
    utilsBar->actions().at(ARTISTS_UB)->setVisible(false);


    ui->search->setClearButtonEnabled(true);
    ui->search->setPlaceholderText("Search...");

    saveResults_menu = new QMenu(this);
    connect(saveResults_menu, SIGNAL(triggered(QAction*)), this, SLOT(saveResultsTo(QAction*)));
    ui->saveResults->setMenu(saveResults_menu);
    ui->saveResults->setStyleSheet("QToolButton::menu-indicator { image: none; }");

    leftFrame_layout->addWidget(ui->mainToolBar,0,0,3,1,Qt::AlignLeft);
    leftFrame_layout->addWidget(lineV,0,1,3,1,Qt::AlignLeft);
    leftFrame_layout->addWidget(views,0,2);
    leftFrame_layout->addWidget(line,1,2);
    leftFrame_layout->addWidget(utilsBar,2,2);

    mainLayout->addWidget(leftFrame, 0,0);
    mainLayout->addWidget(rightFrame,0,1, Qt::AlignRight);

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

    album_art = new Album(":Data/data/babe.png",200,0,false);
    connect(album_art,&Album::playAlbum,this,&MainWindow::putAlbumOnPlay);
    connect(album_art,&Album::changedArt,this,&MainWindow::changedArt);
    connect(album_art,&Album::babeAlbum_clicked,this,&MainWindow::babeAlbum);

    album_art->setFixedSize(200,200);
    // album_art->setFixedHeight(200);
    // album_art->setMaximumWidth(200);
    album_art->setTitleGeometry(0,0,200,30);
    album_art->titleVisible(false);

    ui->controls->setParent(album_art);
    ui->controls->setGeometry(0,200-50,200,50);

    seekBar = new QSlider(this);
    connect(seekBar,SIGNAL(sliderMoved(int)),this,SLOT(on_seekBar_sliderMoved(int)));

    seekBar->setMaximum(1000);
    seekBar->setOrientation(Qt::Horizontal);
    seekBar->setContentsMargins(0,0,0,0);
    seekBar->setFixedHeight(5);
    seekBar->setStyleSheet(QString("QSlider { background:transparent;} QSlider::groove:horizontal {border: none; background: transparent; height: 5px; border-radius: 0; } QSlider::sub-page:horizontal { background: %1;border: none; height: 5px;border-radius: 0;} QSlider::add-page:horizontal {background: transparent; border: none; height: 5px; border-radius: 0; } QSlider::handle:horizontal {background: %1; width: 8px; } QSlider::handle:horizontal:hover {background: qlineargradient(x1:0, y1:0, x2:1, y2:1,stop:0 #fff, stop:1 #ddd);border: 1px solid #444;border-radius: 4px;}QSlider::sub-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::add-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::handle:horizontal:disabled {background: transparent;border: 1px solid #aaa;border-radius: 4px;}").arg(this->palette().color(QPalette::Highlight).name()));

    /* addMusicImg = new QLabel();
    addMusicImg->setPixmap(QPixmap(":Data/data/add.png").scaled(120,120,Qt::KeepAspectRatio));
    // addMusicImg->setGeometry(45,40,120,120);
    addMusicImg->setVisible(false);

    int id = QFontDatabase::addApplicationFont(":Data/data/LilyScriptOne-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);*/

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
    connect(clearIt, &QAction::triggered, [this]() { clearMainList(); });
    calibrateBtn_menu->addAction(clearIt);

    auto cleanIt = new QAction("Clean...");
    connect(cleanIt, &QAction::triggered, [this]() { mainList->removeRepeated(); });
    calibrateBtn_menu->addAction(cleanIt);

    auto saveIt = new QAction("Save as playlist...");
    calibrateBtn_menu->addAction(saveIt);

    auto changeIt = new QAction("Change Playlist...");
    calibrateBtn_menu->addAction(changeIt);

    playlistWidget_layout->addWidget(album_art, 0,0,Qt::AlignTop);
    playlistWidget_layout->addWidget(ui->frame_6,1,0);
    playlistWidget_layout->addWidget(seekBar,2,0);
    playlistWidget_layout->addWidget(ui->frame_4,3,0);
    playlistWidget_layout->addWidget(mainList,4,0);
    playlistWidget_layout->addWidget(ui->frame_5,5,0);
    playlistWidget_layout->addWidget(ui->playlistUtils,6,0);

}


void MainWindow::setUpRightFrame()
{
    auto *rightFrame_layout = new QGridLayout();
    rightFrame_layout->setContentsMargins(0,0,0,0);
    rightFrame_layout->setSpacing(0);

    rightFrame = new QFrame(this);
    rightFrame->setLayout(rightFrame_layout);
    rightFrame->setFrameShadow(QFrame::Raised);
    rightFrame->setFrameShape(QFrame::StyledPanel);
    rightFrame->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);

    rightFrame_layout->addWidget(playlistWidget,0,0);
}

void MainWindow::setUpActions()
{

}

void MainWindow::changedArt(QMap<int,QString> info)
{
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];
    QString path = info[Album::ART];
    settings_widget->getCollectionDB().execQuery(QString("UPDATE albums SET art = \"%1\" WHERE title = \"%2\" AND artist = \"%3\"").arg(path,album,artist) );
}

void MainWindow::albumDoubleClicked(QMap<int, QString> info)
{
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];

    QList<QMap<int,QString>> mapList;

    if(album.isEmpty())
        mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\" ORDER by album asc, track asc"));
    else if(!album.isEmpty()&&!artist.isEmpty())
        mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\" AND album = \""+album+"\" ORDER by track asc"));

    if(!mapList.isEmpty()) addToPlaylist(mapList);

}

void MainWindow::putAlbumOnPlay(QMap<int,QString> info)
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

        if(!mapList.isEmpty())
            this->putOnPlay(mapList);

    }

}

void MainWindow::putOnPlay(const QList<QMap<int,QString>> &mapList)
{
    if(!mapList.isEmpty())
    {
        mainList->flushTable();
        currentList.clear();

        addToPlaylist(mapList);

        if(mainList->rowCount()>0)
        {
            mainList->setCurrentCell(0,BabeTable::TITLE);
            lCounter=0;
            loadTrack();
        }
    }
}

void MainWindow::addToPlayed(QString url)
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

void MainWindow::resizeEvent(QResizeEvent* event)
{
    if(this->viewMode == FULLMODE && event->size().width() < this->minimumSize().width()+20)
        go_playlistMode();

    QMainWindow::resizeEvent(event);
}

void MainWindow::refreshTables() //tofix
{
    collectionTable->flushTable();
    collectionTable->populateTableView("SELECT * FROM tracks");
    // favoritesTable->flushTable();
    //favoritesTable->populateTableView("SELECT * FROM tracks WHERE stars > \"0\" OR babe =  \"1\"");
    //albumsTable->flushGrid();
    albumsTable->populateTableView(settings_widget->getCollectionDB().getQuery("SELECT * FROM albums ORDER by title asc"));
    albumsTable->hideAlbumFrame();
    //artistsTable->flushGrid();
    artistsTable->populateTableViewHeads(settings_widget->getCollectionDB().getQuery("SELECT * FROM artists ORDER by title asc"));
    artistsTable->hideAlbumFrame();

    playlistTable->list->clear();
    playlistTable->setDefaultPlaylists();
    QStringList playLists =settings_widget->getCollectionDB().getPlaylists();
    playlistTable->definePlaylists(playLists);
    playlistTable->setPlaylists(playLists);
    QStringList playListsMoods =settings_widget->getCollectionDB().getPlaylistsMoods();
    playlistTable->defineMoods(playListsMoods);
    playlistTable->setPlaylistsMoods(playListsMoods);
}

void MainWindow::AlbumsViewOrder(QString order)
{
    albumsTable->flushGrid();
    albumsTable->populateTableView(settings_widget->getCollectionDB().getQuery("SELECT * FROM albums ORDER by "+order.toLower()+" asc"));
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
    showControls();
}

void MainWindow::leaveEvent(QEvent *event)
{
    event->accept();
    hideControls();
}

void MainWindow::hideControls() { ui->controls->setVisible(false); }

void MainWindow::showControls() { ui->controls->setVisible(true); }

void MainWindow::dragEnterEvent(QDragEnterEvent *event) { event->accept(); }

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event) { event->accept(); }

void MainWindow::dragMoveEvent(QDragMoveEvent *event) { event->accept(); }

void MainWindow::dropEvent(QDropEvent *event)
{
    event->accept();
    QList<QMap<int, QString>> mapList;
    QList<QUrl> urls = event->mimeData()->urls();
    qDebug()<< urls;

    if(urls.isEmpty())
    {
        auto info = event->mimeData()->text();
        auto infoList = info.split("/by/");

        if(infoList.size()==2)
        {
            //qDebug()<<"album: " << infoList.at(0) << "artist: "<< infoList.at(1);
            QString _artist = infoList.at(1).simplified();
            QString _album = infoList.at(0).simplified();
            mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+_artist+"\" and album = \""+_album+"\" ORDER by track asc "));

        }else
            mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+info+"\" ORDER by album asc, track asc "));

        addToPlaylist(mapList);

    }else
    {
        QList<QUrl> urls = event->mimeData()->urls();

        QStringList trackList;
        for( auto url  : urls)
        {
            if(QFileInfo(url.path()).isDir())
            {
                QDirIterator it(url.path(), settings_widget->formats, QDir::Files, QDirIterator::Subdirectories);

                while (it.hasNext()) trackList<<it.next();

            }else if (QFileInfo(url.path()).isFile())
                trackList<<url.path();
        }

        auto tracks = new Playlist();
        tracks->add(trackList);
        addToPlaylist(tracks->getTracksData());
    }
}

void MainWindow::dummy() { qDebug()<<"TEST on DUMMYT"; }

void MainWindow::setCoverArt(const QString &artist, const QString &album,const QString &title)
{
    qDebug()<<"Trying to retieve the cover art from Pulpo for"<< title << artist << album;
    Pulpo coverArt(title,artist,album);
    connect(&coverArt,&Pulpo::albumArtReady,this,&MainWindow::putPixmap);
    coverArt.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::Spotify);
}

void MainWindow::putPixmap(const QByteArray &array)
{
    if(!array.isEmpty()) album_art->putPixmap(array);
    else  album_art->putDefaultPixmap();
}

void MainWindow::setToolbarIconSize(const int &iconSize) //tofix
{
    qDebug()<< "Toolbar icons size changed";
    ui->mainToolBar->setIconSize(QSize(iconSize,iconSize));
    //playback->setIconSize(QSize(iconSize,iconSize));
    //utilsBar->setIconSize(QSize(iconSize,iconSize));
    ui->mainToolBar->update();
    //this->utilsBar->setIconSize(QSize(iconSize,iconSize));
    /*for (auto obj : ui->collectionUtils->children())
    {
        if(static_cast<QToolButton *>(obj)!=0)
        {
            //static_cast<QToolButton *>(obj)->setIconSize(QSize(iconSize,iconSize));
        }
    }*/
    //playback->update();
    // this->update();
}

void MainWindow::collectionView()
{
    views->setCurrentIndex(COLLECTION);

    if(this->viewMode != FULLMODE) expand();

    utilsBar->actions().at(ALBUMS_UB)->setVisible(false);
    utilsBar->actions().at(ARTISTS_UB)->setVisible(false);
    utilsBar->actions().at(COLLECTION_UB)->setVisible(true);
    utilsBar->actions().at(PLAYLISTS_UB)->setVisible(false); ui->frame_3->setVisible(false);
    utilsBar->actions().at(INFO_UB)->setVisible(false);

    ui->tracks_view->setChecked(true);
    prevIndex=views->currentIndex();
}

void MainWindow::albumsView()
{
    views->setCurrentIndex(ALBUMS);

    if(this->viewMode != FULLMODE) expand();

    utilsBar->actions().at(ALBUMS_UB)->setVisible(true);
    utilsBar->actions().at(ARTISTS_UB)->setVisible(false);
    utilsBar->actions().at(COLLECTION_UB)->setVisible(true);
    utilsBar->actions().at(PLAYLISTS_UB)->setVisible(false); ui->frame_3->setVisible(false);
    utilsBar->actions().at(INFO_UB)->setVisible(false);

    prevIndex = views->currentIndex();
}

void MainWindow::playlistsView()
{
    views->setCurrentIndex(PLAYLISTS);

    // playlistTable->list->itemClicked(playlistTable->list->indexAt();

    if(this->viewMode != FULLMODE) expand();

    utilsBar->actions().at(ALBUMS_UB)->setVisible(false);
    utilsBar->actions().at(ARTISTS_UB)->setVisible(false);
    utilsBar->actions().at(COLLECTION_UB)->setVisible(true);
    utilsBar->actions().at(PLAYLISTS_UB)->setVisible(true); ui->frame_3->setVisible(true);
    utilsBar->actions().at(INFO_UB)->setVisible(false);

    prevIndex = views->currentIndex();
}

void MainWindow::rabbitView()
{
    views->setCurrentIndex(RABBIT);

    if(this->viewMode != FULLMODE) expand();

    prevSize = this->size();

    utilsBar->actions().at(ALBUMS_UB)->setVisible(false);
    utilsBar->actions().at(ARTISTS_UB)->setVisible(false);
    utilsBar->actions().at(PLAYLISTS_UB)->setVisible(false); ui->frame_3->setVisible(false);
    utilsBar->actions().at(INFO_UB)->setVisible(false);

    prevIndex = views->currentIndex();
}

void MainWindow::infoView()
{
    views->setCurrentIndex(INFO);

    if(this->viewMode != FULLMODE) expand();

    utilsBar->actions().at(ALBUMS_UB)->setVisible(false);
    utilsBar->actions().at(ARTISTS_UB)->setVisible(false);
    utilsBar->actions().at(COLLECTION_UB)->setVisible(true);
    utilsBar->actions().at(PLAYLISTS_UB)->setVisible(false);
    utilsBar->actions().at(INFO_UB)->setVisible(true); ui->frame_3->setVisible(true);

    prevIndex = views->currentIndex();
}

void MainWindow::artistsView()
{
    views->setCurrentIndex(ARTISTS);

    if(this->viewMode != FULLMODE) expand();

    utilsBar->actions().at(ALBUMS_UB)->setVisible(false);
    utilsBar->actions().at(ARTISTS_UB)->setVisible(true);
    utilsBar->actions().at(COLLECTION_UB)->setVisible(true);
    utilsBar->actions().at(PLAYLISTS_UB)->setVisible(false); ui->frame_3->hide();
    utilsBar->actions().at(INFO_UB)->setVisible(false);

    prevIndex = views->currentIndex();
}


void MainWindow::settingsView()
{
    views->setCurrentIndex(SETTINGS);

    if(this->viewMode != FULLMODE) expand();

    utilsBar->actions().at(ALBUMS_UB)->setVisible(false);
    utilsBar->actions().at(ARTISTS_UB)->setVisible(false);
    utilsBar->actions().at(COLLECTION_UB)->setVisible(true);
    utilsBar->actions().at(PLAYLISTS_UB)->setVisible(false); ui->frame_3->setVisible(false);
    utilsBar->actions().at(INFO_UB)->setVisible(false);

    prevIndex = views->currentIndex();
}

void MainWindow::expand()
{
    ui->tracks_view_2->setVisible(false);
    if(!leftFrame->isVisible()) leftFrame->setVisible(true);
    if(!ui->frame_4->isVisible()) ui->frame_4->setVisible(true);
    if(!mainList->isVisible()) mainList->setVisible(true);
    if(!ui->frame_5->isVisible()) ui->frame_5->setVisible(true);
    if(!ui->playlistUtils->isVisible()) ui->playlistUtils->setVisible(true);

    album_art->borderColor=false;

    rightFrame->setFrameShadow(QFrame::Raised);
    rightFrame->setFrameShape(QFrame::StyledPanel);
    mainLayout->setContentsMargins(6,0,6,0);

    this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    this->setMinimumSize(0,0);

    this->resize(700,500);
    /*this->setWindowFlags(defaultWindowFlags);
    this->show();*/

    ui->hide_sidebar_btn->setToolTip("Go Mini");
    ui->hide_sidebar_btn->setIcon(QIcon(":Data/data/mini_mode.svg"));

    this->viewMode=FULLMODE;
}

void MainWindow::go_mini()
{

    leftFrame->setVisible(false);
    ui->frame_4->setVisible(false);
    mainList->setVisible(false);
    ui->frame_5->setVisible(false);
    ui->playlistUtils->setVisible(false);

    //album_art->borderColor=true;

    rightFrame->setFrameShadow(QFrame::Plain);
    rightFrame->setFrameShape(QFrame::NoFrame);

    this->resize(200,200);
    this->setFixedSize(200,200);
    mainLayout->setContentsMargins(0,0,0,0);

    /*this->setWindowFlags(this->windowFlags() | Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->show();*/
    //this->updateGeometry();
    //this->setfix(minimumSizeHint());
    //this->adjustSize();
    ui->hide_sidebar_btn->setToolTip("Expand");
    ui->hide_sidebar_btn->setIcon(QIcon(":Data/data/full_mode.svg"));
    this->viewMode=MINIMODE;
}

void MainWindow::go_playlistMode()
{

    QString icon;
    switch(prevIndex)
    {
    case COLLECTION: icon="filename-filetype-amarok"; break;
    case ALBUMS:  icon="media-album-track"; break;
    case ARTISTS:  icon="amarok_artist"; break;
    case PLAYLISTS:  icon="amarok_lyrics"; break;
    case RABBIT:  icon="kstars_constellationart"; break;
    case INFO: icon="internet-amarok"; break;

    case SETTINGS:  icon="games-config-options"; break;
    default:  icon="search";
    }
    ui->tracks_view_2->setIcon(QIcon::fromTheme(icon));

    leftFrame->setVisible(false);
    if(!ui->frame_4->isVisible()) ui->frame_4->setVisible(true);
    if(!mainList->isVisible()) mainList->setVisible(true);
    if(!ui->frame_5->isVisible()) ui->frame_5->setVisible(true);
    if(!ui->playlistUtils->isVisible()) ui->playlistUtils->setVisible(true);
    ui->tracks_view_2->setVisible(true);

    album_art->borderColor=false;

    rightFrame->setFrameShadow(QFrame::Plain);
    rightFrame->setFrameShape(QFrame::NoFrame);

    mainLayout->setContentsMargins(0,0,0,0);

    int oldHeigh = this->size().height();

    this->resize(200,oldHeigh);
    this->setMinimumSize(0,0);
    this->setFixedWidth(200);

    //this->adjustSize();
    //this->setWindowFlags(defaultWindowFlags);
    //this->show();

    ui->hide_sidebar_btn->setToolTip("Go Mini");
    ui->hide_sidebar_btn->setIcon(QIcon(":Data/data/mini_mode.svg"));
    this->viewMode=PLAYLISTMODE;
}

void MainWindow::keepOnTop(bool state)
{
    if (state) this->setWindowFlags(Qt::WindowStaysOnTopHint);
    else this->setWindowFlags(defaultWindowFlags);

    this->show();
}

void MainWindow::setStyle()
{

    /* ui->mainToolBar->setStyleSheet(" QToolBar { border-right: 1px solid #575757; } QToolButton:hover { background-color: #d8dfe0; border-right: 1px solid #575757;}");
    playback->setStyleSheet("QToolBar { border:none;} QToolBar QToolButton { border:none;} QToolBar QSlider { border:none;}");
    this->setStyleSheet("QToolButton { border: none; padding: 5px; }  QMainWindow { border-top: 1px solid #575757; }");*/
    //status->setStyleSheet("QToolButton { color:#fff; } QToolBar {background-color:#575757; color:#fff; border:1px solid #575757;} QToolBar QLabel { color:#fff;}" );

}



void MainWindow::on_hide_sidebar_btn_clicked()
{
    switch(this->viewMode)
    {
    case FULLMODE: go_playlistMode(); break;

    case PLAYLISTMODE: go_mini(); break;

    case MINIMODE: expand(); break;
    }

}

void MainWindow::on_shuffle_btn_clicked() //tofix
{

    if(shuffle_state == REGULAR)
    {
        shuffle = true;
        repeat = false;
        shufflePlaylist();
        ui->shuffle_btn->setIcon(QIcon(":Data/data/media-playlist-shuffle.svg"));
        ui->shuffle_btn->setToolTip("Repeat");
        shuffle_state = SHUFFLE;

    }else if (shuffle_state == SHUFFLE)
    {

        repeat = true;
        shuffle = false;
        ui->shuffle_btn->setIcon(QIcon(":Data/data/media-playlist-repeat.svg"));
        ui->shuffle_btn->setToolTip("Consecutive");
        shuffle_state = REPEAT;


    }else if(shuffle_state == REPEAT)
    {
        repeat = false;
        shuffle = false;
        ui->shuffle_btn->setIcon(QIcon(":Data/data/view-media-playlist.svg"));
        ui->shuffle_btn->setToolTip("Shuffle");
        shuffle_state = REGULAR;
    }
}

void MainWindow::on_open_btn_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),QDir().homePath()+"/Music/", tr("Audio (*.mp3 *.wav *.mp4 *.flac *.ogg *.m4a)"));
    if(!files.isEmpty())
    {
        auto tracks = new Playlist();
        tracks->add(files);

        addToPlaylist(tracks->getTracksData());
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
        mainList->addRow(list);
}


void MainWindow::on_mainList_clicked(QList<QMap<int,QString>> list)
{
    Q_UNUSED(list);
    lCounter = mainList->getIndex();
    loadTrack();

    if(!currentList.contains(current_song)) currentList<<current_song;

    ui->play_btn->setIcon(QIcon(":Data/data/media-playback-pause.svg"));
}

void MainWindow::removeSong(const int &index)
{
    QObject* obj = sender();

    if(index != -1)
    {
        qDebug()<<"ehat was in current list:";
        for(auto a: currentList)
        {
            qDebug()<<a[BabeTable::TITLE];
        }
        if(obj == mainList) currentList.removeAt(index);
        qDebug()<<"in current list:";
        for(auto a: currentList)
        {
            qDebug()<<a[BabeTable::TITLE];
        }
        //updateList();
        // mainList->setCurrentCell(index,0);

        if(shuffle) shufflePlaylist();
    }
}

void MainWindow::loadTrack()
{
    //mainList->item(current_song_pos,BabeTable::TITLE)->setIcon(QIcon());
    prev_song = current_song;

    current_song_pos = mainList->getIndex();
    current_song = mainList->getRowData(current_song_pos);
    //mainList->item(current_song_pos,BabeTable::TITLE)->setIcon(QIcon::fromTheme("media-playback-pause"));

    mainList->scrollTo(mainList->model()->index(current_song_pos,BabeTable::TITLE));
    queued_song_pos = -1;


    qDebug()<<"in mainlist="<<current_song[BabeTable::LOCATION];

    if(BaeUtils::fileExists(current_song[BabeTable::LOCATION]))
    {
        player->setMedia(QUrl::fromLocalFile(current_song[BabeTable::LOCATION]));
        player->play();

        timer->start(3000);

        ui->play_btn->setIcon(QIcon(":Data/data/media-playback-pause.svg"));

        this->setWindowTitle(current_song[BabeTable::TITLE]+" \xe2\x99\xa1 "+current_song[BabeTable::ARTIST]);

        album_art->setTitle(current_song[BabeTable::ARTIST],current_song[BabeTable::ALBUM]);

        //CHECK IF THE SONG IS BABED IT OR IT ISN'T
        if(isBabed(current_song))
            ui->fav_btn->setIcon(QIcon(":Data/data/loved.svg"));
        else
            ui->fav_btn->setIcon(QIcon(":Data/data/love-amarok.svg"));

        loadMood();

        loadCover(current_song[BabeTable::ARTIST],current_song[BabeTable::ALBUM],current_song[BabeTable::TITLE]);


        if(!this->isActiveWindow())
            nof.notifySong(current_song,album_art->getPixmap());


    }else removeSong(current_song_pos);

}

void MainWindow::feedRabbit()
{
    rabbitTable->flushSuggestions(RabbitView::GENERAL);

    Pulpo rabbitInfo(current_song[BabeTable::TITLE],current_song[BabeTable::ARTIST],current_song[BabeTable::ALBUM]);

    //if(prev_song[BabeTable::ARTIST] != current_song[BabeTable::ARTIST])

    rabbitTable->flushSuggestions();
    rabbitTable->populateGeneralSuggestion(connection.getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+current_song[BabeTable::ARTIST]+"\"")));

    connect(&rabbitInfo, &Pulpo::artistSimilarReady, [this] (QMap<QString,QByteArray> info)
    {
        rabbitTable->populateArtistSuggestion(info);
        QStringList query;
        for (auto tag : info.keys()) query << QString("artist:"+tag).trimmed();
        auto searchResults = this->searchFor(query);

        if(!searchResults.isEmpty()) rabbitTable->populateGeneralSuggestion(searchResults);

    });


    connect(&rabbitInfo, &Pulpo::albumTagsReady, [this] (QStringList tags)
    {

        auto searchResults = this->searchFor(tags);

        if(!searchResults.isEmpty()) rabbitTable->populateGeneralSuggestion(searchResults);

    });

    rabbitInfo.fetchAlbumInfo(Pulpo::AlbumTags,Pulpo::LastFm);
    rabbitInfo.fetchArtistInfo(Pulpo::ArtistSimilar,Pulpo::LastFm);


    // rabbitTable->populateGeneralSuggestion(searchFor({"genre:"+current_song[BabeTable::GENRE]}));

}

bool MainWindow::isBabed(QMap<int,QString> track)
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

    if(!color.isEmpty())
    {
        /*QColor mood;::item:selected
         mood.setNamedColor(color);
         mood = mood.lighter(120);*/
        seekBar->setStyleSheet(QString("QSlider\n{\nbackground:transparent;}\nQSlider::groove:horizontal {border: none; background: transparent; height: 5px; border-radius: 0; } QSlider::sub-page:horizontal {\nbackground: %1 ;border: none; height: 5px;border-radius: 0;} QSlider::add-page:horizontal {\nbackground: transparent; border: none; height: 5px; border-radius: 0; } QSlider::handle:horizontal {background: %1; width: 8px; } QSlider::handle:horizontal:hover {background: qlineargradient(x1:0, y1:0, x2:1, y2:1,stop:0 #fff, stop:1 #ddd);border: 1px solid #444;border-radius: 4px;}QSlider::sub-page:horizontal:disabled {background: #bbb;border-color: #999;}QSlider::add-page:horizontal:disabled {background: #eee;border-color: #999;}QSlider::handle:horizontal:disabled {background: #eee;border: 1px solid #aaa;border-radius: 4px;}").arg(color));
        mainList->setStyleSheet(QString("QTableWidget::item:selected {background:rgba( %1, %2, %3, 40); color: %4}").arg(QString::number(QColor(color).toRgb().red()),QString::number(QColor(color).toRgb().green()),QString::number(QColor(color).toRgb().blue()),mainList->palette().color(QPalette::WindowText).name()));
        ui->mainToolBar->setStyleSheet(QString("QToolBar {margin:0; background-color:rgba( %1, %2, %3, 20); background-image:url('%4');} QToolButton{ border-radius:0;} QToolButton:checked{border-radius:0; background: rgba( %1, %2, %3, 155); color: %5;}").arg(QString::number(QColor(color).toRgb().red()),QString::number(QColor(color).toRgb().green()),QString::number(QColor(color).toRgb().blue()),":Data/data/pattern.png",this->palette().color(QPalette::BrightText).name()));

    }else
    {
        //ui->listWidget->setBackgroundRole(QPalette::Highlight);
        //ui->listWidget->setpa
        seekBar->setStyleSheet(QString("QSlider { background:transparent;} QSlider::groove:horizontal {border: none; background: transparent; height: 5px; border-radius: 0; } QSlider::sub-page:horizontal { background: %1;border: none; height: 5px;border-radius: 0;} QSlider::add-page:horizontal {background: transparent; border: none; height: 5px; border-radius: 0; } QSlider::handle:horizontal {background: %1; width: 8px; } QSlider::handle:horizontal:hover {background: qlineargradient(x1:0, y1:0, x2:1, y2:1,stop:0 #fff, stop:1 #ddd);border: 1px solid #444;border-radius: 4px;}QSlider::sub-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::add-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::handle:horizontal:disabled {background: transparent;border: 1px solid #aaa;border-radius: 4px;}").arg(this->palette().color(QPalette::Highlight).name()));
        mainList->setStyleSheet(QString("QTableWidget::item:selected {background:%1; color: %2}").arg(this->palette().color(QPalette::Highlight).name(),this->palette().color(QPalette::BrightText).name()));
        ui->mainToolBar->setStyleSheet(QString("QToolBar {margin:0; background-color:rgba( 0, 0, 0, 0); background-image:url('%1');} QToolButton{ border-radius:0;} QToolButton:checked{border-radius:0; background: %2; color:%3;}").arg(":Data/data/pattern.png",this->palette().color(QPalette::Highlight).name(),this->palette().color(QPalette::BrightText).name()));

    }
}


bool MainWindow::loadCover(const QString &artist, const QString &album, const QString &title) //tofix separte getalbumcover from get artisthead
{
    Q_UNUSED(title);
    QString artistHead;
    QString albumCover;

    //IF CURRENT SONG EXISTS IN THE COLLECTION THEN GET THE COVER FROM DB
    if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM tracks WHERE location = \""+current_song[BabeTable::LOCATION]+"\""))
    {
        artistHead = settings_widget->getCollectionDB().getArtistArt(artist);

        if(!artistHead.isEmpty())
        {
            infoTable->setArtistArt(artistHead);
            infoTable->artist->setArtist(artist);

        }else infoTable->setArtistArt(QString(":Data/data/cover.svg"));


        albumCover = settings_widget->getCollectionDB().getAlbumArt(album, artist);

        if(!albumCover.isEmpty())
            album_art->putPixmap(albumCover);
        else  if (!artistHead.isEmpty()) album_art->putPixmap(artistHead);
        else album_art->putDefaultPixmap();

        return true;

    }else
    {
        qDebug()<<"Song path does not exits in db so going to get artwork somehowelse <<"<<album<<artist;

        if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM albums WHERE title = \""+album+"\" AND artist = \""+artist+"\""))
        {
            artistHead = settings_widget->getCollectionDB().getArtistArt(artist);

            albumCover = settings_widget->getCollectionDB().getAlbumArt(album, artist);

            if(!albumCover.isEmpty())
                album_art->putPixmap(albumCover);
            else  if (!artistHead.isEmpty()) album_art->putPixmap(artistHead);
            else album_art->putDefaultPixmap();

            return true;
        }else
        {
            emit fetchCover(artist,album,title);
            return false;
        }

    }
}

void MainWindow::addToQueue(QMap<int,QString> track)
{
    queued_song_pos = queued_song_pos > 0 ? queued_song_pos+1 : current_song_pos+1;
    queued_songs.insert(track[BabeTable::LOCATION],track);

    mainList->addRowAt(queued_song_pos,track,true);
    mainList->item(queued_song_pos,BabeTable::TITLE)->setIcon(QIcon::fromTheme("clock"));
    //mainList->addRowAt(current_song_pos+1,track,true);
    nof.notify("Song added to Queue",track[BabeTable::TITLE]+" by "+track[BabeTable::ARTIST]);

}

void MainWindow::on_seekBar_sliderMoved(int position)
{
    player->setPosition(player->duration() / 1000 * position);
}

void MainWindow::update()
{
    if(!current_song.isEmpty())
    {
        if(!seekBar->isEnabled()) seekBar->setEnabled(true);

        if(!seekBar->isSliderDown())
            seekBar->setValue(static_cast<int>(static_cast<double>(player->position())/player->duration()*1000));

        if(player->state() == QMediaPlayer::StoppedState)
        {
            if(!queued_songs.isEmpty()) removeQueuedTrack(current_song);

            prev_song = current_song;
            qDebug()<<"finished playing song: "<<prev_song[BabeTable::LOCATION];

            emit finishedPlayingSong(prev_song[BabeTable::LOCATION]);
            next();
        }


    }else
    {
        seekBar->setValue(0);
        seekBar->setEnabled(false);
    }
}


bool MainWindow::removeQueuedTrack(QMap<int, QString> track)
{
    if(queued_songs.contains(track[BabeTable::LOCATION]))
        if(mainList->item(current_song_pos,BabeTable::TITLE)->icon().name()=="clock")
        {
            mainList->removeRow(current_song_pos);
            queued_songs.remove(track[BabeTable::LOCATION]);
            lCounter--;
            return true;
        }

    return false;
}

void MainWindow::next()
{
    if(!queued_songs.isEmpty()) removeQueuedTrack(current_song); //check if the track was queued and then removed it

    lCounter++;

    if(repeat) lCounter--;

    if(lCounter >= mainList->rowCount()) lCounter = 0;

    mainList->setCurrentCell((!shuffle || repeat) ?
                                 lCounter : shuffledPlaylist[static_cast<unsigned short int>(lCounter)], 0);

    loadTrack();
}


void MainWindow::back()
{
    lCounter--;

    if(lCounter < 0)
        lCounter = mainList->rowCount() - 1;

    (!shuffle) ? mainList->setCurrentCell(lCounter,0) :
                 mainList->setCurrentCell(shuffledPlaylist[static_cast<unsigned short int>(lCounter)],0);

    loadTrack();
}

void MainWindow::shufflePlaylist()
{
    shuffledPlaylist.resize(0);

    for(int i = 0; i < mainList->rowCount(); i++)
        shuffledPlaylist.push_back(static_cast<unsigned short>(i));

    random_shuffle(shuffledPlaylist.begin(), shuffledPlaylist.end());
}

void MainWindow::on_play_btn_clicked()
{
    if(mainList->rowCount() > 0 || !current_song.isEmpty())
    {
        if(player->state() == QMediaPlayer::PlayingState)
        {
            player->pause();
            ui->play_btn->setIcon(QIcon(":Data/data/media-playback-start.svg"));
        }
        else
        {
            player->play();
            //updater->start();
            ui->play_btn->setIcon(QIcon(":Data/data/media-playback-pause.svg"));
        }
    }
}

void MainWindow::on_backward_btn_clicked()
{
    if(mainList->rowCount() > 0)
    {
        if(player->position() > 3000) player->setPosition(0);
        else
        {
            back();
            ui->play_btn->setIcon(QIcon(":Data/data/media-playback-pause.svg"));
        }
    }
}

void MainWindow::on_foward_btn_clicked()
{
    if(mainList->rowCount() > 0)
    {
        if(repeat)
        {
            repeat = !repeat;
            next();

        }else
        {
            next();
            ui->play_btn->setIcon(QIcon(":Data/data/media-playback-pause.svg"));
        }
    }
}


void MainWindow::collectionDBFinishedAdding(bool state)
{
    if(state)
    {
        if(!ui->fav_btn->isEnabled()) ui->fav_btn->setEnabled(true);
        qDebug()<<"now it i time to put the tracks in the table ;)";
        //settings_widget->getCollectionDB().closeConnection();
        albumsTable->flushGrid();
        artistsTable->flushGrid();
        refreshTables();

    }else refreshTables();

}

void MainWindow::orderTables()
{
    //favoritesTable->setTableOrder(BabeTable::STARS,BabeTable::DESCENDING);
    collectionTable->setTableOrder(BabeTable::ARTIST,BabeTable::ASCENDING);
    qDebug()<<"finished populating tables, now ordering them";
}


void MainWindow::on_fav_btn_clicked()
{
    if(!isBabed(current_song))
    {
        if(babeIt(current_song))  ui->fav_btn->setIcon(QIcon(":Data/data/loved.svg"));
    }else
    {
        if(unbabeIt(current_song)) ui->fav_btn->setIcon(QIcon(":Data/data/love-amarok.svg"));
        qDebug()<<"brujas";
    }
}

void MainWindow::babeAlbum(QMap<int,QString> info)
{
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];

    QList<QMap<int,QString>> mapList;
    if(album.isEmpty())
        mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\""));
    else if(!artist.isEmpty())
        mapList = settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE artist = \""+artist+"\" and album = \""+album+"\""));

    if(!mapList.isEmpty())
        for (auto track : mapList) babeIt(track);

}

bool MainWindow::unbabeIt(QMap<int,QString> track)
{
    if(settings_widget->getCollectionDB().insertInto("tracks","babe",track[BabeTable::LOCATION],0))
    {
        nof.notify("Song unBabe'd it",track[BabeTable::TITLE]+" by "+track[BabeTable::ARTIST]);
        return  true;
    } else return false;

}

bool MainWindow::babeIt(const QMap<int, QString> &track)
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
                addToPlaylist({track},true);
                return true;
            } else return false;

        }else
        {

            ui->fav_btn->setEnabled(false);

            if(addToCollectionDB({url},"1"))
            {
                nof.notify("Song Babe'd it",track[BabeTable::TITLE]+" by "+track[BabeTable::ARTIST]);
                return true;
            }
            else return false;

            ui->fav_btn->setEnabled(true);
        }

    }

    return true;
}


void  MainWindow::infoIt(QString title, QString artist, QString album)
{
    //views->setCurrentIndex(INFO);
    infoView();
    infoTable->getTrackInfo(title, artist,album);

}




void MainWindow::scanNewDir(QString url,QString babe)
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
        if(addToCollectionDB(list,babe))
            nof.notify("New music added to collection",list.join("\n"));

    }else
    {
        refreshTables();
        settings_widget->refreshWatchFiles(); qDebug()<<"a folder probably got removed or changed";
    }
}


bool MainWindow::addToCollectionDB(QStringList url, QString babe)
{
    if(settings_widget->getCollectionDB().addTrack(url,babe.toInt()))
    {
        if(babe.contains("1"))
            for(auto track: url) addToPlaylist(settings_widget->getCollectionDB().getTrackData(url),true);

        return true;
    }else return false;

}


void MainWindow::addToPlaylist(QList<QMap<int,QString>> mapList, bool notRepeated)
{
    //currentList.clear();
    qDebug()<<"Adding mapList to mainPlaylist";

    if(notRepeated)
    {
        QList<QMap<int,QString>> newList;
        QStringList alreadyInList = mainList->getTableContent(BabeTable::LOCATION);
        for(auto track: mapList)
        {
            if(!alreadyInList.contains(track[BabeTable::LOCATION]))
            {
                newList<<track;
                mainList->addRow(track,true);
            }
        }

        currentList+=newList;
    }else
    {
        currentList+=mapList;
        for(auto track:mapList) mainList->addRow(track, true);
    }

    mainList->resizeRowsToContents();

    //updateList();
    if(shuffle) shufflePlaylist();
    //mainList->scrollToBottom();
}


void  MainWindow::clearCurrentList()
{
    currentList.clear();
    mainList->flushTable();
}

void MainWindow::on_search_returnPressed()
{
    if(resultsTable->rowCount()<1)
    {
        views->setCurrentIndex(prevIndex);
        if(views->currentIndex()==ALBUMS)  utilsBar->actions().at(ALBUMS_UB)->setVisible(true);;
        if(views->currentIndex()==PLAYLISTS) {utilsBar->actions().at(PLAYLISTS_UB)->setVisible(true); ui->frame_3->setVisible(true);}
        resultsTable->flushTable();
        // ui->saveResults->setEnabled(false);
    }else views->setCurrentIndex(RESULTS);
}

void MainWindow::on_search_textChanged(const QString &arg1)
{

    if(!ui->search->text().isEmpty())
    {
        QStringList searchList=arg1.split(",");

        auto searchResults = searchFor(searchList);

        if(!searchResults.isEmpty()) populateResultsTable(searchResults);

    }else views->setCurrentIndex(prevIndex);


}

void MainWindow::populateResultsTable(QList<QMap<int,QString>> mapList)
{
    views->setCurrentIndex(RESULTS);
    utilsBar->actions().at(ALBUMS_UB)->setVisible(false);
    resultsTable->flushTable();
    resultsTable->populateTableView(mapList,false);
}

QList<QMap<int, QString> > MainWindow::searchFor(QStringList queries)
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
            if(prevIndex==PLAYLISTS) {utilsBar->actions().at(PLAYLISTS_UB)->setVisible(false); ui->frame_3->setVisible(false);}

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

            else
                mapList += settings_widget->getCollectionDB().getTrackData(QString("SELECT * FROM tracks WHERE title LIKE \"%"+searchQuery+"%\" OR artist LIKE \"%"+searchQuery+"%\" OR album LIKE \"%"+searchQuery+"%\"OR genre LIKE \"%"+searchQuery+"%\""));

        }
    }

    return mapList;
}



void MainWindow::on_rowInserted(QModelIndex model ,int x,int y)
{
    //Q_UNUSED(model);
    //Q_UNUSED(x);Q_UNUSED(y);
    //mainList->scrollToBottom();
    //qDebug()<<x<<y;

    mainList->scrollTo(mainList->model()->index(x,BabeTable::TITLE),QAbstractItemView::PositionAtCenter);

    //qDebug()<<"indexes moved";
    //addMusicImg->setVisible(false);
}


void MainWindow::clearMainList()
{
    album_art->putDefaultPixmap();
    currentList.clear();
    current_song.clear();
    this->mainList->flushTable();
    lCounter=-1;
    player->stop();
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
        addToPlaylist(collectionTable->getAllTableContent()); break;
    case ALBUMS:
        addToPlaylist(albumsTable->albumTable->getAllTableContent()); break;
    case ARTISTS:
        addToPlaylist(artistsTable->albumTable->getAllTableContent()); break;
    case PLAYLISTS:
        addToPlaylist(playlistTable->table->getAllTableContent()); break;
    case RABBIT:
        addToPlaylist(rabbitTable->getTable()->getAllTableContent()); break;
    case RESULTS:
        addToPlaylist(resultsTable->getAllTableContent()); break;
    }
}

void MainWindow::on_saveResults_clicked()
{

    qDebug()<<"on_saveResults_clicked";
    saveResults_menu->clear();

    for(auto action: collectionTable->getPlaylistMenus()) saveResults_menu->addAction(action);
    ui->saveResults->showMenu();

}



void MainWindow::saveResultsTo(QAction *action)
{
    QString playlist=action->text().replace("&","");
    switch(views->currentIndex())
    {
    case COLLECTION: collectionTable->populatePlaylist(collectionTable->getTableContent(BabeTable::LOCATION),playlist); break;
    case ALBUMS: albumsTable->albumTable->populatePlaylist(albumsTable->albumTable->getTableContent(BabeTable::LOCATION),playlist); break;
    case ARTISTS: artistsTable->albumTable->populatePlaylist(artistsTable->albumTable->getTableContent(BabeTable::LOCATION),playlist); break;
    case PLAYLISTS: playlistTable->table->populatePlaylist(playlistTable->table->getTableContent(BabeTable::LOCATION),playlist); break;
        //case INFO: collectionTable->populatePlaylist(collectionTable->getTableContent(BabeTable::LOCATION),playlist); break;
        //case SETTINGS:  collectionTable->populatePlaylist(collectionTable->getTableContent(BabeTable::LOCATION),playlist); break;
    case RESULTS: resultsTable->populatePlaylist(resultsTable->getTableContent(BabeTable::LOCATION),playlist); break;

    }
}

void MainWindow::on_filterBtn_clicked()
{
    if(ui->filterBtn->isChecked())
    {
        ui->filterBtn->setChecked(true);
        ui->filterBox->setVisible(true);
        ui->filter->setFocus();
    }else
    {
        ui->filterBtn->setChecked(false);
        ui->filterBox->setVisible(false);
    }

}

void MainWindow::on_filter_textChanged(const QString &arg1)
{

    if(!ui->filter->text().isEmpty())
    {
        QStringList searchList=arg1.split(",");

        auto searchResults = searchFor(searchList);

        if(!searchResults.isEmpty())
        {
            mainList->flushTable();
            mainList->populateTableView(searchResults,true);
        }else
        {
            mainList->flushTable();
            auto old = currentList;
            currentList.clear();
            addToPlaylist(old);

        }

    }else
    {

        mainList->flushTable();
        auto old = currentList;
        currentList.clear();
        addToPlaylist(old);

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
        mainList->setCurrentCell(0,BabeTable::TITLE);
        lCounter=0;
        loadTrack();
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
