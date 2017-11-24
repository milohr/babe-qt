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


#include "babewindow.h"
#include "ui_babewindow.h"
#include "../pulpo/services/geniusService.h"

BabeWindow::BabeWindow(const QStringList &files, QWidget *parent) : QMainWindow(parent),
    ui(new Ui::BabeWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    this->setWindowIcon(QIcon(":Data/data/48-apps-babe.svg"));
    this->setWindowIconText("Babe...");
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->defaultWindowFlags = this->windowFlags();
    this->player = new QMediaPlayer(this);
    //mpris = new Mpris(this);
    this->nof = new Notify(this);
    this->album_art = new BabeAlbum(Bae::DB{{Bae::KEY::ARTWORK,":Data/data/babe.png"}},Bae::AlbumSizeHint::BIG_ALBUM,0,false,this);
    connect(album_art,&BabeAlbum::playAlbum,this,&BabeWindow::putAlbumOnPlay);
    connect(album_art,&BabeAlbum::babeAlbum,this,&BabeWindow::babeAlbum);

    this->ALBUM_SIZE = album_art->getSize();

    album_art->setFixedSize(static_cast<int>(ALBUM_SIZE),static_cast<int>(ALBUM_SIZE));
    album_art->setTitleGeometry(0,0,static_cast<int>(ALBUM_SIZE),static_cast<int>(ALBUM_SIZE*0.15));
    album_art->showTitle(false);
    album_art->showPlayBtn=false;

    this->setMinimumSize(this->minimumSizeHint().width(),0);
    this->defaultGeometry = (QStyle::alignedRect(
                                 Qt::LeftToRight,
                                 Qt::AlignCenter,
                                 qApp->desktop()->availableGeometry().size()*0.7,
                                 qApp->desktop()->availableGeometry()
                                 ));

    this->setGeometry( this->loadSettings("GEOMETRY","MAINWINDOW",defaultGeometry).toRect());
    this->saveSettings("GEOMETRY",this->geometry(),"MAINWINDOW");

    connect(this, &BabeWindow::finishedPlayingSong, this, &BabeWindow::addToPlayed);
    connect(this,&BabeWindow::fetchCover,this,&BabeWindow::setCoverArt);
    connect(this,&BabeWindow::collectionChecked,this,&BabeWindow::refreshTables);

    //* SETUP BABE PARTS *//
    this->setUpViews();
    this->setUpPlaylist();
    this->setUpRightFrame();
    this->setUpCollectionViewer();

    seekBar->installEventFilter(this);
    ui->controls->installEventFilter(this);

    settings_widget->setToolbarIconSize(this->loadSettings("TOOLBAR","MAINWINDOW",QVariant(16)).toInt());

    connect(nof,&Notify::babeSong,[this](const Bae::DB &track)
    {
        if(this->babeTrack(track))
            this->babedIcon(this->isBabed(track));
    });
    connect(nof,&Notify::skipSong,this,&BabeWindow::next);
    connect(updater, &QTimer::timeout, this, &BabeWindow::update);

    if(!files.isEmpty())
    {
        this->appendFiles(files, APPENDTOP);
        current_song_pos = 0;
    }else  current_song_pos = this->loadSettings("PLAYLIST_POS","MAINWINDOW",QVariant(0)).toInt();

    if(this->loadSettings("MINIPLAYBACK","MAINWINDOW",false).toBool())
        emit ui->miniPlaybackBtn->clicked();
    else ui->controls->setVisible(false);

    movePanel(static_cast<position>(this->loadSettings("PANEL_POS","MAINWINDOW",RIGHT).toInt()));
    this->loadStyle();
    this->player->setVolume(100);
}


BabeWindow::~BabeWindow()
{
    qDebug()<<"DELETING BABEWINDOW";
    delete ui;
}

void BabeWindow::start()
{
    //* CHECK FOR DATABASE *//
    if(settings_widget->checkCollection())
    {
        qDebug()<<"COLLECTION DB EXISTS";
        auto savedList = loadSettings("PLAYLIST","MAINWINDOW").toStringList();
        this->addToPlaylist(connection.getDBData(savedList),false, APPENDBOTTOM);
        if(this->mainList->rowCount()==0) populateMainList();
        emit collectionChecked({{TABLE::TRACKS, true},{TABLE::ALBUMS, true},{TABLE::ARTISTS, true},{TABLE::PLAYLISTS, true}});
    } else settings_widget->createCollectionDB();

    if(mainList->rowCount()>0)
    {
        mainList->setCurrentCell(current_song_pos>=mainList->rowCount()? 0 : current_song_pos,static_cast<int>(Bae::KEY::TITLE));
        loadTrack();
        collectionView();
        go_playlistMode();

    }else if(collectionTable->rowCount()>0) collectionView();
    else settingsView();

    updater->start(100);

}


void BabeWindow::saveSettings(const QString &key, const QVariant &value, const QString &group)
{
    QSettings setting("Babe","babe");
    setting.beginGroup(group);
    setting.setValue(key,value);
    setting.endGroup();
}

QVariant BabeWindow::loadSettings(const QString &key, const QString &group, const QVariant &defaultValue)
{
    QVariant variant;
    QSettings setting("Babe","babe");
    setting.beginGroup(group);
    variant = setting.value(key,defaultValue);
    setting.endGroup();

    return variant;
}

//*HERE THE MAIN VIEWS GET SETUP WITH THEIR SIGNALS AND SLOTS**//
void BabeWindow::setUpViews()
{
    settings_widget = new settings(this); //this needs to go first

    playlistTable = new PlaylistsView(this);
    connect(playlistTable->table,&BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(playlistTable->table,&BabeTable::removeIt_clicked,this,&BabeWindow::removeSong);
    connect(playlistTable->table,&BabeTable::babeIt_clicked,this,&BabeWindow::babeIt);
    connect(playlistTable->table,&BabeTable::queueIt_clicked,this,&BabeWindow::addToQueue);
    connect(playlistTable->table,&BabeTable::infoIt_clicked,this,&BabeWindow::infoIt);
    connect(playlistTable->table,&BabeTable::previewStarted,this,&BabeWindow::pause);
    connect(playlistTable->table,&BabeTable::previewFinished,this,&BabeWindow::play);
    connect(playlistTable->table,&BabeTable::playItNow,this,&BabeWindow::playItNow);
    connect(playlistTable->table,&BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(playlistTable->table,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    collectionTable = new BabeTable(this);
    collectionTable->showColumn(static_cast<int>(Bae::KEY::STARS));
    collectionTable->showColumn(static_cast<int>(Bae::KEY::GENRE));
    connect(collectionTable,&BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(collectionTable,&BabeTable::finishedPopulating,[this]()
    {
        collectionTable->sortByColumn(static_cast<int>(Bae::KEY::ARTIST), Qt::AscendingOrder);
    });
    connect(collectionTable,&BabeTable::removeIt_clicked,this,&BabeWindow::removeSong);
    connect(collectionTable,&BabeTable::babeIt_clicked,this,&BabeWindow::babeIt);
    connect(collectionTable,&BabeTable::queueIt_clicked,this,&BabeWindow::addToQueue);
    connect(collectionTable,&BabeTable::infoIt_clicked,this,&BabeWindow::infoIt);
    connect(collectionTable,&BabeTable::previewStarted,this,&BabeWindow::pause);
    connect(collectionTable,&BabeTable::previewFinished,this,&BabeWindow::play);
    connect(collectionTable,&BabeTable::playItNow,this,&BabeWindow::playItNow);
    connect(collectionTable,&BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(collectionTable,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    mainList = new BabeTable(this);
    mainList->setObjectName("mainList");
    mainList->hideColumn(static_cast<int>(Bae::KEY::ALBUM));
    mainList->hideColumn(static_cast<int>(Bae::KEY::ARTIST));
    mainList->hideColumn(static_cast<int>(Bae::KEY::DURATION));
    mainList->horizontalHeader()->setVisible(false);
    mainList->enableRowColoring(true);
    mainList->enableRowDragging(true);
    mainList->enablePreview(false);
//    mainList->setStyleSheet("QTableWidget { background:transparent; }");

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

    connect(mainList,&BabeTable::tableWidget_doubleClicked,this,&BabeWindow::on_mainList_clicked);
    connect(mainList,&BabeTable::removeIt_clicked,this,&BabeWindow::removeSong);
    connect(mainList,&BabeTable::babeIt_clicked,this,&BabeWindow::babeIt);
    connect(mainList,&BabeTable::queueIt_clicked,this,&BabeWindow::addToQueue);
    connect(mainList,&BabeTable::moodIt_clicked,mainList,&BabeTable::colorizeRow);
    connect(mainList,&BabeTable::infoIt_clicked,this,&BabeWindow::infoIt);
    connect(mainList,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);

    filterList = new BabeTable(this);
    filterList->hideColumn(static_cast<int>(Bae::KEY::ALBUM));
    filterList->hideColumn(static_cast<int>(Bae::KEY::ARTIST));
    filterList->horizontalHeader()->setVisible(false);
    filterList->enableRowColoring(true);

    filterList->setAddMusicMsg("\nDidn't find anything!","face-surprise");
    connect(filterList,&BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list)
    {
        playItNow(list);
        ui->filter->clear();
    });

    connect(filterList,&BabeTable::removeIt_clicked,this,&BabeWindow::removeSong);
    connect(filterList,&BabeTable::babeIt_clicked,this,&BabeWindow::babeIt);
    connect(filterList,&BabeTable::queueIt_clicked,this,&BabeWindow::addToQueue);
    connect(filterList,&BabeTable::moodIt_clicked,mainList,&BabeTable::colorizeRow);
    connect(filterList,&BabeTable::infoIt_clicked,this,&BabeWindow::infoIt);
    connect(filterList,&BabeTable::previewStarted,this,&BabeWindow::pause);
    connect(filterList,&BabeTable::previewFinished,this,&BabeWindow::play);
    connect(filterList,&BabeTable::appendIt, [this] (Bae::DB_LIST list)
    {
        addToPlaylist(list,false,APPENDAFTER);

    });
    connect(filterList,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    mainListView = new QStackedWidget(this);
    mainListView->setFrameShape(QFrame::NoFrame);
    mainListView->addWidget(this->mainList);
    mainListView->addWidget(this->filterList);

    resultsTable=new BabeTable(this);
    //    resultsTable->passStyle("QHeaderView::section { background-color:#575757; color:white; }");
    resultsTable->horizontalHeader()->setHighlightSections(true);
    resultsTable->showColumn(static_cast<int>(Bae::KEY::STARS));
    resultsTable->showColumn(static_cast<int>(Bae::KEY::GENRE));
    connect(resultsTable,&BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(resultsTable,&BabeTable::removeIt_clicked,this,&BabeWindow::removeSong);
    connect(resultsTable,&BabeTable::babeIt_clicked,this,&BabeWindow::babeIt);
    connect(resultsTable,&BabeTable::queueIt_clicked,this,&BabeWindow::addToQueue);
    connect(resultsTable,&BabeTable::infoIt_clicked,this,&BabeWindow::infoIt);
    connect(resultsTable,&BabeTable::previewStarted,this,&BabeWindow::pause);
    connect(resultsTable,&BabeTable::previewFinished,this,&BabeWindow::play);
    connect(resultsTable,&BabeTable::playItNow,this,&BabeWindow::playItNow);
    connect(resultsTable,&BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(resultsTable,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    albumsTable = new AlbumsView(false,this);
    connect(albumsTable, &AlbumsView::populateFinished, []()
    {
        qDebug()<<"Finished populating albumsTable";
    });
    connect(albumsTable->albumTable,&BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(albumsTable->albumTable,&BabeTable::removeIt_clicked,this,&BabeWindow::removeSong);
    connect(albumsTable->albumTable,&BabeTable::babeIt_clicked,this,&BabeWindow::babeIt);
    connect(albumsTable->albumTable,&BabeTable::queueIt_clicked,this,&BabeWindow::addToQueue);
    connect(albumsTable->albumTable,&BabeTable::infoIt_clicked,this,&BabeWindow::infoIt);
    connect(albumsTable,&AlbumsView::playAlbum,this,&BabeWindow::putAlbumOnPlay);
    connect(albumsTable,&AlbumsView::babeAlbum,this,&BabeWindow::babeAlbum);
    connect(albumsTable,&AlbumsView::albumDoubleClicked,this,&BabeWindow::albumDoubleClicked);
    connect(albumsTable,&AlbumsView::expandTo,this,&BabeWindow::expandAlbumList);
    connect(albumsTable->albumTable,&BabeTable::previewStarted,this,&BabeWindow::pause);
    connect(albumsTable->albumTable,&BabeTable::previewFinished,this,&BabeWindow::play);
    connect(albumsTable->albumTable,&BabeTable::playItNow,this,&BabeWindow::playItNow);
    connect(albumsTable->albumTable,&BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(albumsTable->albumTable,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    artistsTable = new AlbumsView(true,this);
    connect(artistsTable, &AlbumsView::populateFinished, []()
    {
        qDebug()<<"Finished populating artistTable";
    });
    artistsTable->expandBtn->setVisible(false);
    artistsTable->albumTable->showColumn(static_cast<int>(Bae::KEY::ALBUM));
    connect(artistsTable->albumTable,&BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(artistsTable->albumTable,&BabeTable::removeIt_clicked,this,&BabeWindow::removeSong);
    connect(artistsTable->albumTable,&BabeTable::babeIt_clicked,this,&BabeWindow::babeIt);
    connect(artistsTable->albumTable,&BabeTable::queueIt_clicked,this,&BabeWindow::addToQueue);
    connect(artistsTable->albumTable,&BabeTable::infoIt_clicked,this,&BabeWindow::infoIt);
    connect(artistsTable,&AlbumsView::playAlbum,this,&BabeWindow::putAlbumOnPlay);
    connect(artistsTable,&AlbumsView::babeAlbum,this,&BabeWindow::babeAlbum);
    connect(artistsTable,&AlbumsView::albumDoubleClicked,this,&BabeWindow::albumDoubleClicked);
    connect(artistsTable->albumTable,&BabeTable::previewStarted,this,&BabeWindow::pause);
    connect(artistsTable->albumTable,&BabeTable::previewFinished,this,&BabeWindow::play);
    connect(artistsTable->albumTable,&BabeTable::playItNow,this,&BabeWindow::playItNow);
    connect(artistsTable->albumTable,&BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(artistsTable->albumTable,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);


    infoTable = new InfoView(this);
    connect(infoTable,&InfoView::playAlbum,this,&BabeWindow::putAlbumOnPlay);
    connect(infoTable,&InfoView::similarBtnClicked,[this](QStringList queries) { this->ui->search->setText(queries.join(",")); });
    connect(infoTable,&InfoView::tagsBtnClicked,[this](QStringList queries) { this->ui->search->setText(queries.join(",")); });
    connect(infoTable,&InfoView::tagClicked,[this](QString query) { this->ui->search->setText(query);});
    connect(infoTable,&InfoView::similarArtistTagClicked,[this](QString query) { this->ui->search->setText(query);});
    //    connect(infoTable,&InfoView::artistSimilarReady, [this] (QMap<QString,QByteArray> info,const Bae::DB &track)
    //    {


    //        if(this->current_song==track)
    //        {
    //
    //            calibrateBtn_menu->actions().at(3)->setEnabled(true);
    //            rabbitTable->flushSuggestions(RabbitView::ALL);
    //            qDebug()<<"&InfoView::artistSimilarReady:"<<info.keys();
    //            rabbitTable->populateArtistSuggestion(info);
    //            QStringList query;
    //            for (auto tag : info.keys()) query << QString("artist:"+tag).trimmed();
    //            auto searchResults = this->searchFor(query);
    //            if(!searchResults.isEmpty()) rabbitTable->populateGeneralSuggestion(searchResults);
    //        }
    //    });

    this->rabbitTable = new RabbitView(this);
    connect(rabbitTable,&RabbitView::playAlbum,this,&BabeWindow::putAlbumOnPlay);
    connect(rabbitTable->generalSuggestion, &BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(rabbitTable->generalSuggestion, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(rabbitTable->generalSuggestion, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(rabbitTable->generalSuggestion, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(rabbitTable->generalSuggestion, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(rabbitTable->generalSuggestion, &BabeTable::previewFinished, this ,&BabeWindow::play);
    connect(rabbitTable->generalSuggestion, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(rabbitTable->generalSuggestion, &BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(rabbitTable->generalSuggestion, &BabeTable::saveToPlaylist, playlistTable,&PlaylistsView::saveToPlaylist);
    connect(rabbitTable->artistSuggestion, &BabeGrid::playAlbum, this,&BabeWindow::putAlbumOnPlay);


    settings_widget->readSettings();
    connect(settings_widget,&settings::toolbarIconSizeChanged, this, &BabeWindow::setToolbarIconSize);
    connect(settings_widget,&settings::refreshTables,this,  &BabeWindow::refreshTables);

    /* THE BUTTONS VIEWS */
    connect(ui->tracks_view,&QToolButton::clicked, this, &BabeWindow::collectionView);
    connect(ui->albums_view,&QToolButton::clicked, this, &BabeWindow::albumsView);
    connect(ui->artists_view,&QToolButton::clicked, this, &BabeWindow::artistsView);
    connect(ui->playlists_view,&QToolButton::clicked, this, &BabeWindow::playlistsView);
    connect(ui->rabbit_view,&QToolButton::clicked, this, &BabeWindow::rabbitView);
    connect(ui->info_view,&QToolButton::clicked, this, &BabeWindow::infoView);
    connect(ui->settings_view,&QToolButton::clicked, this,&BabeWindow::settingsView);

    views = new QStackedWidget(this);
    views->setFrameShape(QFrame::NoFrame);
    views->insertWidget(VIEWS::COLLECTION, collectionTable);
    views->insertWidget(VIEWS::ALBUMS, albumsTable);
    views->insertWidget(VIEWS::ARTISTS, artistsTable);
    views->insertWidget(VIEWS::PLAYLISTS, playlistTable);
    views->insertWidget(VIEWS::INFO, infoTable);
    views->insertWidget(VIEWS::RABBIT, rabbitTable);
    views->insertWidget(VIEWS::SETTINGS, settings_widget);
    views->insertWidget(VIEWS::RESULTS, resultsTable);

}

void BabeWindow::setUpCollectionViewer()
{
    mainLayout = new QHBoxLayout;

    leftFrame_layout = new QGridLayout;
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

    auto showText = new QAction("Hide text",this);
    ui->viewsUtils->addAction(showText);
    connect(showText,&QAction::triggered,[showText,this]()
    {
        for(auto btn : ui->viewsUtils->children())
        {
            if(qobject_cast<QToolButton *>(btn)!=nullptr)
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


    this->searchTimer = new QTimer(this);
    this->searchTimer->setSingleShot(true);
    this->searchTimer->setInterval(500);
    connect(this->searchTimer, &QTimer::timeout,this, &BabeWindow::runSearch);
    connect(this->ui->search, SIGNAL(textChanged(QString)), this->searchTimer, SLOT(start()));

    ui->search->setClearButtonEnabled(true);
    ui->collectionUtils->setVisible(false);
    connect(ui->saveResults,&QToolButton::clicked, this, &BabeWindow::saveResultsTo);


//    leftFrame_layout->addWidget(ui->frame,0,0);
    leftFrame_layout->addWidget(views,0,0);
    leftFrame_layout->addWidget(line,1,0);
    leftFrame_layout->addWidget(ui->viewsUtils,2,0);


    mainLayout->addWidget(leftFrame);
//    mainLayout->addWidget(ui->vLine);
    mainLayout->addWidget(rightFrame);
    mainLayout->setContentsMargins(0,0,0,0);
//    mainLayout->setMargin(0);
//    mainLayout->setSpacing(0);

    this->mainWidget= new QWidget(this);
    this->mainWidget->setLayout(mainLayout);

    this->setCentralWidget(mainWidget);
}

void BabeWindow::setUpPlaylist()
{

    auto *playlistWidget_layout = new QGridLayout;
    playlistWidget_layout->setContentsMargins(0,0,0,0);
    playlistWidget_layout->setSpacing(0);
    playlistWidget_layout->setMargin(0);

    playlistWidget = new QWidget(this);

    playlistWidget->setLayout(playlistWidget_layout);
    playlistWidget->setFixedWidth(static_cast<int>(ALBUM_SIZE));

    //    ui->controls->setParent(album_art);
    //    ui->controls->setGeometry(0,0,ALBUM_SIZE,ALBUM_SIZE);
    ui->controls->setMinimumSize(static_cast<int>(ALBUM_SIZE),static_cast<int>(ALBUM_SIZE));
    ui->controls->setMaximumSize(static_cast<int>(ALBUM_SIZE),static_cast<int>(ALBUM_SIZE));

    auto moveIt= new QAction("Move to left",this);
    moveIt->setShortcut(QKeySequence("Ctrl+m"));
    connect(moveIt, &QAction::triggered,[moveIt,this]()
    {

        if(playlistSta==OUT) emit ui->controls->actions().at(1)->triggered();

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

    auto popPanel = new QAction("Pop panel out",this);
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

    auto hideTimeLabels = new QAction("Hide time labels",this);
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
    ui->controls->setStyleSheet(QString("QWidget#controls{background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(%1,%2,%3,100%),stop:1 rgba(%1,%2,%3,40%))}").arg(QString::number(controlsColor.red()),QString::number(controlsColor.green()),QString::number(controlsColor.blue())));
    ui->miniPlaybackBtn->setVisible(false);

    seekBar = new QSlider(this);

    connect(seekBar,&QSlider::sliderMoved,this,&BabeWindow::on_seekBar_sliderMoved);

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

    auto refreshIt = new QAction("Calibrate...",this);
    refreshIt->setToolTip("Clean & play Babe'd tracks");
    connect(refreshIt, &QAction::triggered, [this]() { calibrateMainList(); });
    calibrateBtn_menu->addAction(refreshIt);

    auto clearIt = new QAction("Clear...",this);
    clearIt->setToolTip("Remove unselected tracks");
    connect(clearIt, &QAction::triggered, [this]()
    {
        this->clearMainList();
        if(mainList->rowCount()>0)
        {
            this->mainList->setCurrentCell(current_song_pos,static_cast<int>(Bae::KEY::TITLE));
            this->mainList->getItem(current_song_pos,Bae::KEY::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));
        }
    });

    calibrateBtn_menu->addAction(clearIt);

    auto cleanIt = new QAction("Clean...",this);
    cleanIt->setToolTip("Remove repeated tracks");
    connect(cleanIt, &QAction::triggered, [this]() { mainList->removeRepeated(); /*this->removequeuedTracks();*/ });
    calibrateBtn_menu->addAction(cleanIt);

    auto similarIt = new QAction("Append Similar...",this);
    connect(similarIt, &QAction::triggered, [this]()
    {
        auto results = searchFor(infoTable->getSimilarArtistTags());

        int i = 1;
        if(mainList->rowCount()==0) i=0;

        for(auto track : results)
        {
            mainList->addRowAt(current_song_pos+i,track);
            //            mainList->item(current_song_pos+i,BabeTable::TITLE)->setIcon(QIcon::fromTheme("filename-space-amarok"));
            mainList->colorizeRow({current_song_pos+i},"#000");

            i++;
        }


        //        this->addToPlaylist(searchFor(infoTable->getTags()),true);
    });
    calibrateBtn_menu->addAction(similarIt);

    auto open = new QAction("Open...",this);
    connect(open, &QAction::triggered,this,&BabeWindow::on_open_btn_clicked);
    calibrateBtn_menu->addAction(open);


    playlistWidget_layout->addWidget(album_art, 0,0,Qt::AlignTop);
    playlistWidget_layout->addWidget(ui->controls, 0,0,Qt::AlignTop);
    playlistWidget_layout->addWidget(ui->frame_4,1,0);
    playlistWidget_layout->addWidget(seekBar,2,0);
    playlistWidget_layout->addWidget(ui->frame_5,3,0);
    playlistWidget_layout->addWidget(mainListView,4,0);

}

void BabeWindow::movePanel(const position &pos)
{
    auto position = QWidget::mapToGlobal(leftFrame->pos());

    switch(pos)
    {
    case RIGHT:
        this->mainLayout->removeWidget(rightFrame);
        this->mainLayout->insertWidget(1,rightFrame);
//        this->mainLayout->removeWidget(ui->vLine);
//        this->mainLayout->insertWidget(1,ui->vLine);
        playlistPos=RIGHT;
        break;
    case LEFT:
        this->mainLayout->removeWidget(rightFrame);
        this->mainLayout->insertWidget(0,rightFrame);
//        this->mainLayout->removeWidget(ui->vLine);
//        this->mainLayout->insertWidget(1,ui->vLine);
        playlistPos=LEFT;
        break;
    case OUT:
        if(viewMode != FULLMODE) expand();
        this->mainLayout->removeWidget(rightFrame);
        rightFrame->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        rightFrame->setWindowTitle("Playlist");
        //        rightFrame->setFrameShape(QFrame::NoFrame);
        rightFrame->show();
        rightFrame->setMinimumHeight(static_cast<int>(ALBUM_SIZE)*2);
        rightFrame->window()->setFixedWidth(rightFrame->minimumSizeHint().width());
        rightFrame->window()->move(position.x()+this->size().width()-rightFrame->size().width(),this->pos().y());
//        ui->vLine->setVisible(false);
        playlistSta=OUT;
        break;
    case IN:
        rightFrame->setWindowFlags(Qt::Widget);
        this->mainLayout->insertWidget(playlistPos==RIGHT?1:0,rightFrame);
        rightFrame->setFixedWidth(rightFrame->minimumSizeHint().width());
        rightFrame->setMinimumHeight(0);
//        ui->vLine->setVisible(true);

        rightFrame->show();
        playlistSta=IN;
        break;

    }
}

void BabeWindow::loadStyle()
{
    /*LOAD THE STYLE*/
    //    QFile styleFile(stylePath);
    //    if(styleFile.exists())
    //    {
    //        qDebug()<<"A Babe style file exists";
    //        styleFile.open(QFile::ReadOnly);
    //        QString style(styleFile.readAll());
    //        this->setStyleSheet(style);
    //    }
}

void BabeWindow::setUpRightFrame()
{

    this->playlistWidget->setFixedWidth(static_cast<int>(ALBUM_SIZE));

    auto *rightFrame_layout = new QGridLayout;
    rightFrame_layout->setContentsMargins(0,0,0,0);
    rightFrame_layout->setSpacing(0);
    rightFrame_layout->setMargin(0);

    rightFrame = new QFrame(this);
    rightFrame->setObjectName("rightFrame");
    rightFrame->installEventFilter(this);
    rightFrame->setAcceptDrops(true);
    rightFrame->setLayout(rightFrame_layout);
    rightFrame->setFrameShadow(QFrame::Raised);
    rightFrame->setFrameShape(QFrame::StyledPanel);
    rightFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    //    rightFrame->setMinimumHeight(ALBUM_SIZE*2);
    rightFrame_layout->addWidget(this->playlistWidget,0,0);
    rightFrame_layout->addWidget(ui->frame_6,1,0,1,1);
    rightFrame_layout->addWidget(ui->playlistUtils,2,0,2,1);

    rightFrame->setFixedWidth(rightFrame->minimumSizeHint().width());

}

void BabeWindow::albumDoubleClicked(const Bae::DB &info)
{
    QString artist =info[Bae::KEY::ARTIST];
    QString album = info[Bae::KEY::ALBUM];

    Bae::DB_LIST mapList;

    switch(Bae::albumType(info))
    {
    case TABLE::ARTISTS:
        mapList = connection.getArtistTracks(artist);
        break;
    case TABLE::ALBUMS:
        mapList = connection.getAlbumTracks(album,artist);
        break;
    default: break;
    }

    if(!mapList.isEmpty()) addToPlaylist(mapList,false, APPENDBOTTOM);
}

void BabeWindow::playItNow(const Bae::DB_LIST &list)
{
    if(!list.isEmpty())
    {
        /* if in the list there's a single track, check its position on the mainPlaylist*/
        if(list.size()==1)
        {
            auto it = this->mainList->getAllTableContent().indexOf(list.first());

            if( it!=-1)
                mainList->setCurrentCell(it,static_cast<int>(Bae::KEY::TITLE));
            else
            {
                addToPlaylist(list,false,APPENDBOTTOM);
                mainList->setCurrentCell(mainList->rowCount()-list.size(),static_cast<int>(Bae::KEY::TITLE));
            }

        }else if(list.size()>1)
        {
            addToPlaylist(list,false,APPENDBOTTOM);
            mainList->setCurrentCell(mainList->rowCount()-list.size(),static_cast<int>(Bae::KEY::TITLE));
        }

        this->loadTrack();
    }

}

void BabeWindow::putAlbumOnPlay(const Bae::DB &info)
{
    if(!info.isEmpty())
    {
        QString artist =info[Bae::KEY::ARTIST];
        QString album = info[Bae::KEY::ALBUM];

        if(!artist.isEmpty()||!album.isEmpty())
        {
            qDebug()<<"put on play<<"<<artist<<album;

            Bae::DB_LIST mapList;

            if(album.isEmpty())
                mapList = connection.getArtistTracks(artist);
            else if(!album.isEmpty()&&!artist.isEmpty())
                mapList = connection.getAlbumTracks(album, artist);

            if(!mapList.isEmpty()) this->putOnPlay(mapList);
        }
    }

}

void BabeWindow::putOnPlay(const Bae::DB_LIST &mapList)
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
            mainList->setCurrentCell(current_song_pos,static_cast<int>(Bae::KEY::TITLE));

            loadTrack();
        }
    }
}

void BabeWindow::addToPlayed(const QString &url)
{
    if(connection.check_existance(Bae::TABLEMAP[Bae::TABLE::TRACKS],Bae::KEYMAP[Bae::KEY::URL],url))
    {
        qDebug()<<"Song totally played"<<url;
        connection.playedTrack(url);
    }
}

bool BabeWindow::eventFilter(QObject *object, QEvent *event)
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

            Bae::DB_LIST mapList;
            QList<QUrl> urls = dropEvent->mimeData()->urls();
            qDebug()<< urls;

            if(urls.isEmpty())
            {
                auto info = dropEvent->mimeData()->text();
                auto infoList = info.split("/by/");

                if(infoList.size()==2)
                {
                    auto artist = infoList.at(1).simplified();
                    auto album = infoList.at(0).simplified();

                    mapList = connection.getAlbumTracks(album,artist);

                }else mapList = connection.getArtistTracks(info);


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

void BabeWindow::closeEvent(QCloseEvent* event)
{

    if(this->settings_widget->brainDeamon.isRunning())
    {
        QMessageBox msgBox;
        msgBox.setText("Babe is still collecting important information about your collection.");
        msgBox.setInformativeText("Do you want to quit?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        switch(msgBox.exec())
        {
        case QMessageBox::Yes: break;
        case QMessageBox::No: event->ignore(); return;
        default: event->ignore(); return;
        }

    }

    if(viewMode == FULLMODE )
    {
        this->saveSettings("GEOMETRY",this->geometry(),"MAINWINDOW");
        qDebug()<<"saved geometry: "<<this->geometry();
    }


    this->saveSettings("PLAYLIST",mainList->getTableColumnContent(Bae::KEY::URL),"MAINWINDOW");
    this->saveSettings("PLAYLIST_POS", current_song_pos,"MAINWINDOW");
    this->saveSettings("TOOLBAR", this->iconSize,"MAINWINDOW");
    this->saveSettings("MINIPLAYBACK",miniPlayback,"MAINWINDOW");
    this->saveSettings("PANEL_POS",playlistPos,"MAINWINDOW");
    this->saveSettings("TIME_LABEL",ui->time->isVisible()&&ui->duration->isVisible(),"MAINWINDOW");

    event->accept();

    QMainWindow::closeEvent(event);
}

void BabeWindow::resizeEvent(QResizeEvent* event)
{
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

void BabeWindow::refreshTables(const QMap<Bae::TABLE, bool> &tableReset) //tofix
{
    nof->notify("Loading collection","this might take some time depending on your colleciton size");

    QSqlQuery query;

    for (auto table : tableReset.keys())
        switch(table)
        {
        case Bae::TABLE::TRACKS:
        {
            if(tableReset[table]) collectionTable->flushTable();
            qDebug()<<"popullllll tracks";
            query.prepare(QString("SELECT * FROM %1 ORDER BY  %2").arg(Bae::TABLEMAP[Bae::TABLE::TRACKS],Bae::KEYMAP[Bae::KEY::ARTIST]));
            collectionTable->populateTableView(query);
            break;

        }
        case Bae::TABLE::ALBUMS:
        {
            if(tableReset[table]) albumsTable->flushView();

            query.prepare(QString("SELECT * FROM %1 ORDER BY  %2").arg(Bae::TABLEMAP[Bae::TABLE::ALBUMS],Bae::KEYMAP[Bae::KEY::ALBUM]));
            albumsTable->populate(query);
            albumsTable->hideAlbumFrame();
            break;
        }
        case Bae::TABLE::ARTISTS:
        {
            if(tableReset[table]) artistsTable->flushView();


            query.prepare(QString("SELECT * FROM %1 ORDER BY  %2").arg(Bae::TABLEMAP[Bae::TABLE::ARTISTS],Bae::KEYMAP[Bae::KEY::ARTIST]));
            artistsTable->populate(query);
            artistsTable->hideAlbumFrame();
            break;
        }
        case Bae::TABLE::PLAYLISTS:
        {
            playlistTable->list->clear();
            playlistTable->setDefaultPlaylists();
            playlistTable->setPlaylists(connection.getPlaylists());
            break;
        }

        default: return;
        }

    emit finishRefresh();

}

void BabeWindow::keyPressEvent(QKeyEvent *event) //todo
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

void BabeWindow::enterEvent(QEvent *event)
{
    event->accept();
}

void BabeWindow::leaveEvent(QEvent *event)
{
    event->accept();
}

void BabeWindow::showControls(const bool &state)
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

void BabeWindow::dummy() { qDebug()<<"TEST on DUMMYT"; }

void BabeWindow::setCoverArt(const Bae::DB &song)
{
    //    qDebug()<<"Trying to retieve the cover art from Pulpo for"<< title << artist << album;
    //    Pulpo coverArt(song);
    //    connect(&coverArt,&Pulpo::albumArtReady,this,&MainWindow::putPixmap);
    //    if (coverArt.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::LastFm)) qDebug()<<"using lastfm";
    //    else if(coverArt.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::Spotify)) qDebug()<<"using spotify";
    //    else if(coverArt.fetchAlbumInfo(Pulpo::AlbumArt,Pulpo::GeniusInfo)) qDebug()<<"using genius";
    //    else coverArt.albumArtReady(QByteArray());
}

void BabeWindow::setToolbarIconSize(const uint &iconSize) //tofix
{

    this->iconSize = iconSize;

    //    for(auto obj: ui->controls->children())
    //        if(qobject_cast<QToolButton *>(obj)!=NULL)
    //            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(iconSize,iconSize));

    for(auto obj: ui->playlistUtils->children())
        if(qobject_cast<QToolButton *>(obj)!=nullptr)
            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(static_cast<int>(iconSize),static_cast<int>(iconSize)));

    //    for(auto obj: ui->collectionUtils->children())
    //        if(qobject_cast<QToolButton *>(obj)!=NULL)
    //            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(iconSize,iconSize));

    for(auto obj: ui->viewsUtils->children())
        if(qobject_cast<QToolButton *>(obj)!=nullptr)
            qobject_cast<QToolButton *>(obj)->setIconSize(QSize(static_cast<int>(iconSize),static_cast<int>(iconSize)));

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

void BabeWindow::collectionView()
{

    if(resultsTable->rowCount()>0) views->setCurrentIndex(RESULTS);
    else views->setCurrentIndex(COLLECTION);

    ui->tracks_view->setChecked(true);

    if(this->viewMode != FULLMODE) expand();

    ui->tracks_view->setChecked(true);
    prevIndex=views->currentIndex();
}

void BabeWindow::albumsView()
{
    views->setCurrentIndex(ALBUMS);
    ui->albums_view->setChecked(true);

    if(this->viewMode != FULLMODE) expand();


    prevIndex = views->currentIndex();
}

void BabeWindow::playlistsView()
{
    views->setCurrentIndex(PLAYLISTS);
    ui->playlists_view->setChecked(true);
    if(this->viewMode != FULLMODE) expand();


    if(this->playlistTable->list->currentRow()==0)
    {
        this->playlistTable->list->setCurrentRow(0);
        emit this->playlistTable->list->clicked(this->playlistTable->list->model()->index(0,0));
    }

    prevIndex = views->currentIndex();
}

void BabeWindow::rabbitView()
{
    views->setCurrentIndex(RABBIT);
    ui->rabbit_view->setChecked(true);
    if(this->viewMode != FULLMODE) expand();

    prevIndex = views->currentIndex();
}

void BabeWindow::infoView()
{
    views->setCurrentIndex(INFO);
    ui->info_view->setChecked(true);

    if(this->viewMode != FULLMODE) expand();


    prevIndex = views->currentIndex();
}

void BabeWindow::artistsView()
{
    views->setCurrentIndex(ARTISTS);
    ui->artists_view->setChecked(true);
    if(this->viewMode != FULLMODE) expand();

    prevIndex = views->currentIndex();
}


void BabeWindow::settingsView()
{
    views->setCurrentIndex(SETTINGS);
    ui->settings_view->setChecked(true);
    if(this->viewMode != FULLMODE) expand();

    prevIndex = views->currentIndex();
}

void BabeWindow::expand()
{

    this->setMinimumSize(this->minimumSizeHint().width(),0);

    this->viewMode=FULLMODE;

    if(album_art->getSize()!=ALBUM_SIZE)
    {
        album_art->setSize(ALBUM_SIZE);
        //        this->seekBar->setMaximumWidth(ALBUM_SIZE);
        ui->controls->setMaximumSize(static_cast<int>(ALBUM_SIZE),static_cast<int>(ALBUM_SIZE));
        ui->controls->setMinimumSize(static_cast<int>(ALBUM_SIZE),static_cast<int>(ALBUM_SIZE));

    }

    ui->tracks_view_2->setVisible(false);
//    ui->vLine->setVisible(true);
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
    this->setMinimumSize(this->minimumSizeHint().width(),0);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry",this);
    animation->setDuration(200);
    animation->setStartValue(this->geometry());
    animation->setEndValue(this->loadSettings("GEOMETRY","MAINWINDOW",this->geometry()).toRect());

    animation->start();

    ui->hide_sidebar_btn->setToolTip("Go Mini");

    //    this->setWindowFlags(this->windowFlags() & ~Qt::Tool & ~Qt::FramelessWindowHint & ~Qt::WindowStaysOnTopHint);
    //    this->show();

}

void BabeWindow::go_mini()
{
    this->setMinimumSize(this->minimumSizeHint().width(),0);

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
    QPropertyAnimation *animation = new QPropertyAnimation(this, "maximumHeight",this);
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

void BabeWindow::go_playlistMode()
{

    this->setMinimumSize(this->minimumSizeHint().width(),0);
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
        case INFO: icon = ui->info_view->icon().name(); break;

        case SETTINGS:  icon= ui->settings_view->icon().name(); break;
        default:  icon="go-back";
        }
        ui->tracks_view_2->setIcon(QIcon::fromTheme(icon));

//        ui->vLine->setVisible(false);
        if(!ui->frame_4->isVisible()) ui->frame_4->setVisible(true);
        if(!mainList->isVisible()) mainListView->setVisible(true);
        if(!ui->frame_5->isVisible()) ui->frame_5->setVisible(true);
        if(!ui->frame_6->isVisible()) ui->frame_6->setVisible(true);
        if(!ui->playlistUtils->isVisible()) ui->playlistUtils->setVisible(true);
        ui->tracks_view_2->setVisible(true);
        //        album_art->borderColor=false;
        this->setMaximumWidth(rightFrame->minimumSizeHint().width());
        this->setMinimumWidth(rightFrame->minimumSizeHint().width());
        QPropertyAnimation *animation = new QPropertyAnimation(this, "maximumWidth",this);
        animation->setDuration(200);
        animation->setStartValue(this->size().width());
        animation->setEndValue(rightFrame->minimumSizeHint().width());

        animation->start();

        leftFrame->setVisible(false);

        ui->hide_sidebar_btn->setToolTip("Go Mini");
    }


}

void BabeWindow::keepOnTop(bool state)
{
    if (state) this->setWindowFlags(Qt::WindowStaysOnTopHint);
    else this->setWindowFlags(defaultWindowFlags);

    this->show();
}





void BabeWindow::on_hide_sidebar_btn_clicked()
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

void BabeWindow::on_shuffle_btn_clicked() //tofix
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

void BabeWindow::on_open_btn_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),QDir().homePath()+"/Music/", tr("Audio (*.mp3 *.wav *.mp4 *.flac *.ogg *.m4a)"));
    if(!files.isEmpty()) appendFiles(files);

}

void BabeWindow::appendFiles(const QStringList &paths,const appendPos &pos)
{
    if(!paths.isEmpty())
    {
        QStringList trackList;
        for( auto url  : paths)
        {
            if(QFileInfo(url).isDir())
            {
                QDirIterator it(url, Bae::formats, QDir::Files, QDirIterator::Subdirectories);

                while (it.hasNext()) trackList<<it.next();

            }else if (QFileInfo(url).isFile())
                trackList<<url;
        }

        Tracklist tracks;
        tracks.add(trackList);
        addToPlaylist(tracks.getTracks(),false,pos);
    }
}

void BabeWindow::populateMainList()
{
    auto results = connection.getBabedTracks();
    mainList->populateTableView(results);
    mainList->resizeRowsToContents();
    currentList = mainList->getAllTableContent();
}

void BabeWindow::updateList()
{
    mainList->flushTable();
    for(auto list: currentList)
        mainList->addRow(list);
}


void BabeWindow::on_mainList_clicked(const Bae::DB_LIST &list)
{
    Q_UNUSED(list);
    loadTrack();
}

void BabeWindow::removeSong(const int &index)/*tofix*/
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

void BabeWindow::expandAlbumList(const QString &artist)
{
    if(!artist.isEmpty())
    {
        emit ui->artists_view->clicked();
        artistsTable->showAlbumInfo({{Bae::KEY::ARTIST,artist}});
    }

}

void BabeWindow::loadTrack()
{
    if(stopped) updater->start(100);

    prev_song = current_song;
    prev_song_pos = current_song_pos;

    if(prev_song_pos<this->mainList->rowCount() && mainList->getItem(current_song_pos,Bae::KEY::TITLE)->icon().name()!="clock")
        mainList->getItem(prev_song_pos,Bae::KEY::TITLE)->setIcon(QIcon());

    calibrateBtn_menu->actions().at(3)->setEnabled(false);

    current_song_pos = mainList->getIndex();
    current_song = mainList->getRowData(current_song_pos);

    if(mainList->getItem(current_song_pos,Bae::KEY::TITLE)->icon().name()!="clock")
        mainList->getItem(current_song_pos,Bae::KEY::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));

    mainList->scrollTo(mainList->model()->index(current_song_pos,static_cast<int>(Bae::KEY::TITLE)));

    qDebug()<<"in mainlist="<<current_song[Bae::KEY::URL];

    if(Bae::fileExists(current_song[Bae::KEY::URL]))
    {
        player->setMedia(QUrl::fromLocalFile(current_song[Bae::KEY::URL]));

        this->play();

        album_art->setTitle(current_song[Bae::KEY::ARTIST],current_song[Bae::KEY::ALBUM]);

        //CHECK IF THE SONG IS BABED IT
        if(isBabed(current_song)) babedIcon(true);
        else babedIcon(false);

        loadMood();

        loadCover(current_song);

        if(!this->isActiveWindow())
            nof->notifySong(current_song,QPixmap(current_song[Bae::KEY::ARTWORK]));

        if(miniPlayback)
        {
            this->blurWidget(*album_art,28);
            album_art->saturatePixmap(100);
        }

        loadInfo(current_song);
        this->rabbitTable->seed(current_song);

    }else removeSong(current_song_pos);

}

int BabeWindow::isBabed(const Bae::DB &track)
{
    return connection.getTrackBabe(track[Bae::KEY::URL]);
}

void BabeWindow::loadMood()
{
    auto color = this->connection.getTrackArt(current_song[Bae::KEY::URL]);

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


bool BabeWindow::loadCover(DB &track) //tofix separte getalbumcover from get artisthead
{
    auto artist = track[Bae::KEY::ARTIST];
    auto album = track[Bae::KEY::ALBUM];
    auto title = track[Bae::KEY::TITLE];

    QString artistHead = this->connection.getArtistArt(artist);

    if(!artistHead.isEmpty())
    {
        infoTable->setArtistArt(artistHead);
        infoTable->artist->setArtist(artist);

    }else infoTable->setArtistArt(QString(":Data/data/cover.svg"));

    this->current_song.insert(Bae::KEY::ARTWORK,this->connection.getAlbumArt(album, artist));

    if(!current_song[Bae::KEY::ARTWORK].isEmpty())
        this->album_art->putPixmap(current_song[Bae::KEY::ARTWORK]);
    else if (!artistHead.isEmpty())
    {
        this->current_song.insert(Bae::KEY::ARTWORK,artistHead);
        this->album_art->putPixmap(current_song[Bae::KEY::ARTWORK]);
    }
    else{
        emit fetchCover(track);
        return false;
    }

    return true;
}

void BabeWindow::addToQueue(const Bae::DB_LIST &tracks)
{
    prev_queued_song_pos=current_song_pos;

    QStringList queuedList;
    for(auto track : tracks)
    {
        if(!queued_songs.contains(track[Bae::KEY::URL]))
        {
            mainList->addRowAt(queued_songs.size(),track);
            mainList->getItem(queued_songs.size(),Bae::KEY::TITLE)->setIcon(QIcon::fromTheme("clock"));
            mainList->colorizeRow({queued_songs.size()},"#333");
            queued_songs.insert(track[Bae::KEY::URL],track);
            queuedList<<track[Bae::KEY::TITLE]+" by "+track[Bae::KEY::ARTIST];
            current_song_pos++;
        }

    }
    //mainList->addRowAt(current_song_pos+1,track,true);
    qDebug()<<"saving track pos to restore after queued is empty"<<prev_queued_song_pos;

    mainList->scrollToItem(mainList->getItem(0,Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

    nof->notify("Song added to Queue",queuedList.join("\n"));

}

void BabeWindow::on_seekBar_sliderMoved(const int &position)
{
    player->setPosition(player->duration() / 1000 * position);
}


void BabeWindow::update()
{

    if(mainList->rowCount()==0) stop();

    if(!current_song.isEmpty())
    {
        if(!seekBar->isEnabled()) seekBar->setEnabled(true);

        if(!seekBar->isSliderDown())
            seekBar->setValue(static_cast<int>(static_cast<double>(player->position())/player->duration()*1000));
        ui->time->setText(Bae::transformTime(player->position()/1000));
        ui->duration->setText(Bae::transformTime(player->duration()/1000));

        //        QToolTip::showText( seekBar->mapToGlobal( QPoint( 0, 0 ) ),this->transformTime(player->position()/1000) );
        if(player->state() == QMediaPlayer::StoppedState)
        {
            if(!queued_songs.isEmpty()) removeQueuedTrack(current_song,current_song_pos);

            prev_song = current_song;
            qDebug()<<"finished playing song: "<<prev_song[Bae::KEY::URL];

            emit finishedPlayingSong(prev_song[Bae::KEY::URL]);
            next();
        }


    }else
    {
        //        qDebug()<<"no song to play";
        seekBar->setValue(0);
        seekBar->setEnabled(false);
    }
}


bool BabeWindow::removeQueuedTrack(const Bae::DB &track, const int &pos)
{
    if(queued_songs.contains(track[Bae::KEY::URL]))
        if(mainList->getItem(pos,Bae::KEY::TITLE)->icon().name()=="clock")
        {
            mainList->removeRow(pos);
            queued_songs.remove(track[Bae::KEY::URL]);
            return true;
        }

    return false;
}

void BabeWindow::removequeuedTracks()
{
    QList<QMap<int, QString>> newList;

    for(auto row=0;row<this->mainList->rowCount();row++)

        if(mainList->getItem(row,Bae::KEY::TITLE)->icon().name()=="clock")
        {
            mainList->removeRow(row);
            queued_songs.remove(mainList->getRowData(row)[Bae::KEY::URL]);
            current_song_pos--;
        }

}


void BabeWindow::next()
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

    mainList->setCurrentCell((shuffle&&queued_songs.isEmpty()) ? shuffleNumber():nextSong, static_cast<int>(Bae::KEY::TITLE));

    loadTrack();
}


void BabeWindow::back()
{
    auto lCounter = current_song_pos-1;

    if(lCounter < 0)
        lCounter = mainList->rowCount() - 1;

    mainList->setCurrentCell(!shuffle ? lCounter : shuffleNumber(), static_cast<int>(Bae::KEY::TITLE));

    loadTrack();
}

int BabeWindow::shuffleNumber()
{
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0,mainList->rowCount()-1); // guaranteed unbiased

    auto random_integer = uni(rng);
    qDebug()<<"random number:"<<random_integer;
    if (current_song_pos !=random_integer)return random_integer;
    else return random_integer+1;
}

void BabeWindow::on_play_btn_clicked()
{
    if(mainList->rowCount() > 0 || !current_song.isEmpty())
    {
        if(player->state() == QMediaPlayer::PlayingState) this->pause();
        else this->play();

    }
}

void BabeWindow::play()
{
    player->play();
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));
    this->setWindowTitle(current_song[Bae::KEY::TITLE]+" \xe2\x99\xa1 "+current_song[Bae::KEY::ARTIST]);
}

void BabeWindow::pause()
{
    player->pause();
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-start"));
}

void BabeWindow::stop()
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

void BabeWindow::on_backward_btn_clicked()
{
    if(mainList->rowCount() > 0)
    {
        if(player->position() > 3000) player->setPosition(0);
        else back();
    }
}

void BabeWindow::on_foward_btn_clicked()
{
    if(mainList->rowCount() > 0) next();
}






void BabeWindow::babedIcon(const bool &state)
{
    auto effect = new QGraphicsColorizeEffect(this);
    effect->setColor(QColor("#dc042c"));

    if(state)  effect->setStrength(1.0);
    else effect->setStrength(0);

    ui->fav_btn->setGraphicsEffect(effect);
}

void BabeWindow::on_fav_btn_clicked()
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

void BabeWindow::babeAlbum(const Bae::DB &info)
{
    auto artist =info[Bae::KEY::ARTIST];
    auto album = info[Bae::KEY::ALBUM];

    Bae::DB_LIST mapList;
    if(album.isEmpty())
        mapList = connection.getArtistTracks(artist);
    else if(!artist.isEmpty())
        mapList = connection.getAlbumTracks(album, artist);

    if(!mapList.isEmpty())
        babeIt(mapList);

}

bool BabeWindow::unbabeIt(const Bae::DB &track)
{
    if(connection.babeTrack(track[Bae::KEY::URL],0))
    {
        nof->notify("Song unBabe'd it",track[Bae::KEY::TITLE]+" by "+track[Bae::KEY::ARTIST]);
        return  true;
    } else return false;

}

bool BabeWindow::babeTrack(const Bae::DB &track)
{

    QString url = track[Bae::KEY::URL];
    if(isBabed(track))
    {
        if(unbabeIt(track)) return true;
        else return false;

    }else
    {

        if(this->connection.check_existance(Bae::TABLEMAP[Bae::TABLE::TRACKS],Bae::KEYMAP[Bae::KEY::URL],url))
        {
            if(this->connection.babeTrack(url,true))
            {
                nof->notify("Song Babe'd it",track[Bae::KEY::TITLE]+" by "+track[Bae::KEY::ARTIST]);
                addToPlaylist({track},true,APPENDBOTTOM);
                return true;
            } else return false;

        }else
        {
            ui->fav_btn->setEnabled(false);
            auto newTrack = track;
            newTrack.insert(Bae::KEY::BABE, "1");
            connection.addTrack(newTrack);

            ui->fav_btn->setEnabled(true);
            return true;
        }

    }

}

void BabeWindow::loadInfo(const Bae::DB &track)
{
    auto lyrics = this->connection.getTrackLyrics(track[Bae::KEY::URL]);
    auto albumTags = this->connection.getAlbumTags(track[Bae::KEY::ALBUM],track[Bae::KEY::ARTIST]);
    auto albumWiki = this->connection.getAlbumWiki(track[Bae::KEY::ALBUM],track[Bae::KEY::ARTIST]);
    auto artistWiki = this->connection.getArtistWiki(track[Bae::KEY::ARTIST]);
    auto artistTags = this->connection.getArtistTags(track[Bae::KEY::ARTIST]);


    this->infoTable->setTrack(track);

    if(!lyrics.isEmpty())
        this->infoTable->setLyrics(lyrics);

    if(!albumWiki.isEmpty())
        this->infoTable->setAlbumInfo(albumWiki);

    if(!artistWiki.isEmpty())
        this->infoTable->setArtistInfo(artistWiki);

    if(!artistWiki.isEmpty())
        this->infoTable->setArtistTags(artistTags);

    if(!albumTags.isEmpty())
        this->infoTable->setAlbumTags(albumTags);


}

void BabeWindow::babeIt(const Bae::DB_LIST &tracks)
{
    for(auto track : tracks)
        if(!babeTrack(track)) qDebug()<<"couldn't Babe track:"<<track[Bae::KEY::URL];

}

void  BabeWindow::infoIt(const Bae::DB &track)
{
    //views->setCurrentIndex(INFO);
    infoView();
    this->loadInfo(track);
}


void BabeWindow::addToPlaylist(const Bae::DB_LIST &mapList, const bool &notRepeated, const appendPos &pos)
{
    qDebug()<<"Adding mapList to mainPlaylist";

    if(!mapList.isEmpty())
    {
        emit ui->filter->textChanged("");
        if(notRepeated)
        {
            Bae::DB_LIST newList;
            QStringList alreadyInList = mainList->getTableColumnContent(Bae::KEY::URL);
            for(auto track: mapList)
            {
                if(!alreadyInList.contains(track[Bae::KEY::URL]))
                {
                    newList<<track;
                    switch(pos)
                    {
                    case APPENDBOTTOM:
                        mainList->addRow(track);
                        mainList->scrollToBottom();

                        break;
                    case APPENDTOP:
                        mainList->addRowAt(0,track);
                        mainList->scrollToItem(mainList->getItem(0,Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

                        break;
                    case APPENDAFTER:
                        mainList->addRowAt(current_song_pos+1,track);
                        mainList->scrollToItem(mainList->getItem(current_song_pos+1,Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

                        break;
                    case APPENDBEFORE:
                        mainList->addRowAt(current_song_pos,track);
                        mainList->scrollToItem(mainList->getItem(current_song_pos,Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

                        break;
                    case APPENDINDEX:
                        mainList->addRowAt(mainList->getIndex(),track);
                        mainList->scrollToItem(mainList->getItem(mainList->getIndex(),Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

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
                    mainList->addRow(track);
                    mainList->scrollToBottom();

                    break;
                case APPENDTOP:
                    mainList->addRowAt(0,track);
                    mainList->scrollToItem(mainList->getItem(0,Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

                    break;
                case APPENDAFTER:
                    mainList->addRowAt(current_song_pos+1,track);
                    mainList->scrollToItem(mainList->getItem(current_song_pos+1,Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

                    break;
                case APPENDBEFORE:
                    mainList->addRowAt(current_song_pos,track);
                    mainList->scrollToItem(mainList->getItem(current_song_pos,Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

                    break;
                case APPENDINDEX:
                    mainList->addRowAt(mainList->getIndex(),track);
                    mainList->scrollToItem(mainList->getItem(mainList->getIndex(),Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

                    break;
                }
            currentList+=mapList;

        }

        if(stopped)
        {
            mainList->setCurrentCell(0,static_cast<int>(Bae::KEY::TITLE));
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


void  BabeWindow::clearCurrentList()
{
    currentList.clear();
    mainList->flushTable();
}

void BabeWindow::runSearch()
{
    if(!ui->search->text().isEmpty())
    {
        qDebug()<<"Looking for";
        QStringList searchList=ui->search->text().split(",");
        auto searchResults = searchFor(searchList);
        resultsTable->flushTable();
        if(!searchResults.isEmpty())
        {
            albumsTable->filter(searchResults,Bae::KEY::ALBUM);
            artistsTable->filter(searchResults,Bae::KEY::ARTIST);
            populateResultsTable(searchResults);
        }else
        {
            this->resultsTable->setAddMusicMsg("Nothing on: \n"+ui->search->text());
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

void BabeWindow::populateResultsTable(const Bae::DB_LIST &mapList)
{
    if(views->currentIndex()!=ALBUMS&&views->currentIndex()!=ARTISTS)
        views->setCurrentIndex(RESULTS);
    resultsTable->populateTableView(mapList);
}

Bae::DB_LIST BabeWindow::searchFor(const QStringList &queries)
{
    Bae::DB_LIST mapList;
    bool hasKey=false;

    for(auto searchQuery : queries)
    {
        if(searchQuery.contains(Bae::SearchTMap[Bae::SearchT::LIKE]+":") || searchQuery.startsWith("#"))
        {
            if(searchQuery.startsWith("#"))
                searchQuery=searchQuery.replace("#","").trimmed();
            else
                searchQuery=searchQuery.replace(Bae::SearchTMap[Bae::SearchT::LIKE]+":","").trimmed();


            searchQuery=searchQuery.trimmed();
            if(!searchQuery.isEmpty())
            {
                mapList += this->connection.getSearchedTracks(Bae::KEY::WIKI,searchQuery);
                mapList += this->connection.getSearchedTracks(Bae::KEY::TAG,searchQuery);
                mapList += this->connection.getSearchedTracks(Bae::KEY::LYRICS,searchQuery);
            }

        }else if(searchQuery.contains((Bae::SearchTMap[Bae::SearchT::SIMILAR]+":")))
        {
            searchQuery=searchQuery.replace(Bae::SearchTMap[Bae::SearchT::SIMILAR]+":","").trimmed();
            searchQuery=searchQuery.trimmed();
            if(!searchQuery.isEmpty())
                mapList += this->connection.getSearchedTracks(Bae::KEY::TAG,searchQuery);

        }else
        {
            Bae::KEY key;

            QMapIterator<Bae::KEY, QString> k(Bae::KEYMAP);
            while (k.hasNext())
            {
                k.next();
                if(searchQuery.contains(QString(k.value()+":")))
                {
                    hasKey=true;
                    key=k.key();
                    searchQuery=searchQuery.replace(k.value()+":","").trimmed();
                }
            }

            searchQuery=searchQuery.trimmed();
            qDebug()<<"Searching for: "<<searchQuery;

            if(!searchQuery.isEmpty())
            {
                if(hasKey)
                    mapList += this->connection.getSearchedTracks(key,searchQuery);
                else
                {
                    auto queryTxt = "SELECT * FROM tracks WHERE title LIKE \"%"+searchQuery+"%\" OR artist LIKE \"%"+searchQuery+"%\" OR album LIKE \"%"+searchQuery+"%\"OR genre LIKE \"%"+searchQuery+"%\"OR url LIKE \"%"+searchQuery+"%\"";
                    QSqlQuery query(queryTxt);
                    mapList += this->connection.getDBData(query);
                }
            }
        }

    }

    return mapList;
}



void BabeWindow::on_rowInserted(QModelIndex model ,int x,int y)
{
    Q_UNUSED(model);
    Q_UNUSED(y);
    mainList->scrollTo(mainList->model()->index(x,static_cast<int>(Bae::KEY::TITLE)),QAbstractItemView::PositionAtCenter);

}


void BabeWindow::clearMainList()
{

    Bae::DB_LIST mapList;
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

void BabeWindow::on_tracks_view_2_clicked()
{
    expand();
}

void BabeWindow::on_addAll_clicked()
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
    case RESULTS:
        addToPlaylist(resultsTable->getAllTableContent(),false, APPENDBOTTOM); break;
    }

}


void BabeWindow::saveResultsTo()
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

void BabeWindow::on_filterBtn_clicked()
{
    if(ui->filterBtn->isChecked())
    {
        ui->filterBtn->setChecked(true);
        ui->filterBox->setVisible(true);
        ui->calibrateBtn->setVisible(false);
        ui->rabbit_view->setVisible(false);
        if(ui->tracks_view_2->isVisible()) ui->tracks_view_2->setVisible(false);
        mainListView->setCurrentIndex(FILTERLIST);
        ui->filter->setFocus();
    }else
    {
        ui->filterBtn->setChecked(false);
        ui->filterBox->setVisible(false);
        ui->calibrateBtn->setVisible(true);
        ui->rabbit_view->setVisible(true);

        if(!ui->tracks_view_2->isVisible() && viewMode==PLAYLISTMODE)ui->tracks_view_2->setVisible(true);
        mainListView->setCurrentIndex(MAINPLAYLIST);

    }

}

void BabeWindow::on_filter_textChanged(const QString &arg1)
{

    QString query = arg1;
    if(!query.isEmpty())
    {
        mainListView->setCurrentIndex(FILTERLIST);

        QStringList searchList=query.split(",");

        auto searchResults = searchFor(searchList);
        filterList->flushTable();
        if(!searchResults.isEmpty())
        {
            qDebug()<<"GOT SEARCH RESULTS";
            filterList->populateTableView(searchResults);
        }



    }else
    {
        filterList->flushTable();
        ui->filterBtn->setChecked(false);
        emit  ui->filterBtn->clicked();
        ui->filterBox->setVisible(false);
        mainListView->setCurrentIndex(MAINPLAYLIST);
    }



}

void BabeWindow::calibrateMainList()
{
    clearMainList();
    populateMainList();
    currentList = mainList->getAllTableContent();
    mainList->scrollToTop();

    if(mainList->rowCount()>0)
    {
        this->mainList->setCurrentCell(current_song_pos,static_cast<int>(Bae::KEY::TITLE));
        this->mainList->getItem(current_song_pos,Bae::KEY::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));
        this->mainList->removeRepeated();
    }
}

void BabeWindow::on_playAll_clicked()
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
    case RESULTS:
        putOnPlay(resultsTable->getAllTableContent()); break;
    }
}

void BabeWindow::on_miniPlaybackBtn_clicked()
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

        if(viewMode==MINIMODE)  this->setMaximumHeight(ui->controls->minimumSizeHint().height()-40);


    }else
    {
        ui->controls->setFixedHeight(static_cast<int>(ALBUM_SIZE));
        album_art->setFixedHeight(static_cast<int>(ALBUM_SIZE));
        ui->miniPlaybackBtn->setIcon(QIcon::fromTheme("go-top"));
        miniPlayback=!miniPlayback;
        this->blurWidget(*album_art,15);
        album_art->restoreSaturation();

        if(viewMode==MINIMODE) this->setMaximumHeight(ui->controls->height());


    }

    ui->controls->update();
}


void BabeWindow::blurWidget(BabeAlbum &widget, const int &radius )
{
    QGraphicsBlurEffect* effect	= new QGraphicsBlurEffect(this);

    effect->setBlurRadius(radius);

    widget.setGraphicsEffect(effect);

}
