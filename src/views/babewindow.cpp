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

BabeWindow::BabeWindow(const QStringList &files, QWidget *parent) : QMainWindow(parent),
    ui(new Ui::BabeWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    this->setWindowIcon(QIcon(":Data/data/48-apps-babe.svg"));
    this->setWindowIconText("Babe...");
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->defaultWindowFlags = this->windowFlags();
    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    this->player = new QMediaPlayer(this);
    this->player->setVolume(100);

    //* SETUP MAIN COVER ARTWORK *//
    this->album_art = new BabeAlbum(Bae::DB{{Bae::KEY::ARTWORK, ":Data/data/babe.png"}}, Bae::AlbumSizeHint::BIG_ALBUM, 0, false, this);
    connect(this->album_art, &BabeAlbum::playAlbum, this, &BabeWindow::putAlbumOnPlay);
    connect(this->album_art, &BabeAlbum::babeAlbum, this, &BabeWindow::babeAlbum);

    this->ALBUM_SIZE = album_art->getSize();

    this->album_art->setFixedSize(static_cast<int>(ALBUM_SIZE), static_cast<int>(ALBUM_SIZE));
    this->album_art->setTitleGeometry(0, 0, static_cast<int>(ALBUM_SIZE), static_cast<int>(ALBUM_SIZE*0.15));
    this->album_art->showTitle(false);
    this->album_art->showPlayBtn = false;


    this->setMinimumSize(this->minimumSizeHint().width(),0);
    this->defaultGeometry = (QStyle::alignedRect(
                                 Qt::LeftToRight,
                                 Qt::AlignCenter,
                                 qApp->desktop()->availableGeometry().size()*0.7,
                                 qApp->desktop()->availableGeometry()
                                 ));

    this->setGeometry( Bae::loadSettings("GEOMETRY", "MAINWINDOW", defaultGeometry).toRect());
    Bae::saveSettings("GEOMETRY", this->geometry(), "MAINWINDOW");

    connect(this, &BabeWindow::finishedPlayingSong, this, &BabeWindow::addToPlayed);
    connect(this, &BabeWindow::collectionChecked, this,&BabeWindow::refreshTables);


    //* SETUP BABE PARTS *//
    this->setUpViews();
    this->setUpPlaylist();
    this->setUpRightFrame();
    this->setUpCollectionViewer();
    this->setUpMenuBar();

    this->nof = new Notify(this);
    connect(nof,&Notify::babeSong,[this](const Bae::DB &track)
    {
        if(this->babeTrack(track))
            this->babedIcon(this->isBabed(track));

    });
    connect(this->nof,&Notify::skipSong,this,&BabeWindow::next);

    connect(this->updater, &QTimer::timeout, this, &BabeWindow::update);

    //* LOAD OPENED FILES*/
    if(!files.isEmpty())
    {
        this->appendFiles(files, APPENDTOP);
        this->current_song_pos = 0;
    }

    if(Bae::loadSettings("MINIPLAYBACK","MAINWINDOW",false).toBool())
        emit ui->miniPlaybackBtn->clicked();

    this->movePanel(static_cast<POSITION>(Bae::loadSettings("PANEL_POS","MAINWINDOW", RIGHT).toInt()));
    this->setToolbarIconSize(static_cast<uint>(Bae::loadSettings("TOOLBAR_ICON_SIZE", "MAINWINDOW", QVariant(16)).toInt()));

    this->loadStyle();
}


BabeWindow::~BabeWindow()
{
    qDebug()<<"DELETING BABEWINDOW";
    this->connection.closeConnection();
    delete ui;
}

void BabeWindow::start()
{
    this->current_song_pos = Bae::loadSettings("PLAYLIST_POS","MAINWINDOW",QVariant(0)).toInt();

    //* CHECK FOR DATABASE *//
    if(this->settings_widget->checkCollection())
    {
        auto savedList = Bae::loadSettings("PLAYLIST","MAINWINDOW",{}).toStringList();

        if(!savedList.isEmpty())
        {
            this->addToPlaylist(connection.getDBData(savedList), false, APPENDBOTTOM);

        }else this->populateMainList();

        emit collectionChecked({{TABLE::TRACKS, true}, {TABLE::ALBUMS, true}, {TABLE::ARTISTS, true}, {TABLE::PLAYLISTS, true}});

    }else this->settings_widget->createCollectionDB();

    if(this->mainList->rowCount() > 0)
    {
        this->mainList->setCurrentCell(this->current_song_pos >= this->mainList->rowCount()? 0 : this->current_song_pos, static_cast<int>(Bae::KEY::TITLE));
        this->collectionView();
        this->go_playlistMode();

    }else if(this->collectionTable->rowCount() > 0)
        this->collectionView();

    else
    {
        this->stop();
        this->settingsView();
    }
}

//*HERE THE MAIN VIEWS GET SETUP WITH THEIR SIGNALS AND SLOTS**//
void BabeWindow::setUpViews()
{
    this->settings_widget = new settings(this); //this needs to go first
    this->settings_widget->setToolbarIconSize(Bae::loadSettings("TOOLBAR_ICON_SIZE", "MAINWINDOW", QVariant(16)).toInt());
    this->settings_widget->setToolbarPosition(static_cast<Qt::ToolBarArea>(Bae::loadSettings("TOOLBAR_POS", "MAINWINDOW", Qt::ToolBarArea::LeftToolBarArea).toInt()));
    connect(&connection, &CollectionDB::trackInserted, [this]()
    {
        this->settings_widget->collectionWatcher();
        emit this->settings_widget->refreshTables({{TABLE::TRACKS, true},{TABLE::ALBUMS, false},{TABLE::ARTISTS, false},{TABLE::PLAYLISTS, true}});
        emit this->settings_widget->getArtwork();
    });

    this->playlistTable = new PlaylistsView(this);
    connect(this->playlistTable->table, &BabeTable::tableWidget_doubleClicked, [this] (const Bae::DB_LIST &list) { this->addToPlaylist(list,false,APPENDBOTTOM);});
    connect(this->playlistTable->table, &BabeTable::removeIt_clicked, this, &BabeWindow::removeSong);
    connect(this->playlistTable->table, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->playlistTable->table, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->playlistTable->table, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->playlistTable->table, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->playlistTable->table, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->playlistTable->table, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->playlistTable->table, &BabeTable::appendIt, [this] (const Bae::DB_LIST &list) { this->addToPlaylist(list, false, APPENDAFTER);});
    connect(this->playlistTable->table, &BabeTable::saveToPlaylist, this->playlistTable, &PlaylistsView::saveToPlaylist);

    this->collectionTable = new BabeTable(this);
    this->collectionTable->setFrameShape(QFrame::StyledPanel);
    this->collectionTable->setFrameShadow(QFrame::Sunken);
    this->collectionTable->showColumn(static_cast<int>(Bae::KEY::STARS));
    this->collectionTable->showColumn(static_cast<int>(Bae::KEY::GENRE));
    connect(this->collectionTable,&BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { this->addToPlaylist(list,false,APPENDBOTTOM);});
    //    connect(this->collectionTable,&BabeTable::finishedPopulating,[this]()
    //    {
    //        this->collectionTable->sortByColumn(static_cast<int>(Bae::KEY::ARTIST), Qt::AscendingOrder);
    //    });
    connect(this->collectionTable, &BabeTable::removeIt_clicked, this, &BabeWindow::removeSong);
    connect(this->collectionTable, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->collectionTable, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->collectionTable, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->collectionTable, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->collectionTable, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->collectionTable, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->collectionTable, &BabeTable::appendIt, [this] (const Bae::DB_LIST &list) { this->addToPlaylist(list,false, APPENDAFTER);});
    connect(this->collectionTable, &BabeTable::saveToPlaylist, this->playlistTable, &PlaylistsView::saveToPlaylist);

    this->mainList = new BabeTable(this);
    this->mainList->setObjectName("mainList");
    this->mainList->hideColumn(static_cast<int>(Bae::KEY::ALBUM));
    this->mainList->hideColumn(static_cast<int>(Bae::KEY::ARTIST));
    this->mainList->hideColumn(static_cast<int>(Bae::KEY::DURATION));
    this->mainList->horizontalHeader()->setVisible(false);
    this->mainList->enableRowColoring(true);
    this->mainList->enableRowDragging(true);
    this->mainList->enablePreview(false);
    this->mainList->setAddMusicMsg("\nDrag and drop music here!","face-ninja");
    connect(this->mainList,&BabeTable::indexesMoved,[this](const int &row, const int &newRow)
    {
        if(row > this->current_song_pos && newRow < this->current_song_pos )
        {
            this->current_song_pos++;
            this->prev_song_pos++;

            if(this->prev_queued_song_pos != -1)
                this->prev_queued_song_pos++;

        }else if(row < this->current_song_pos && newRow >= this->current_song_pos)
        {
            this->current_song_pos--;
            this->prev_song_pos--;

            if(this->prev_queued_song_pos != -1)
                this->prev_queued_song_pos--;
        }
    });

    connect(mainList,&BabeTable::indexRemoved,[this](int  row)
    {
        if(row < this->current_song_pos)
        {
            this->current_song_pos--;
            this->prev_song_pos--;
            if(this->prev_queued_song_pos != -1)
                this->prev_queued_song_pos--;
        }

    });

    connect(this->mainList, &BabeTable::tableWidget_doubleClicked, this, &BabeWindow::on_mainList_clicked);
    connect(this->mainList, &BabeTable::removeIt_clicked, this, &BabeWindow::removeSong);
    connect(this->mainList, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->mainList, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->mainList, &BabeTable::moodIt_clicked, this->mainList, &BabeTable::colorizeRow);
    connect(this->mainList, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->mainList, &BabeTable::saveToPlaylist, this->playlistTable, &PlaylistsView::saveToPlaylist);

    this->filterList = new BabeTable(this);
    this->filterList->hideColumn(static_cast<int>(Bae::KEY::ALBUM));
    this->filterList->hideColumn(static_cast<int>(Bae::KEY::ARTIST));
    this->filterList->horizontalHeader()->setVisible(false);
    this->filterList->enableRowColoring(true);
    this->filterList->setAddMusicMsg("\nDidn't find anything!","face-surprise");
    connect(this->filterList, &BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list)
    {
        playItNow(list);
        ui->search->clear();
    });

    connect(this->filterList, &BabeTable::removeIt_clicked, this, &BabeWindow::removeSong);
    connect(this->filterList, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->filterList, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->filterList, &BabeTable::moodIt_clicked, mainList ,&BabeTable::colorizeRow);
    connect(this->filterList, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->filterList, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->filterList, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->filterList, &BabeTable::appendIt, [this] (Bae::DB_LIST list)
    {
        addToPlaylist(list, false, APPENDAFTER);
    });
    connect(filterList,&BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);

    this->mainListView = new QStackedWidget(this);
    this->mainListView->setFrameShape(QFrame::NoFrame);

    this->mainListView->addWidget(this->mainList);
    this->mainListView->addWidget(this->filterList);

    this->resultsTable = new BabeTable(this);
    this->resultsTable->setFrameShape(QFrame::StyledPanel);
    this->resultsTable->setFrameShadow(QFrame::Sunken);    //    resultsTable->passStyle("QHeaderView::section { background-color:#575757; color:white; }");
    this->resultsTable->horizontalHeader()->setHighlightSections(true);
    this->resultsTable->showColumn(static_cast<int>(Bae::KEY::STARS));
    this->resultsTable->showColumn(static_cast<int>(Bae::KEY::GENRE));
    connect(this->resultsTable, &BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(this->resultsTable, &BabeTable::removeIt_clicked, this, &BabeWindow::removeSong);
    connect(this->resultsTable, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->resultsTable, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->resultsTable, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->resultsTable, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->resultsTable, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->resultsTable, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->resultsTable, &BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(this->resultsTable, &BabeTable::saveToPlaylist, playlistTable, &PlaylistsView::saveToPlaylist);

    this->albumsTable = new AlbumsView(false, this);
    connect(this->albumsTable->albumTable, &BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(this->albumsTable->albumTable, &BabeTable::removeIt_clicked, this, &BabeWindow::removeSong);
    connect(this->albumsTable->albumTable, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->albumsTable->albumTable, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->albumsTable->albumTable, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->albumsTable->albumTable, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->albumsTable->albumTable, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->albumsTable->albumTable, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->albumsTable->albumTable, &BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(this->albumsTable->albumTable, &BabeTable::saveToPlaylist, playlistTable, &PlaylistsView::saveToPlaylist);
    connect(this->albumsTable, &AlbumsView::playAlbum, this, &BabeWindow::putAlbumOnPlay);
    connect(this->albumsTable, &AlbumsView::babeAlbum, this, &BabeWindow::babeAlbum);
    connect(this->albumsTable, &AlbumsView::albumDoubleClicked, this, &BabeWindow::albumDoubleClicked);
    connect(this->albumsTable, &AlbumsView::expandTo, this, &BabeWindow::expandAlbumList);

    this->artistsTable = new AlbumsView(true, this);
    this->artistsTable->expandBtn->setVisible(false);
    this->artistsTable->albumTable->showColumn(static_cast<int>(Bae::KEY::ALBUM));
    connect(this->artistsTable->albumTable, &BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(this->artistsTable->albumTable, &BabeTable::removeIt_clicked, this, &BabeWindow::removeSong);
    connect(this->artistsTable->albumTable, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->artistsTable->albumTable, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->artistsTable->albumTable, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->artistsTable->albumTable, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->artistsTable->albumTable, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->artistsTable->albumTable, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->artistsTable->albumTable, &BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(this->artistsTable->albumTable, &BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);
    connect(this->artistsTable, &AlbumsView::playAlbum, this, &BabeWindow::putAlbumOnPlay);
    connect(this->artistsTable, &AlbumsView::babeAlbum, this, &BabeWindow::babeAlbum);
    connect(this->artistsTable, &AlbumsView::albumDoubleClicked, this, &BabeWindow::albumDoubleClicked);

    this->infoTable = new InfoView(this);
    connect(this->infoTable, &InfoView::playAlbum, this,&BabeWindow::putAlbumOnPlay);
    connect(this->infoTable, &InfoView::similarBtnClicked, [this](QStringList queries) { this->ui->search->setText(queries.join(",")); });
    connect(this->infoTable, &InfoView::tagsBtnClicked, [this](QStringList queries) { this->ui->search->setText(queries.join(",")); });
    connect(this->infoTable, &InfoView::tagClicked, [this](QString query) { this->ui->search->setText(query);});
    connect(this->infoTable, &InfoView::similarArtistTagClicked, [this](QString query) { this->ui->search->setText(query);});

    this->rabbitTable = new RabbitView(this);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::tableWidget_doubleClicked, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDBOTTOM);});
    connect(this->rabbitTable->generalSuggestion, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::previewFinished, this ,&BabeWindow::play);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::appendIt, [this] (Bae::DB_LIST list) { addToPlaylist(list,false,APPENDAFTER);});
    connect(this->rabbitTable->generalSuggestion, &BabeTable::saveToPlaylist, playlistTable,&PlaylistsView::saveToPlaylist);
    connect(this->rabbitTable->artistSuggestion, &BabeGrid::playAlbum, this,&BabeWindow::putAlbumOnPlay);

    this->settings_widget->readSettings();
    connect(this->settings_widget, &settings::toolbarIconSizeChanged, this, &BabeWindow::setToolbarIconSize);
    connect(this->settings_widget, &settings::toolbarPositionChanged, [this](const Qt::ToolBarArea &pos)
    {
        this->removeToolBar(this->mainToolbar);
        this->addToolBar(pos, this->mainToolbar);
        this->mainToolbar->show();
    });
    connect(this->settings_widget, &settings::refreshTables, this, &BabeWindow::refreshTables);

    /* THE BUTTONS VIEWS */
    connect(ui->tracks_view, &QToolButton::clicked, this, &BabeWindow::collectionView);
    connect(ui->albums_view, &QToolButton::clicked, this, &BabeWindow::albumsView);
    connect(ui->artists_view, &QToolButton::clicked, this, &BabeWindow::artistsView);
    connect(ui->playlists_view, &QToolButton::clicked, this, &BabeWindow::playlistsView);
    connect(ui->rabbit_view, &QToolButton::clicked, this, &BabeWindow::rabbitView);
    connect(ui->info_view, &QToolButton::clicked, this, &BabeWindow::infoView);
    connect(ui->settings_view, &QToolButton::clicked, this,&BabeWindow::settingsView);

    this->views = new QStackedWidget(this);
    this->views->setFrameShape(QFrame::NoFrame);
    this->views->insertWidget(VIEWS::COLLECTION, this->collectionTable);
    this->views->insertWidget(VIEWS::ALBUMS, this->albumsTable);
    this->views->insertWidget(VIEWS::ARTISTS, this->artistsTable);
    this->views->insertWidget(VIEWS::PLAYLISTS, this->playlistTable);
    this->views->insertWidget(VIEWS::INFO, this->infoTable);
    this->views->insertWidget(VIEWS::RABBIT, this->rabbitTable);
    this->views->insertWidget(VIEWS::SETTINGS, this->settings_widget);
    this->views->insertWidget(VIEWS::RESULTS, this->resultsTable);
}

void BabeWindow::setUpCollectionViewer()
{
    this->mainLayout = new QHBoxLayout;

    this->mainToolbar = new QToolBar("Views", this);
    this->mainToolbar->setMovable(false);
    connect(this->mainToolbar, &QToolBar::topLevelChanged, [this](bool topLevel)
    {
        Q_UNUSED(topLevel);

        switch(this->toolBarArea(this->mainToolbar))
        {
        case Qt::TopToolBarArea:
        case Qt::BottomToolBarArea:
        {
            this->mainToolbar->setOrientation(Qt::Orientation::Horizontal);
            break;
        }
        case Qt::RightToolBarArea:
        case Qt::LeftToolBarArea:
        {
            this->mainToolbar->setOrientation(Qt::Orientation::Vertical);
            break;
        }
        default:break;
        }
    });

    this->mainToolbar->setContentsMargins(0, 0, 0, 0);
    this->mainToolbar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    this->mainToolbar->addWidget(this->ui->info_view);
    this->mainToolbar->addWidget(ui->spacer1);
    this->mainToolbar->addWidget(this->ui->tracks_view);
    this->mainToolbar->addWidget(this->ui->albums_view);
    this->mainToolbar->addWidget(this->ui->artists_view);
    this->mainToolbar->addWidget(this->ui->playlists_view);
    this->mainToolbar->addWidget(ui->spacer2);
    this->mainToolbar->addWidget(this->ui->rabbit_view);
    this->mainToolbar->addWidget(this->ui->settings_view);

    this->secondaryToolbar = new QToolBar("Search", this);
    this->secondaryToolbar->setAllowedAreas(Qt::ToolBarArea::AllToolBarAreas);
    this->secondaryToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->secondaryToolbar->setMovable(false);
    this->secondaryToolbar->addWidget(this->ui->goBackBtn);
    this->secondaryToolbar->addWidget(this->ui->search);
    this->secondaryToolbar->actions().at(0)->setVisible(false);

    this->searchTimer = new QTimer(this);
    this->searchTimer->setSingleShot(true);
    this->searchTimer->setInterval(500);
    connect(this->searchTimer, &QTimer::timeout,this, &BabeWindow::runSearch);
    connect(this->ui->search, SIGNAL(textChanged(QString)), this->searchTimer, SLOT(start()));

    ui->search->setClearButtonEnabled(true);

    this->mainLayout->addWidget(this->views);
    this->mainLayout->addWidget(this->rightFrame);
    this->mainLayout->setContentsMargins(0, 0, 0, 0);

    this->mainWidget= new QWidget(this);
    this->mainWidget->setMinimumHeight(static_cast<int>(ALBUM_SIZE)*2);

    this->mainWidget->setLayout(mainLayout);

    this->addToolBar(static_cast<Qt::ToolBarArea>(Bae::loadSettings("TOOLBAR_POS","MAINWINDOW",Qt::ToolBarArea::LeftToolBarArea).toInt()), this->mainToolbar);
    this->addToolBar(Qt::ToolBarArea::BottomToolBarArea, this->secondaryToolbar);

    this->setCentralWidget(this->mainWidget);
}

void BabeWindow::setUpPlaylist()
{
    auto playlistWidget_layout = new QGridLayout;
    playlistWidget_layout->setContentsMargins(0,0,0,0);
    playlistWidget_layout->setSpacing(0);
    playlistWidget_layout->setMargin(0);

    this->playlistWidget = new QWidget(this);
    this->playlistWidget->setLayout(playlistWidget_layout);

    this->playlistWidget->setFixedWidth(static_cast<int>(ALBUM_SIZE));

    ui->miniPlaybackBtn->setVisible(false);

    this->ui->controls->setMinimumSize(static_cast<int>(ALBUM_SIZE), static_cast<int>(ALBUM_SIZE));
    this->ui->controls->setMaximumSize(static_cast<int>(ALBUM_SIZE), static_cast<int>(ALBUM_SIZE));
    this->ui->controls->installEventFilter(this);
    this->ui->controls->setVisible(false);

    auto controlsColor = this->palette().color(QPalette::Window);
    this->ui->controls->setStyleSheet(QString("QWidget#controls{background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(%1,%2,%3,100%),stop:1 rgba(%1,%2,%3,40%))}").arg(QString::number(controlsColor.red()),QString::number(controlsColor.green()),QString::number(controlsColor.blue())));

    this->seekBar = new QSlider(this);
    this->seekBar->installEventFilter(this);
    connect(seekBar,&QSlider::sliderMoved,this,&BabeWindow::on_seekBar_sliderMoved);

    this->seekBar->setMaximum(1000);
    this->seekBar->setOrientation(Qt::Horizontal);
    this->seekBar->setContentsMargins(0, 0, 0, 0);
    this->seekBar->setFixedHeight(5);
    this->seekBar->setStyleSheet(QString("QSlider { background:transparent;} "
                                         "QSlider::groove:horizontal {border: none; background: transparent; height: 5px; border-radius: 0; } "
                                         "QSlider::sub-page:horizontal { background: %1;border: none; height: 5px;border-radius: 0;} "
                                         "QSlider::add-page:horizontal {background: transparent; border: none; height: 5px; border-radius: 0; } "
                                         "QSlider::handle:horizontal {background: %1; width: 8px; } "
                                         "QSlider::handle:horizontal:hover {background: qlineargradient(x1:0, y1:0, x2:1, y2:1,stop:0 #fff, stop:1 #ddd);border: 1px solid #444;border-radius: 4px;}"
                                         "QSlider::sub-page:horizontal:disabled {background: transparent;border-color: #999;}QSlider::add-page:horizontal:disabled {background: transparent;border-color: #999;}"
                                         "QSlider::handle:horizontal:disabled {background: transparent;border: 1px solid #aaa;border-radius: 4px;}").arg(this->palette().color(QPalette::Highlight).name()));

    ui->hide_sidebar_btn->setToolTip("Go Mini");
    ui->shuffle_btn->setToolTip("Shuffle");

    playlistWidget_layout->addWidget(album_art, 0,0,Qt::AlignTop);
    playlistWidget_layout->addWidget(ui->controls, 0,0,Qt::AlignTop);
    playlistWidget_layout->addWidget(ui->frame_4,1,0);
    playlistWidget_layout->addWidget(seekBar,2,0);
    playlistWidget_layout->addWidget(ui->frame_5,3,0);
    playlistWidget_layout->addWidget(mainListView,4,0);
}

void BabeWindow::movePanel(const POSITION &pos)
{
    auto position = QWidget::mapToGlobal(views->pos());

    switch(pos)
    {
    case RIGHT:
    {
        this->mainLayout->removeWidget(rightFrame);
        this->mainLayout->insertWidget(1,rightFrame);

        this->playlistPos = RIGHT;
        break;
    }
    case LEFT:
    {
        this->mainLayout->removeWidget(rightFrame);
        this->mainLayout->insertWidget(0,rightFrame);

        this->playlistPos = LEFT;
        break;
    }
    case OUT:
    {
        if(this->viewMode != FULLMODE) expand();

        this->mainLayout->removeWidget(this->rightFrame);

        this->rightFrame->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        this->rightFrame->setWindowTitle("Playlist");

        this->rightFrame->show();
        this->rightFrame->setMinimumHeight(static_cast<int>(ALBUM_SIZE)*2);
        this->rightFrame->window()->setFixedWidth(rightFrame->minimumSizeHint().width());
        this->rightFrame->window()->move(position.x()+this->size().width()-rightFrame->size().width(), this->pos().y());

        this->playlistSta = OUT;
        break;
    }
    case IN:
    {
        this->rightFrame->setWindowFlags(Qt::Widget);

        this->mainLayout->insertWidget(playlistPos==RIGHT? 1 : 0, rightFrame);
        this->rightFrame->setFixedWidth(rightFrame->minimumSizeHint().width());
        this->rightFrame->setMinimumHeight(0);

        this->rightFrame->show();
        this->playlistSta = IN;
        break;
    }
    }
}

void BabeWindow::loadStyle()
{
    //* LOAD THE STYLE* //
    QFile styleFile(stylePath);
    if(styleFile.exists())
    {
        qDebug()<<"A Babe style file exists";
        styleFile.open(QFile::ReadOnly);
        QString style(styleFile.readAll());
        this->setStyleSheet(style);
    }
}

void BabeWindow::setUpRightFrame()
{
    this->playlistWidget->setFixedWidth(static_cast<int>(ALBUM_SIZE));

    auto rightFrame_layout = new QGridLayout;
    rightFrame_layout->setContentsMargins(0,0,0,0);
    rightFrame_layout->setSpacing(0);
    rightFrame_layout->setMargin(0);

    this->rightFrame = new QFrame(this);
    this->rightFrame->setObjectName("rightFrame");
    this->rightFrame->installEventFilter(this);
    this->rightFrame->setAcceptDrops(true);
    this->rightFrame->setFrameShadow(QFrame::Raised);
    this->rightFrame->setFrameShape(QFrame::StyledPanel);
    this->rightFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    this->rightFrame->setLayout(rightFrame_layout);

    rightFrame_layout->addWidget(this->playlistWidget, 0, 0);

    this->rightFrame->setFixedWidth(this->rightFrame->minimumSizeHint().width());

}

void BabeWindow::setUpMenuBar()
{
    auto moveIt= new QAction("Move to left", this);
    moveIt->setShortcut(QKeySequence("Ctrl+m"));
    connect(moveIt, &QAction::triggered,[=]()
    {
        if(this->playlistSta == OUT) emit ui->controls->actions().at(1)->triggered();

        if(this->playlistPos == RIGHT)
        {
            this->movePanel(LEFT);
            moveIt->setText("Move to right");

        }else if(this->playlistPos == LEFT)
        {
            this->movePanel(RIGHT);
            moveIt->setText("Move to left");

        }
    });

    auto popPanel = new QAction("Pop panel out", this);
    popPanel->setShortcut(QKeySequence("Ctrl+p"));
    connect(popPanel, &QAction::triggered, [popPanel, this]()
    {
        if(this->playlistSta == OUT)
        {
            this->movePanel(IN);
            popPanel->setText("Pop panel out");
        }
        else if (this->playlistSta == IN)
        {
            this->movePanel(OUT);
            popPanel->setText("Pop panel in");
        }
    });

    auto hideTimeLabels = new QAction("Hide time labels", this);
    connect (hideTimeLabels, &QAction::triggered, [hideTimeLabels, this]()
    {
        if(ui->time->isVisible() && ui->duration->isVisible())
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

    auto refreshIt = new QAction("Calibrate...", this);
    refreshIt->setShortcut(QKeySequence("Ctrl+b"));
    refreshIt->setToolTip("Clean & play Babe'd tracks");
    connect(refreshIt, &QAction::triggered, [this]() { calibrateMainList(); });

    auto clearIt = new QAction("Clear out...", this);
    clearIt->setShortcut(QKeySequence("Ctrl+e"));
    clearIt->setToolTip("Remove unselected tracks");
    connect(clearIt, &QAction::triggered, [this]()
    {
        this->clearMainList();
        if(this->mainList->rowCount() > 0)
        {
            this->mainList->setCurrentCell(this->current_song_pos, static_cast<int>(Bae::KEY::TITLE));
            this->mainList->getItem(this->current_song_pos, Bae::KEY::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));
        }
    });

    auto cleanIt = new QAction("Clean...",this);
    cleanIt->setShortcut(QKeySequence("Ctrl+c"));
    cleanIt->setToolTip("Remove repeated tracks");
    connect(cleanIt, &QAction::triggered, [this]() { mainList->removeRepeated(); /*this->removequeuedTracks();*/ });

    auto open = new QAction("Open...", this);
    open->setShortcut(QKeySequence("Ctrl+o"));
    connect(open, &QAction::triggered, this, &BabeWindow::on_open_btn_clicked);

    auto fileMenu = this->menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(open);

    auto panelMenu = this->menuBar()->addMenu(tr("&Playlist"));
    panelMenu->addAction(moveIt);
    panelMenu->addAction(popPanel);
    panelMenu->addAction(hideTimeLabels);
    panelMenu->addSeparator();
    panelMenu->addAction(refreshIt);
    panelMenu->addAction(clearIt);
    panelMenu->addAction(cleanIt);
    panelMenu->addAction(refreshIt);

}

void BabeWindow::albumDoubleClicked(const Bae::DB &info)
{
    auto artist = info[Bae::KEY::ARTIST];
    auto album = info[Bae::KEY::ALBUM];

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

    addToPlaylist(mapList, false, APPENDBOTTOM);
}

void BabeWindow::playItNow(const Bae::DB_LIST &list)
{
    if(!list.isEmpty())
    {
        /* if in the list there's a single track, check its position on the mainPlaylist*/
        if(list.size() == 1)
        {
            auto pos = this->mainList->getAllTableContent().indexOf(list.first());

            if( pos != -1)
                this->mainList->setCurrentCell(pos, static_cast<int>(Bae::KEY::TITLE));
            else
            {
                this->addToPlaylist(list, false, APPENDBOTTOM);
                this->mainList->setCurrentCell(mainList->rowCount()-list.size(), static_cast<int>(Bae::KEY::TITLE));
            }

        }else
        {
            this->addToPlaylist(list, false, APPENDBOTTOM);
            this->mainList->setCurrentCell(this->mainList->rowCount()-list.size(), static_cast<int>(Bae::KEY::TITLE));
        }

        this->loadTrack();
    }

}

void BabeWindow::putAlbumOnPlay(const Bae::DB &info)
{
    if(!info.isEmpty())
    {
        Bae::DB_LIST mapList;

        switch(Bae::albumType(info))
        {
        case TABLE::ARTISTS:
            mapList = connection.getArtistTracks(info[Bae::KEY::ARTIST]);
            break;
        case TABLE::ALBUMS:
            mapList = connection.getAlbumTracks(info[Bae::KEY::ALBUM], info[Bae::KEY::ARTIST]);
            break;
        default:break;
        }

        if(!mapList.isEmpty()) this->putOnPlay(mapList);
    }

}

void BabeWindow::putOnPlay(const Bae::DB_LIST &mapList)
{
    if(!mapList.isEmpty())
    {
        this->mainList->flushTable();
        this->addToPlaylist(mapList, false, APPENDBOTTOM);

        if(this->mainList->rowCount()>0)
        {
            this->current_song_pos=0;
            this->prev_song_pos = current_song_pos;
            this->mainList->setCurrentCell(this->current_song_pos, static_cast<int>(Bae::KEY::TITLE));

            this->loadTrack();
        }
    }
}

void BabeWindow::addToPlayed(const QString &url)
{
    qDebug()<<"Song totally played"<<url;
    if(this->connection.check_existance(Bae::TABLEMAP[Bae::TABLE::TRACKS], Bae::KEYMAP[Bae::KEY::URL],url))
        this->connection.playedTrack(url);
}

bool BabeWindow::eventFilter(QObject *object, QEvent *event)
{
    if(object == this->ui->controls)
    {
        if(event->type() == QEvent::Enter)
            ui->miniPlaybackBtn->setVisible(true);
        else if(event->type()==QEvent::Leave && !miniPlayback)
            if(ui->miniPlaybackBtn->isVisible()) ui->miniPlaybackBtn->setVisible(false);

        event->accept();
        return true;
    }


    if ((object == this->seekBar) && this->seekBar->isEnabled())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            auto mevent = static_cast<QMouseEvent *>(event);
            qreal value = this->seekBar->minimum() + (this->seekBar->maximum() - this->seekBar->minimum()) * mevent->localPos().x() / this->seekBar->width();

            if (mevent->button() == Qt::LeftButton)
                emit this->seekBar->sliderMoved(qRound(value));

            event->accept();
            return true;
        }

        if (event->type() == QEvent::MouseMove)
        {
            auto mevent = static_cast<QMouseEvent *>(event);
            qreal value = this->seekBar->minimum() + (this->seekBar->maximum() - this->seekBar->minimum()) * mevent->localPos().x() / this->seekBar->width();

            if (mevent->buttons() & Qt::LeftButton)
                emit this->seekBar->sliderMoved(qRound(value));

            event->accept();
            return true;
        }

        if (event->type() == QEvent::MouseButtonDblClick)
        {
            event->accept();
            return true;
        }
    }

    if(object == this->rightFrame)
    {
        if(event->type() == QEvent::Enter && !stopped)
        {
            this->showControls(true);
            event->accept();
            return true;
        }

        if(event->type()==QEvent::Leave)
        {
            this->showControls(false);
            event->accept();
            return true;
        }

        if(event->type()==QEvent::DragEnter)
            event->accept();

        if(event->type()==QEvent::DragLeave)
            event->accept();

        if(event->type()==QEvent::DragMove)
            event->accept();

        if(event->type()==QEvent::Drop)
        {
            QDropEvent* dropEvent = static_cast<QDropEvent*>(event);

            Bae::DB_LIST mapList;
            QList<QUrl> urls = dropEvent->mimeData()->urls();

            if(urls.isEmpty())
            {
                auto info = dropEvent->mimeData()->text();
                auto infoList = info.split("/by/");

                if(infoList.size() == 2)
                {
                    auto artist = infoList.at(1).simplified();
                    auto album = infoList.at(0).simplified();

                    mapList = this->connection.getAlbumTracks(album,artist);

                }else mapList = this->connection.getArtistTracks(info);


                this->addToPlaylist(mapList,false,APPENDBOTTOM);

                event->accept();
                return true;

            }else
            {
                QList<QUrl> urls = dropEvent->mimeData()->urls();
                QStringList list;

                for(auto url : urls)
                    list << url.path();

                this->appendFiles(list);

                event->accept();
                return true;
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

    if(this->viewMode == FULLMODE )
        Bae::saveSettings("GEOMETRY",this->geometry(),"MAINWINDOW");

    Bae::saveSettings("PLAYLIST", this->mainList->getTableColumnContent(Bae::KEY::URL), "MAINWINDOW");
    Bae::saveSettings("PLAYLIST_POS", this->current_song_pos, "MAINWINDOW");
    Bae::saveSettings("TOOLBAR_ICON_SIZE", this->iconSize, "MAINWINDOW");
    Bae::saveSettings("MINIPLAYBACK", this->miniPlayback, "MAINWINDOW");
    Bae::saveSettings("PANEL_POS", this->playlistPos, "MAINWINDOW");
    Bae::saveSettings("TIME_LABEL", ui->time->isVisible() && ui->duration->isVisible(), "MAINWINDOW");
    Bae::saveSettings("TOOLBAR_POS", this->toolBarArea(this->mainToolbar), "MAINWINDOW");

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

void BabeWindow::refreshTables(const QMap<Bae::TABLE, bool> &tableReset)
{
    nof->notify("Loading collection","this might take some time depending on your colleciton size");

    QSqlQuery query;

    for (auto table : tableReset.keys())
        switch(table)
        {
        case Bae::TABLE::TRACKS:
        {
            if(tableReset[table]) this->collectionTable->flushTable();
            query.prepare(QString("SELECT * FROM %1 ORDER BY  %2").arg(Bae::TABLEMAP[Bae::TABLE::TRACKS],Bae::KEYMAP[Bae::KEY::ARTIST]));
            this->collectionTable->populateTableView(query);
            break;
        }
        case Bae::TABLE::ALBUMS:
        {
            if(tableReset[table]) this->albumsTable->flushView();

            query.prepare(QString("SELECT * FROM %1 ORDER BY  %2").arg(Bae::TABLEMAP[Bae::TABLE::ALBUMS],Bae::KEYMAP[Bae::KEY::ALBUM]));
            this->albumsTable->populate(query);
            this->albumsTable->hideAlbumFrame();
            break;
        }
        case Bae::TABLE::ARTISTS:
        {
            if(tableReset[table]) this->artistsTable->flushView();

            query.prepare(QString("SELECT * FROM %1 ORDER BY  %2").arg(Bae::TABLEMAP[Bae::TABLE::ARTISTS],Bae::KEYMAP[Bae::KEY::ARTIST]));
            this->artistsTable->populate(query);
            this->artistsTable->hideAlbumFrame();
            break;
        }
        case Bae::TABLE::PLAYLISTS:
        {
            this->playlistTable->list->clear();
            this->playlistTable->setDefaultPlaylists();
            this->playlistTable->setPlaylists(this->connection.getPlaylists());
            break;
        }

        default: return;
        }

    emit finishRefresh();
}

void BabeWindow::keyPressEvent(QKeyEvent *event) //TODO
{
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
    QMainWindow::keyPressEvent(event);

}

void BabeWindow::showControls(const bool &state)
{
    if(state)
    {
        if(!ui->controls->isVisible())
        {
            if(!this->miniPlayback) this->blurWidget(*this->album_art,15);
            else this->blurWidget(*this->album_art,28);

            ui->controls->setVisible(true);
        }
    }else
    {
        if(ui->controls->isVisible() && !this->miniPlayback)
        {
            this->blurWidget(*this->album_art,0);
            ui->controls->setVisible(false);
        }
    }
}

void BabeWindow::fetchCoverArt(Bae::DB &song)
{
    if(Bae::artworkCache(song, KEY::ALBUM)) return;

    if(Bae::artworkCache(song, KEY::ARTIST)) return;

    Pulpo pulpo;
    pulpo.registerServices({SERVICES::LastFm, SERVICES::Spotify, SERVICES::MusicBrainz});
    pulpo.setOntology(PULPO::ONTOLOGY::ALBUM);
    pulpo.setInfo(PULPO::INFO::ARTWORK);

    QEventLoop loop;

    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(1000);

    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    connect(&pulpo, &Pulpo::infoReady, [&](const Bae::DB &track,const PULPO::RESPONSE  &res)
    {
        Q_UNUSED(track);
        if(!res[PULPO::ONTOLOGY::ALBUM][PULPO::INFO::ARTWORK].isEmpty())
        {
            auto artwork = res[PULPO::ONTOLOGY::ALBUM][PULPO::INFO::ARTWORK][PULPO::CONTEXT::IMAGE].toByteArray();
            Bae::saveArt(song,artwork, Bae::CachePath);
        }

        loop.quit();
    });

    pulpo.feed(song, PULPO::RECURSIVE::OFF);

    timer.start();
    loop.exec();
    timer.stop();
}

void BabeWindow::setToolbarIconSize(const uint &iconSize)
{
    this->iconSize = iconSize;
    this->mainToolbar->setIconSize(QSize(static_cast<int>(iconSize),static_cast<int>(iconSize)));
    this->mainToolbar->update();
}

void BabeWindow::collectionView()
{
    if(this->resultsTable->rowCount() > 0) this->views->setCurrentIndex(RESULTS);

    else this->views->setCurrentIndex(COLLECTION);
    ui->tracks_view->setChecked(true);

    if(this->viewMode != FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::albumsView()
{
    this->views->setCurrentIndex(ALBUMS);
    ui->albums_view->setChecked(true);

    if(this->viewMode != FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::playlistsView()
{
    this->views->setCurrentIndex(PLAYLISTS);
    ui->playlists_view->setChecked(true);

    if(this->viewMode != FULLMODE) this->expand();

    if(this->playlistTable->list->currentRow() == 0)
    {
        this->playlistTable->list->setCurrentRow(0);
        emit this->playlistTable->list->clicked(this->playlistTable->list->model()->index(0,0));
    }

    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::rabbitView()
{
    this->views->setCurrentIndex(RABBIT);
    ui->rabbit_view->setChecked(true);

    if(this->viewMode != FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::infoView()
{
    this->views->setCurrentIndex(INFO);
    ui->info_view->setChecked(true);

    if(this->viewMode != FULLMODE) this->expand();
    this->prevIndex = views->currentIndex();
}

void BabeWindow::artistsView()
{
    this->views->setCurrentIndex(ARTISTS);
    ui->artists_view->setChecked(true);

    if(this->viewMode != FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::settingsView()
{
    this->views->setCurrentIndex(SETTINGS);
    ui->settings_view->setChecked(true);

    if(this->viewMode != FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::expand()
{
    this->setMinimumSize(this->minimumSizeHint().width(),0);

    this->viewMode = FULLMODE;

    this->secondaryToolbar->actions().at(0)->setVisible(false);
    this->secondaryToolbar->setVisible(true);

    if(!this->views->isVisible()) this->views->setVisible(true);
    if(!this->ui->frame_4->isVisible()) ui->frame_4->setVisible(true);
    if(!this->mainList->isVisible()) this->mainListView->setVisible(true);
    if(!this->ui->frame_5->isVisible()) ui->frame_5->setVisible(true);
    if(!this->mainToolbar->isVisible()) this->mainToolbar->setVisible(true);

    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    this->setMinimumHeight(0);
    this->setMinimumSize(this->minimumSizeHint().width(), 0);

    auto animation = new QPropertyAnimation(this, "geometry",this);
    connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);

    animation->setDuration(200);
    animation->setStartValue(this->geometry());
    animation->setEndValue(Bae::loadSettings("GEOMETRY","MAINWINDOW", this->geometry()).toRect());

    animation->start();

    ui->hide_sidebar_btn->setToolTip("Go Mini");

}

void BabeWindow::go_mini()
{
    this->setMinimumSize(this->minimumSizeHint().width(), 0);

    this->viewMode = MINIMODE;

    this->secondaryToolbar->setVisible(false);
    this->views->setVisible(false);
    ui->frame_4->setVisible(false);
    this->mainListView->setVisible(false);
    ui->frame_5->setVisible(false);

    this->setMaximumWidth(this->rightFrame->minimumSizeHint().width());

    auto animation = new QPropertyAnimation(this, "maximumHeight", this);
    connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);

    animation->setDuration(200);
    animation->setStartValue(this->size().height());
    animation->setEndValue(ui->controls->height());

    animation->start();

    ui->hide_sidebar_btn->setToolTip("Expand");
}

void BabeWindow::go_playlistMode()
{
    this->setMinimumSize(this->minimumSizeHint().width(),0);

    if(playlistSta != OUT)
    {
        Bae::saveSettings("GEOMETRY",this->geometry(),"MAINWINDOW");
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
        default:  icon = "go-back";
        }
        ui->goBackBtn->setIcon(QIcon::fromTheme(icon));

        if(!ui->frame_4->isVisible()) ui->frame_4->setVisible(true);
        if(!mainList->isVisible()) mainListView->setVisible(true);
        if(!ui->frame_5->isVisible()) ui->frame_5->setVisible(true);

        this->mainToolbar->setVisible(false);
        this->secondaryToolbar->actions().at(0)->setVisible(true);

        this->setMaximumWidth(rightFrame->minimumSizeHint().width());
        this->setMinimumWidth(rightFrame->minimumSizeHint().width());

        auto animation = new QPropertyAnimation(this, "maximumWidth",this);
        connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);

        animation->setDuration(200);
        animation->setStartValue(this->size().width());
        animation->setEndValue(rightFrame->minimumSizeHint().width());

        animation->start();

        this->views->setVisible(false);

        ui->hide_sidebar_btn->setToolTip("Go Mini");
    }
}

void BabeWindow::keepOnTop(bool state)
{
    if(state) this->setWindowFlags(Qt::WindowStaysOnTopHint);
    else this->setWindowFlags(defaultWindowFlags);
    this->show();
}

void BabeWindow::on_hide_sidebar_btn_clicked()
{
    if(playlistSta == OUT) emit ui->controls->actions().at(1)->triggered();

    switch(this->viewMode)
    {
    case FULLMODE:
        this->go_playlistMode(); break;

    case PLAYLISTMODE:
        this->go_mini(); break;

    case MINIMODE:
        this->expand(); break;
    }
}

void BabeWindow::on_shuffle_btn_clicked() //TODO
{
    if(this->shuffle_state == REGULAR)
    {
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-shuffle"));
        ui->shuffle_btn->setToolTip("Repeat");
        this->shuffle_state = SHUFFLE;

    }else if (this->shuffle_state == SHUFFLE)
    {
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-repeat"));
        ui->shuffle_btn->setToolTip("Consecutive");
        this->shuffle_state = REGULAR;
    }
}

void BabeWindow::on_open_btn_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),QDir().homePath()+"/Music/", tr("Audio (*.mp3 *.wav *.mp4 *.flac *.ogg *.m4a)"));
    if(!files.isEmpty()) this->appendFiles(files);
}

void BabeWindow::appendFiles(const QStringList &paths,const appendPos &pos)
{
    if(!paths.isEmpty())
    {
        QStringList trackList;
        for(auto url : paths)
        {
            if(QFileInfo(url).isDir())
            {
                QDirIterator it(url, Bae::formats, QDir::Files, QDirIterator::Subdirectories);

                while (it.hasNext()) trackList<<it.next();

            }else if (QFileInfo(url).isFile())
                trackList << url;
        }

        Tracklist tracks;
        tracks.add(trackList);
        this->addToPlaylist(tracks.getTracks(),false,pos);
    }
}

void BabeWindow::populateMainList()
{
    auto results = connection.getBabedTracks();
    this->mainList->populateTableView(results);
    this->mainList->resizeRowsToContents();
}

void BabeWindow::updateList()
{
    auto currentList = this->mainList->getAllTableContent();
    this->mainList->flushTable();
    for(auto list: currentList)
        this->mainList->addRow(list);
}

void BabeWindow::on_mainList_clicked(const Bae::DB_LIST &list)
{
    Q_UNUSED(list);
    this->loadTrack();
}

void BabeWindow::removeSong(const int &index)//TODO
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
        this->artistsTable->showAlbumInfo({{Bae::KEY::ARTIST,artist}});
    }
}

void BabeWindow::loadTrack()
{
    if(stopped) this->updater->start(100);

    this->prev_song = this->current_song;
    this->prev_song_pos = this->current_song_pos;

    if(this->prev_song_pos < this->mainList->rowCount() && this->mainList->getItem(current_song_pos,Bae::KEY::TITLE)->icon().name() != "clock")
        this->mainList->getItem(prev_song_pos,Bae::KEY::TITLE)->setIcon(QIcon());

    this->current_song_pos = this->mainList->getIndex() >= 0 ? this->mainList->getIndex() : 0 ;
    this->current_song = this->mainList->getRowData(this->current_song_pos);
    qDebug()<<this->current_song_pos;
    if(this->mainList->getItem(this->current_song_pos,Bae::KEY::TITLE)->icon().name()!="clock")
        this->mainList->getItem(this->current_song_pos,Bae::KEY::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));

    this->mainList->scrollTo(this->mainList->model()->index(this->current_song_pos, static_cast<int>(Bae::KEY::TITLE)));

    qDebug()<<"in mainlist=" << this->current_song[Bae::KEY::URL];

    if(Bae::fileExists(this->current_song[Bae::KEY::URL]))
    {
        this->player->setMedia(QUrl::fromLocalFile(this->current_song[Bae::KEY::URL]));
        this->play();

        this->album_art->setTitle(this->current_song[Bae::KEY::ARTIST], this->current_song[Bae::KEY::ALBUM]);

        this->babedIcon(this->isBabed(this->current_song));

        if(!this->isActiveWindow())
            this->nof->notifySong(current_song,QPixmap(current_song[Bae::KEY::ARTWORK]));

        this->loadMood();
        this->loadCover(this->current_song);

        if(this->miniPlayback)
        {
            this->blurWidget(*this->album_art, 28);
            this->album_art->saturatePixmap(100);
        }

        this->loadInfo(this->current_song);
        this->rabbitTable->seed(this->current_song);

    }else this->removeSong(this->current_song_pos);

}

int BabeWindow::isBabed(const Bae::DB &track)
{
    return this->connection.getTrackBabe(track[Bae::KEY::URL]);
}

void BabeWindow::loadMood()
{
    auto color = this->connection.getTrackArt(this->current_song[Bae::KEY::URL]);

    auto seekbarEffect = new QGraphicsColorizeEffect(this);
    //    auto controlsEffect = new QGraphicsColorizeEffect(this);

    if(!color.isEmpty())
    {
        seekbarEffect->setColor(QColor(color));
        seekbarEffect->setStrength(1.0);

    }else seekbarEffect->setStrength(0);

    this->seekBar->setGraphicsEffect(seekbarEffect);

}

bool BabeWindow::loadCover(DB &track) //tofix separte getalbumcover from get artisthead
{
    auto artist = track[Bae::KEY::ARTIST];
    auto album = track[Bae::KEY::ALBUM];
    auto title = track[Bae::KEY::TITLE];

    auto artistHead = this->connection.getArtistArt(artist);

    if(!artistHead.isEmpty())
    {
        this->infoTable->setArtistArt(artistHead);
        this->infoTable->artist->setArtist(artist);

    }else this->infoTable->setArtistArt(QString(":Data/data/cover.svg"));

    auto albumArt = this->connection.getAlbumArt(album, artist);

    this->current_song.insert(Bae::KEY::ARTWORK,albumArt.isEmpty()? "" : albumArt);

    if(!this->current_song[Bae::KEY::ARTWORK].isEmpty())
        this->album_art->putPixmap(this->current_song[Bae::KEY::ARTWORK]);

    else if (!artistHead.isEmpty())
    {
        this->current_song.insert(Bae::KEY::ARTWORK, artistHead);
        this->album_art->putPixmap(this->current_song[Bae::KEY::ARTWORK]);

    }else
    {
        this->fetchCoverArt(track);
        this->album_art->putPixmap(this->current_song[Bae::KEY::ARTWORK]);
    }

    return true;
}

void BabeWindow::addToQueue(const Bae::DB_LIST &tracks)
{
    this->prev_queued_song_pos = current_song_pos;

    QStringList queuedList;

    for(auto track : tracks)
    {
        if(!this->queued_songs.contains(track[Bae::KEY::URL]))
        {
            this->mainList->addRowAt(this->queued_songs.size(),track);
            this->mainList->getItem(this->queued_songs.size(),Bae::KEY::TITLE)->setIcon(QIcon::fromTheme("clock"));
            this->mainList->colorizeRow({this->queued_songs.size()},"#333");

            this->queued_songs.insert(track[Bae::KEY::URL],track);
            queuedList<<track[Bae::KEY::TITLE]+" by "+track[Bae::KEY::ARTIST];
            this->current_song_pos++;
        }

    }
    //mainList->addRowAt(current_song_pos+1,track,true);
    qDebug()<<"saving track pos to restore after queued is empty"<<prev_queued_song_pos;

    this->mainList->scrollToItem(mainList->getItem(0,Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

    this->nof->notify("Song added to Queue",queuedList.join("\n"));

}

void BabeWindow::on_seekBar_sliderMoved(const int &position)
{
    this->player->setPosition(this->player->duration() / 1000 * position);
}

void BabeWindow::update()
{
    if(this->mainList->rowCount() == 0) this->stop();

    if(!this->current_song.isEmpty())
    {
        if(!this->seekBar->isEnabled()) this->seekBar->setEnabled(true);

        if(!this->seekBar->isSliderDown())
            this->seekBar->setValue(static_cast<int>(static_cast<double>(this->player->position())/player->duration()*1000));
        ui->time->setText(Bae::transformTime(player->position()/1000));
        ui->duration->setText(Bae::transformTime(player->duration()/1000));

        if(this->player->state() == QMediaPlayer::StoppedState)
        {
            if(!this->queued_songs.isEmpty()) this->removeQueuedTrack(this->current_song, this->current_song_pos);

            this->prev_song = this->current_song;
            emit this->finishedPlayingSong(prev_song[Bae::KEY::URL]);
            this->next();
        }

    }else
    {
        this->seekBar->setValue(0);
        this->seekBar->setEnabled(false);
    }
}

bool BabeWindow::removeQueuedTrack(const Bae::DB &track, const int &pos)
{
    if(this->queued_songs.contains(track[Bae::KEY::URL]))
        if(this->mainList->getItem(pos,Bae::KEY::TITLE)->icon().name()=="clock")
        {
            this->mainList->removeRow(pos);
            this->queued_songs.remove(track[Bae::KEY::URL]);
            return true;
        }

    return false;
}

void BabeWindow::removequeuedTracks()
{
    QList<QMap<int, QString>> newList;

    for(auto row = 0; row < this->mainList->rowCount(); row++)

        if(this->mainList->getItem(row,Bae::KEY::TITLE)->icon().name()=="clock")
        {
            this->mainList->removeRow(row);
            this->queued_songs.remove(this->mainList->getRowData(row)[Bae::KEY::URL]);
            this->current_song_pos--;
        }
}

void BabeWindow::next()
{
    int nextSong=0;

    if(!this->queued_songs.isEmpty())
    {
        this->removeQueuedTrack(this->current_song, this->current_song_pos);
        nextSong = this->current_song_pos;

    }else nextSong= this->current_song_pos+1;

    if(this->queued_songs.isEmpty() && this->prev_queued_song_pos != -1)
    {
        nextSong= this->prev_queued_song_pos+1;
        this->prev_queued_song_pos=-1;
    }

    if(nextSong >= this->mainList->rowCount()) nextSong = 0;

    if(!this->queued_songs.isEmpty())
        nextSong=0;

    this->mainList->setCurrentCell((this->shuffle_state == SHUFFLE && this->queued_songs.isEmpty()) ? this->shuffleNumber(): nextSong, static_cast<int>(Bae::KEY::TITLE));

    this->loadTrack();
}

void BabeWindow::back()
{
    auto lCounter =  this->current_song_pos-1;

    if(lCounter < 0)
        lCounter =  this->mainList->rowCount() - 1;

    this->mainList->setCurrentCell(this->shuffle_state != SHUFFLE ? lCounter :  this->shuffleNumber(), static_cast<int>(Bae::KEY::TITLE));

    this->loadTrack();
}

int BabeWindow::shuffleNumber()
{
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0,mainList->rowCount()-1); // guaranteed unbiased

    auto random_integer = uni(rng);
    qDebug()<<"random number:"<<random_integer;
    if ( this->current_song_pos != random_integer) return random_integer;
    else return random_integer+1;
}

void BabeWindow::on_play_btn_clicked()
{
    if( this->mainList->rowCount() > 0 || ! this->current_song.isEmpty())
    {
        if( this->player->state() == QMediaPlayer::PlayingState) this->pause();
        else this->play();
    }
}

void BabeWindow::play()
{
    this->player->play();
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));
    this->setWindowTitle( this->current_song[Bae::KEY::TITLE]+" \xe2\x99\xa1 "+ this->current_song[Bae::KEY::ARTIST]);
}

void BabeWindow::pause()
{
    this->player->pause();
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-start"));
}

void BabeWindow::stop()
{
    this->album_art->putPixmap(QString(":/Data/data/empty.svg"));
    ui->controls->setVisible(false);
    this->album_art->setVisible(false);
    ui->frame_4->setVisible(false);
    this->seekBar->setVisible(false);
    ui->frame_5->setVisible(false);

    this->current_song.clear();
    this->prev_song = current_song;
    this->current_song_pos = 0;
    this->prev_song_pos = current_song_pos;
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-stop"));

    this->player->stop();
    this->updater->stop();

    this->setWindowTitle(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    this->stopped = true;
}

void BabeWindow::on_backward_btn_clicked()
{
    if(this->mainList->rowCount() > 0)
    {
        if(this->player->position() > 3000) this->player->setPosition(0);
        else this->back();
    }
}

void BabeWindow::on_foward_btn_clicked()
{
    if(this->mainList->rowCount() > 0) this->next();
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
    if(!this->current_song.isEmpty())
    {
        if(!this->isBabed(this->current_song))
        {
            if(this->babeTrack(this->current_song)) this->babedIcon(true);
        }else
        {
            if(this->unbabeIt(this->current_song)) this->babedIcon(false);
        }
    }
}

void BabeWindow::babeAlbum(const Bae::DB &info)
{
    auto artist =info[Bae::KEY::ARTIST];
    auto album = info[Bae::KEY::ALBUM];

    Bae::DB_LIST mapList;

    switch(Bae::albumType(info))
    {
    case TABLE::ARTISTS:
        mapList = this->connection.getArtistTracks(artist);
        break;
    case TABLE::ALBUMS:
        mapList = this->connection.getAlbumTracks(album, artist);
        break;
    default: break;
    }

    if(!mapList.isEmpty())
        this->babeIt(mapList);
}

bool BabeWindow::unbabeIt(const Bae::DB &track)
{
    if(this->connection.babeTrack(track[Bae::KEY::URL],0))
    {
        nof->notify("Song unBabe'd it",track[Bae::KEY::TITLE]+" by "+track[Bae::KEY::ARTIST]);
        return  true;
    }

    return false;

}

bool BabeWindow::babeTrack(const Bae::DB &track)
{
    auto url = track[Bae::KEY::URL];
    if(this->isBabed(track))
    {
        if(this->unbabeIt(track)) return true;

        return false;

    }else
    {
        if(this->connection.check_existance(Bae::TABLEMAP[Bae::TABLE::TRACKS],Bae::KEYMAP[Bae::KEY::URL],url))
        {
            if(this->connection.babeTrack(url,true))
            {
                nof->notify("Song Babe'd it",track[Bae::KEY::TITLE]+" by "+track[Bae::KEY::ARTIST]);
                this->addToPlaylist({track},true,APPENDBOTTOM);
                return true;
            }

            return false;

        }else
        {
            ui->fav_btn->setEnabled(false);
            auto newTrack = track;
            newTrack.insert(Bae::KEY::BABE, "1");
            this->connection.addTrack(newTrack);

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

    this->infoTable->setLyrics(lyrics);
    this->infoTable->setAlbumInfo(albumWiki);
    this->infoTable->setArtistInfo(artistWiki);
    this->infoTable->setArtistTags(artistTags);
    this->infoTable->setAlbumTags(albumTags);
}

void BabeWindow::babeIt(const Bae::DB_LIST &tracks)
{
    for(auto track : tracks)
        if(!babeTrack(track)) qWarning()<<"couldn't Babe track:"<<track[Bae::KEY::URL];
}

void  BabeWindow::infoIt(const Bae::DB &track)
{
    infoView();
    this->loadInfo(track);
}

void BabeWindow::addToPlaylist(const Bae::DB_LIST &mapList, const bool &notRepeated, const appendPos &pos)
{
    if(!mapList.isEmpty())
    {
        if(notRepeated)
        {
            Bae::DB_LIST newList;
            QStringList alreadyInList = this->mainList->getTableColumnContent(Bae::KEY::URL);
            for(auto track: mapList)
            {
                if(!alreadyInList.contains(track[Bae::KEY::URL]))
                {
                    newList<<track;
                    switch(pos)
                    {
                    case APPENDBOTTOM:
                        this->mainList->addRow(track);
                        this->mainList->scrollToBottom();

                        break;
                    case APPENDTOP:
                        this->mainList->addRowAt(0, track);
                        this->mainList->scrollToItem(this->mainList->getItem(0, Bae::KEY::TITLE), QAbstractItemView::PositionAtTop);

                        break;
                    case APPENDAFTER:
                        this->mainList->addRowAt(current_song_pos+1, track);
                        this->mainList->scrollToItem(this->mainList->getItem(current_song_pos+1, Bae::KEY::TITLE), QAbstractItemView::PositionAtTop);

                        break;
                    case APPENDBEFORE:
                        this->mainList->addRowAt(current_song_pos, track);
                        this->mainList->scrollToItem(this->mainList->getItem(current_song_pos, Bae::KEY::TITLE), QAbstractItemView::PositionAtTop);

                        break;
                    case APPENDINDEX:
                        this->mainList->addRowAt(this->mainList->getIndex(), track);
                        this->mainList->scrollToItem(this->mainList->getItem(this->mainList->getIndex(), Bae::KEY::TITLE), QAbstractItemView::PositionAtTop);

                        break;
                    }
                }
            }

        }else
        {
            for(auto track : mapList)
                switch(pos)
                {
                case APPENDBOTTOM:
                    this->mainList->addRow(track);
                    this->mainList->scrollToBottom();

                    break;
                case APPENDTOP:
                    this->mainList->addRowAt(0,track);
                    this->mainList->scrollToItem(this->mainList->getItem(0, Bae::KEY::TITLE), QAbstractItemView::PositionAtTop);

                    break;
                case APPENDAFTER:
                    this->mainList->addRowAt(this->current_song_pos+1, track);
                    this->mainList->scrollToItem(this->mainList->getItem(this->current_song_pos+1, Bae::KEY::TITLE), QAbstractItemView::PositionAtTop);

                    break;
                case APPENDBEFORE:
                    this->mainList->addRowAt(this->current_song_pos, track);
                    this->mainList->scrollToItem(this->mainList->getItem(this->current_song_pos, Bae::KEY::TITLE), QAbstractItemView::PositionAtTop);

                    break;
                case APPENDINDEX:
                    this->mainList->addRowAt(this->mainList->getIndex(), track);
                    this->mainList->scrollToItem(this->mainList->getItem(mainList->getIndex(),Bae::KEY::TITLE),QAbstractItemView::PositionAtTop);

                    break;
                }
        }

        if(stopped)
        {
            qDebug()<<this->current_song_pos;
            this->mainList->setCurrentCell(this->current_song_pos, static_cast<int>(Bae::KEY::TITLE));
            this->album_art->setVisible(true);
            ui->controls->setVisible(true);
            ui->frame_4->setVisible(true);
            this->seekBar->setVisible(true);
            ui->frame_5->setVisible(true);
            this->loadTrack();
            this->stopped = !this->stopped;
        }
    }
}

void  BabeWindow::clearCurrentList()
{
    mainList->flushTable();
}

void BabeWindow::runSearch()
{
    if(!ui->search->text().isEmpty())
    {
        qDebug()<<"Looking for";
        QStringList searchList=ui->search->text().split(",");
        auto searchResults = searchFor(searchList);

        if(!searchResults.isEmpty())
        {
            switch(this->viewMode)
            {
            case FULLMODE:
            {
                resultsTable->flushTable();
                albumsTable->filter(searchResults,Bae::KEY::ALBUM);
                artistsTable->filter(searchResults,Bae::KEY::ARTIST);
                populateResultsTable(searchResults);
                break;

            }
            case PLAYLISTMODE:
            {
                filterList->flushTable();
                mainListView->setCurrentIndex(FILTERLIST);
                filterList->populateTableView(searchResults);
                break;
            }
            default: break;
            }


        }else
        {
            switch(this->viewMode)
            {
            case FULLMODE:
            {
                this->resultsTable->flushTable();
                this->resultsTable->setAddMusicMsg("Nothing on: \n"+ui->search->text());
                break;

            }
            case PLAYLISTMODE:
            {
                this->filterList->setAddMusicMsg("Nothing on: \n"+ui->search->text());
                this->filterList->flushTable();
                break;
            }
            default: break;
            }
        }


    }else
    {
        switch(this->viewMode)
        {
        case FULLMODE:
        {
            albumsTable->hide_all(false);
            artistsTable->hide_all(false);
            resultsTable->flushTable();
            if(views->currentIndex()!=ALBUMS||views->currentIndex()!=ARTISTS)
            {
                if(prevIndex==RESULTS) views->setCurrentIndex(COLLECTION);
                else  views->setCurrentIndex(prevIndex);
            }

            break;
        }
        case PLAYLISTMODE:
        {
            filterList->flushTable();
            mainListView->setCurrentIndex(MAINPLAYLIST);
            break;
        }
        default: break;
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

    this->mainList->flushTable();
    this->addToPlaylist(mapList,true,APPENDBOTTOM);
    mainList->removeRepeated();

    this->current_song_pos=0;
    this->prev_song_pos=current_song_pos;

    //    this->player->stop();
}

void BabeWindow::on_goBackBtn_clicked()
{
    expand();
}

void BabeWindow::calibrateMainList()
{
    clearMainList();
    populateMainList();
    mainList->scrollToTop();

    if(mainList->rowCount()>0)
    {
        this->mainList->setCurrentCell(current_song_pos,static_cast<int>(Bae::KEY::TITLE));
        this->mainList->getItem(current_song_pos,Bae::KEY::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));
        this->mainList->removeRepeated();
    }
}

void BabeWindow::on_miniPlaybackBtn_clicked()
{
    if(!this->miniPlayback)
    {
        if(!ui->controls->isVisible())
            ui->controls->setVisible(true);

        if(!ui->miniPlaybackBtn->isVisible())
            ui->miniPlaybackBtn->setVisible(true);

        ui->controls->setFixedHeight(ui->controls->minimumSizeHint().height()-40);
        this->album_art->setFixedHeight(ui->controls->minimumSizeHint().height()-40);
        ui->miniPlaybackBtn->setIcon(QIcon::fromTheme("go-bottom"));

        this->miniPlayback = !this->miniPlayback;
        this->blurWidget(*this->album_art, 28);
        this->album_art->saturatePixmap(100);

        if(viewMode==MINIMODE)  this->setMaximumHeight(ui->controls->minimumSizeHint().height()-40);


    }else
    {
        ui->controls->setFixedHeight(static_cast<int>(ALBUM_SIZE));
        this->album_art->setFixedHeight(static_cast<int>(ALBUM_SIZE));
        ui->miniPlaybackBtn->setIcon(QIcon::fromTheme("go-top"));
        this->miniPlayback = !this->miniPlayback;

        this->blurWidget(*this->album_art,15);
        this->album_art->restoreSaturation();

        if(this->viewMode == MINIMODE) this->setMaximumHeight(ui->controls->height());
    }

    ui->controls->update();
}

void BabeWindow::blurWidget(BabeAlbum &widget, const int &radius )
{
    QGraphicsBlurEffect* effect	= new QGraphicsBlurEffect(this);

    effect->setBlurRadius(radius);

    widget.setGraphicsEffect(effect);

}
