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

#include <QMessageBox>

#include "../dialogs/moodform.h"
#include "../widget_models/babetable.h"
#include "../widget_models/babealbum.h"
#include "../pulpo/pulpo.h"
#include "playlistsview.h"
#include "infoview.h"
#include "rabbitview.h"
#include "albumsview.h"
#include "../widget_models/babegrid.h"
#include "../dialogs/about.h"
#include "../kde/notify.h"
#include "../settings/settings.h"
#include "../db/collectionDB.h"
#include "../data_models/tracklist.h"

/*Global ststic objects*/
CollectionDB *BabeWindow::connection = new CollectionDB();
Notify *BabeWindow::nof = new Notify;

BabeWindow::BabeWindow(const QStringList &files, QWidget *parent) : QMainWindow(parent),
    ui(new Ui::BabeWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    this->setWindowIcon(QIcon(":Data/data/48-apps-babe.svg"));
    this->setWindowIconText("Babe...");
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->defaultWindowFlags = this->windowFlags();
    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    this->player = new QMediaPlayer(this);
    this->player->setVolume(100);

    //* SETUP MAIN COVER ARTWORK *//
    this->album_art = new BabeAlbum(BAE::DB{{BAE::KEY::ARTWORK, ":Data/data/babe.png"}}, BAE::AlbumSizeHint::BIG_ALBUM, 0, false, this);
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

    this->setGeometry(BAE::loadSettings("GEOMETRY", "MAINWINDOW", defaultGeometry).toRect());

    BAE::saveSettings("GEOMETRY", this->geometry(), "MAINWINDOW");

    //* SETUP BABE PARTS *//
    this->setUpViews();
    this->setUpPlaylist();
    this->setUpRightFrame();
    this->setUpCollectionViewer();
    this->setUpMenuBar();

    this->nof = new Notify(this);
    connect(this->nof,&Notify::babeSong,[this](const BAE::DB &track)
    {
        if(this->babeTrack(track))
            this->babedIcon(this->isBabed(track));
    });

    connect(this->nof,&Notify::skipSong,this,&BabeWindow::next);

    this->updater = new QTimer(this);
    connect(this->updater, &QTimer::timeout, this, &BabeWindow::update);

    //* LOAD OPENED FILES*/
    if(!files.isEmpty())
    {
        this->appendFiles(files, APPEND::APPENDTOP);
        this->current_song_pos = 0;

    }else this->current_song_pos = BAE::loadSettings("PLAYLIST_POS","MAINWINDOW",QVariant(0)).toInt();

    if(BAE::loadSettings("MINIPLAYBACK","MAINWINDOW",false).toBool())
        emit ui->miniPlaybackBtn->clicked();

    this->movePanel(static_cast<POSITION>(BAE::loadSettings("PANEL_POS","MAINWINDOW", RIGHT).toInt()));
    this->setToolbarIconSize(static_cast<uint>(BAE::loadSettings("TOOLBAR_ICON_SIZE", "MAINWINDOW", QVariant(16)).toInt()));

    this->loadStyle();
}


BabeWindow::~BabeWindow()
{
    //BabeWindow::connection->closeConnection();
    delete BabeWindow::connection;
    delete BabeWindow::nof;
    delete ui;
}

void BabeWindow::start()
{   
    this->settings_widget->checkCollection();

    auto savedList = BAE::loadSettings("PLAYLIST","MAINWINDOW",{}).toStringList();

    if(!savedList.isEmpty())
        this->addToPlaylist(BabeWindow::connection->getDBData(savedList), false, APPEND::APPENDBOTTOM);
    else this->populateMainList();

    this->refreshTables({{BAE::TABLE::TRACKS, true}, {BAE::TABLE::ALBUMS, true}, {BAE::TABLE::ARTISTS, true}, {BAE::TABLE::PLAYLISTS, true}});

    if(this->mainList->rowCount() > 0)
    {
        this->mainList->setCurrentCell(this->current_song_pos >= this->mainList->rowCount()? 0 : this->current_song_pos, static_cast<int>(BAE::KEY::TITLE));
        this->collectionView();
        this->go_playlistMode();

    }else if(this->collectionTable->rowCount() > 0)
    {
        this->collectionView();

    }else
    {
        this->stop();
        this->settingsView();
    }
}

//*HERE THE MAIN VIEWS GET SETUP WITH THEIR SIGNALS AND SLOTS**//
void BabeWindow::setUpViews()
{

    this->settings_widget = new settings(this); //this needs to go first
    this->settings_widget->readSettings();

    connect(this->settings_widget, &settings::refreshTables, this, &BabeWindow::refreshTables);

    //    BabeWindow::connection->openDB();
    //    connect(&this->connection, &CollectionDB::trackInserted, [this]()
    //    {
    //        this->settings_widget->collectionWatcher();
    //        emit this->settings_widget->refreshTables({{TABLE::TRACKS, true},{TABLE::ALBUMS, false},{TABLE::ARTISTS, false},{TABLE::PLAYLISTS, true}});
    //        this->settings_widget->fetchArt();
    //    });

    connect(this->connection, &CollectionDB::artistsCleaned, [this](int amount)
    {
        this->nof->notify(QString::number(amount)+tr(" Artists cleaned up"), "");
    });

    connect(this->connection, &CollectionDB::albumsCleaned, [this](int amount)
    {
        this->nof->notify(QString::number(amount)+tr(" Albums cleaned up"), "");
    });

    this->playlistTable = new PlaylistsView(this);
    connect(this->playlistTable->table, &BabeTable::tableWidget_doubleClicked, [this] (const BAE::DB_LIST &list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDBOTTOM);
    });
    connect(this->playlistTable->table, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->playlistTable->table, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->playlistTable->table, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->playlistTable->table, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->playlistTable->table, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->playlistTable->table, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->playlistTable->table, &BabeTable::appendIt, [this] (const BAE::DB_LIST &list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDAFTER);
    });
    connect(this->playlistTable->table, &BabeTable::saveToPlaylist, this->playlistTable, &PlaylistsView::saveToPlaylist);

    this->collectionTable = new BabeTable(this);
    this->collectionTable->setFrameShape(QFrame::StyledPanel);
    this->collectionTable->setFrameShadow(QFrame::Sunken);
    this->collectionTable->showColumn(static_cast<int>(BAE::KEY::STARS));
    this->collectionTable->showColumn(static_cast<int>(BAE::KEY::GENRE));
    connect(this->collectionTable,&BabeTable::tableWidget_doubleClicked, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDBOTTOM);
    });
    connect(this->collectionTable, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->collectionTable, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->collectionTable, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->collectionTable, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->collectionTable, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->collectionTable, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->collectionTable, &BabeTable::appendIt, [this] (const BAE::DB_LIST &list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDAFTER);
    });
    connect(this->collectionTable, &BabeTable::saveToPlaylist, this->playlistTable, &PlaylistsView::saveToPlaylist);

    this->mainList = new BabeTable(this);
    this->mainList->setObjectName("mainList");
    this->mainList->hideColumn(static_cast<int>(BAE::KEY::ALBUM));
    this->mainList->hideColumn(static_cast<int>(BAE::KEY::ARTIST));
    this->mainList->hideColumn(static_cast<int>(BAE::KEY::DURATION));
    this->mainList->horizontalHeader()->setVisible(false);
    this->mainList->enableRowColoring(true);
    this->mainList->enableRowDragging(true);
    this->mainList->enablePreview(false);
    this->mainList->setAddMusicMsg(tr("\nDrag and drop music here!"),"face-ninja");

    connect(this->mainList,&BabeTable::indexesMoved,[this](const int &row, const int &newRow)
    {
        if(row > this->current_song_pos && newRow < this->current_song_pos )
        {
            this->current_song_pos++;
            this->prev_song_pos++;

        }else if(row < this->current_song_pos && newRow >= this->current_song_pos)
        {
            this->current_song_pos--;
            this->prev_song_pos--;
        }
    });

    connect(mainList,&BabeTable::indexRemoved,[this](int row)
    {
        if(row < this->current_song_pos)
        {
            this->current_song_pos--;
            this->prev_song_pos--;
        }
    });

    connect(this->mainList, &BabeTable::tableWidget_doubleClicked, this, &BabeWindow::on_mainList_clicked);
    connect(this->mainList, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->mainList, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->mainList, &BabeTable::moodIt_clicked, this->mainList, &BabeTable::colorizeRow);
    connect(this->mainList, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->mainList, &BabeTable::saveToPlaylist, this->playlistTable, &PlaylistsView::saveToPlaylist);

    this->queueList = new BabeTable(this);
    this->queueList->setVisible(false);

    auto bgcolor= QColor(this->queueList->palette().color(QPalette::Background).name()).dark(120).name();
    this->queueList->setStyleSheet(QString("QTableView{background-color: %1}").arg(bgcolor));

    this->queueList->setObjectName("queueList");
    this->queueList->hideColumn(static_cast<int>(BAE::KEY::ALBUM));
    this->queueList->hideColumn(static_cast<int>(BAE::KEY::ARTIST));
    this->queueList->hideColumn(static_cast<int>(BAE::KEY::DURATION));
    this->queueList->horizontalHeader()->setVisible(false);
    this->queueList->enableRowColoring(true);
    this->queueList->enableRowDragging(true);
    this->queueList->enablePreview(true);
    this->queueList->setAddMusicMsg(tr("\nQueue list"),"face-ninja");

    connect(this->queueList, &BabeTable::tableWidget_doubleClicked, [this] (BAE::DB_LIST list)
    {
        Q_UNUSED(list);
        this->playQueuedTrack(this->queueList->getIndex());
        this->next();
    });

    connect(this->queueList, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->queueList, &BabeTable::moodIt_clicked, this->mainList, &BabeTable::colorizeRow);
    connect(this->queueList, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->queueList, &BabeTable::saveToPlaylist, this->playlistTable, &PlaylistsView::saveToPlaylist);

    this->mainListView = new QStackedWidget(this);
    this->mainListView->setFrameShape(QFrame::NoFrame);

    auto splitter = new QSplitter(this);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Vertical);

    splitter->addWidget(this->mainList);
    splitter->addWidget(this->queueList);

    splitter->setSizes({0,0});
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    this->mainListView->addWidget(splitter);

    this->resultsTable = new BabeTable(this);
    this->resultsTable->setFrameShape(QFrame::StyledPanel);
    this->resultsTable->setFrameShadow(QFrame::Sunken);
    this->resultsTable->horizontalHeader()->setHighlightSections(true);
    this->resultsTable->showColumn(static_cast<int>(BAE::KEY::STARS));
    this->resultsTable->showColumn(static_cast<int>(BAE::KEY::GENRE));

    connect(this->resultsTable, &BabeTable::tableWidget_doubleClicked, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDBOTTOM);
    });

    connect(this->resultsTable, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->resultsTable, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->resultsTable, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->resultsTable, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->resultsTable, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->resultsTable, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->resultsTable, &BabeTable::appendIt, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDAFTER);
    });
    connect(this->resultsTable, &BabeTable::saveToPlaylist, playlistTable, &PlaylistsView::saveToPlaylist);

    this->albumsTable = new AlbumsView(false, this);
    connect(this->albumsTable->albumTable, &BabeTable::tableWidget_doubleClicked, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDBOTTOM);
    });
    connect(this->albumsTable->albumTable, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->albumsTable->albumTable, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->albumsTable->albumTable, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->albumsTable->albumTable, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->albumsTable->albumTable, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->albumsTable->albumTable, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->albumsTable->albumTable, &BabeTable::appendIt, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDAFTER);
    });
    connect(this->albumsTable->albumTable, &BabeTable::saveToPlaylist, playlistTable, &PlaylistsView::saveToPlaylist);
    connect(this->albumsTable, &AlbumsView::playAlbum, this, &BabeWindow::putAlbumOnPlay);
    connect(this->albumsTable, &AlbumsView::babeAlbum, this, &BabeWindow::babeAlbum);
    connect(this->albumsTable, &AlbumsView::albumDoubleClicked, this, &BabeWindow::albumDoubleClicked);
    connect(this->albumsTable, &AlbumsView::expandTo, [this] (const QString &artist)
    {
        if(!artist.isEmpty())
        {
            emit ui->artists_view->clicked();
            this->artistsTable->showAlbumInfo({{BAE::KEY::ARTIST,artist}});
        }
    });

    this->artistsTable = new AlbumsView(true, this);
    this->artistsTable->expandBtn->setVisible(false);
    this->artistsTable->albumTable->showColumn(static_cast<int>(BAE::KEY::ALBUM));
    connect(this->artistsTable->albumTable, &BabeTable::tableWidget_doubleClicked, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDBOTTOM);
    });
    connect(this->artistsTable->albumTable, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->artistsTable->albumTable, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->artistsTable->albumTable, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->artistsTable->albumTable, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->artistsTable->albumTable, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->artistsTable->albumTable, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->artistsTable->albumTable, &BabeTable::appendIt, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDAFTER);
    });
    connect(this->artistsTable->albumTable, &BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);
    connect(this->artistsTable, &AlbumsView::playAlbum, this, &BabeWindow::putAlbumOnPlay);
    connect(this->artistsTable, &AlbumsView::babeAlbum, this, &BabeWindow::babeAlbum);
    connect(this->artistsTable, &AlbumsView::albumDoubleClicked, this, &BabeWindow::albumDoubleClicked);

    this->infoTable = new InfoView(this);
    connect(this->infoTable, &InfoView::playAlbum, this,&BabeWindow::putAlbumOnPlay);
    connect(this->infoTable, &InfoView::similarBtnClicked, [this](QStringList queries)
    {
        this->ui->search->setText(queries.join(","));
    });
    connect(this->infoTable, &InfoView::tagsBtnClicked, [this](QStringList queries)
    {
        this->ui->search->setText(queries.join(","));
    });
    connect(this->infoTable, &InfoView::tagClicked, [this](QString query)
    {
        this->ui->search->setText(query);
    });
    connect(this->infoTable, &InfoView::similarArtistTagClicked, [this](QString query)
    {
        this->ui->search->setText(query);
    });

    this->rabbitTable = new RabbitView(this);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::tableWidget_doubleClicked, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDBOTTOM);
    });
    connect(this->rabbitTable->generalSuggestion, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::previewFinished, this ,&BabeWindow::play);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->rabbitTable->generalSuggestion, &BabeTable::appendIt, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDAFTER);
    });
    connect(this->rabbitTable->generalSuggestion, &BabeTable::saveToPlaylist, playlistTable, &PlaylistsView::saveToPlaylist);
    connect(this->rabbitTable->artistSuggestion, &BabeGrid::playAlbum, this,&BabeWindow::putAlbumOnPlay);
    connect(this->rabbitTable->filterList, &BabeTable::tableWidget_doubleClicked, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDBOTTOM);
    });
    connect(this->rabbitTable->filterList, &BabeTable::babeIt_clicked, this, &BabeWindow::babeIt);
    connect(this->rabbitTable->filterList, &BabeTable::queueIt_clicked, this, &BabeWindow::addToQueue);
    connect(this->rabbitTable->filterList, &BabeTable::infoIt_clicked, this, &BabeWindow::infoIt);
    connect(this->rabbitTable->filterList, &BabeTable::previewStarted, this, &BabeWindow::pause);
    connect(this->rabbitTable->filterList, &BabeTable::previewFinished, this, &BabeWindow::play);
    connect(this->rabbitTable->filterList, &BabeTable::playItNow, this, &BabeWindow::playItNow);
    connect(this->rabbitTable->filterList, &BabeTable::appendIt, [this] (BAE::DB_LIST list)
    {
        this->addToPlaylist(list, false, APPEND::APPENDAFTER);
    });
    connect(this->rabbitTable->filterList, &BabeTable::saveToPlaylist,playlistTable,&PlaylistsView::saveToPlaylist);

    /* THE BUTTONS VIEWS */
    connect(ui->tracks_view, &QToolButton::clicked, this, &BabeWindow::collectionView);
    connect(ui->albums_view, &QToolButton::clicked, this, &BabeWindow::albumsView);
    connect(ui->artists_view, &QToolButton::clicked, this, &BabeWindow::artistsView);
    connect(ui->playlists_view, &QToolButton::clicked, this, &BabeWindow::playlistsView);
    connect(ui->rabbit_view, &QToolButton::clicked, this, &BabeWindow::rabbitView);
    connect(ui->info_view, &QToolButton::clicked, this, &BabeWindow::infoView);

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

    this->mainToolbar = new QToolBar(tr("Views"), this);
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
    this->mainToolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);

    this->mainToolbar->addWidget(this->ui->info_view);
    this->mainToolbar->addWidget(ui->spacer1);
    this->mainToolbar->addWidget(this->ui->tracks_view);
    this->mainToolbar->addWidget(this->ui->albums_view);
    this->mainToolbar->addWidget(this->ui->artists_view);
    this->mainToolbar->addWidget(this->ui->playlists_view);
    this->mainToolbar->addWidget(ui->spacer2);
    this->mainToolbar->addWidget(this->ui->rabbit_view);
    this->mainToolbar->addWidget(this->ui->settings_view);
    this->mainToolbar->actions()[this->mainToolbar->actions().size()-1]->setVisible(false);

    this->searchTimer = new QTimer(this);
    this->searchTimer->setSingleShot(true);
    this->searchTimer->setInterval(500);
    connect(this->searchTimer, &QTimer::timeout,this, &BabeWindow::runSearch);
    connect(this->ui->search, SIGNAL(textChanged(QString)), this->searchTimer, SLOT(start()));

    ui->search->setClearButtonEnabled(true);

    this->viewsWidget = new QWidget(this);
    auto viewsLayout = new QVBoxLayout;
    viewsLayout->setContentsMargins(0, 0, 0, 0);
    viewsLayout->setMargin(0);
    this->viewsWidget->setLayout(viewsLayout);

    viewsLayout->addWidget(this->views);
    viewsLayout->addWidget(this->ui->search);

    this->mainLayout->addWidget(this->viewsWidget);
    this->mainLayout->addWidget(this->rightFrame);
    this->mainLayout->setSpacing(6);

    this->mainWidget= new QWidget(this);
    this->views->setMinimumHeight(static_cast<int>(ALBUM_SIZE)*2);

    this->mainWidget->setLayout(mainLayout);

    this->addToolBar(static_cast<Qt::ToolBarArea>(BAE::loadSettings("TOOLBAR_POS","MAINWINDOW",Qt::ToolBarArea::LeftToolBarArea).toInt()), this->mainToolbar);

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
    this->ui->controls->setStyleSheet(QString("QWidget#controls{"
                                              "background:qlineargradient("
                                              "spread:pad, x1:0, y1:0, x2:0, y2:1, "
                                              "stop:0 rgba(%1,%2,%3,100%),"
                                              "stop:1 rgba(%1,%2,%3,40%))}").arg(QString::number(controlsColor.red()),
                                                                                 QString::number(controlsColor.green()),
                                                                                 QString::number(controlsColor.blue())));

    this->seekBar = new QSlider(this);
    this->seekBar->installEventFilter(this);
    connect(this->seekBar, &QSlider::sliderMoved, this, &BabeWindow::on_seekBar_sliderMoved);

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

    ui->hide_sidebar_btn->setToolTip(tr("Go Mini"));
    ui->shuffle_btn->setToolTip(tr("Shuffle"));

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
        case POSITION::RIGHT:
        {
            this->mainLayout->removeWidget(this->rightFrame);
            this->mainLayout->insertWidget(1,this->rightFrame);

            this->playlistPos = POSITION::RIGHT;
            break;
        }
        case POSITION::LEFT:
        {
            this->mainLayout->removeWidget(this->rightFrame);
            this->mainLayout->insertWidget(0,this->rightFrame);

            this->playlistPos = POSITION::LEFT;
            break;
        }
        case POSITION::OUT:
        {
            if(this->viewMode != VIEW_MODE::FULLMODE) expand();

            this->mainLayout->removeWidget(this->rightFrame);

            this->rightFrame->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
            this->rightFrame->setWindowTitle(tr("Playlist"));

            this->rightFrame->show();
            this->rightFrame->setMinimumHeight(static_cast<int>(ALBUM_SIZE)*2);
            this->rightFrame->window()->setFixedWidth(this->rightFrame->minimumSizeHint().width());
            this->rightFrame->window()->move(position.x()+this->size().width() - this->rightFrame->size().width(), this->pos().y());

            this->playlistSta = POSITION::OUT;
            break;
        }
        case POSITION::IN:
        {
            this->rightFrame->setWindowFlags(Qt::Widget);

            this->mainLayout->insertWidget(this->playlistPos == POSITION::RIGHT? 1 : 0, this->rightFrame);
            this->rightFrame->setFixedWidth(this->rightFrame->minimumSizeHint().width());
            this->rightFrame->setMinimumHeight(0);

            this->rightFrame->show();
            this->playlistSta = POSITION::IN;
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

    this->popPanel = new QAction(tr("Pop panel out"), this);
    this->popPanel->setShortcut(QKeySequence("Ctrl+p"));
    connect(popPanel, &QAction::triggered, [this]()
    {
        if(this->playlistSta == POSITION::OUT)
        {
            this->movePanel(POSITION::IN);
            this->popPanel->setText(tr("Pop panel out"));
        }
        else if (this->playlistSta == POSITION::IN)
        {
            this->movePanel(POSITION::OUT);
            this->popPanel->setText(tr("Pop panel in"));
        }
    });

    auto moveIt= new QAction(tr("Move to left"), this);
    moveIt->setShortcut(QKeySequence("Ctrl+m"));
    connect(moveIt, &QAction::triggered,[=]()
    {
        if(this->playlistSta == POSITION::OUT) emit this->popPanel->triggered(true);

        if(this->playlistPos == POSITION::RIGHT)
        {
            this->movePanel(POSITION::LEFT);
            moveIt->setText(tr("Move to right"));

        }else if(this->playlistPos == POSITION::LEFT)
        {
            this->movePanel(POSITION::RIGHT);
            moveIt->setText(tr("Move to left"));
        }
    });

    auto hideTimeLabels = new QAction(tr("Hide time labels"), this);
    connect (hideTimeLabels, &QAction::triggered, [hideTimeLabels, this]()
    {
        if(ui->time->isVisible() && ui->duration->isVisible())
        {
            ui->time->setVisible(false);
            ui->duration->setVisible(false);
            hideTimeLabels->setText(tr("Show time labels"));
        }else
        {
            ui->time->setVisible(true);
            ui->duration->setVisible(true);
            hideTimeLabels->setText(tr("Hide time labels"));
        }
    });

    auto refreshIt = new QAction(tr("Calibrate..."), this);
    refreshIt->setShortcut(QKeySequence("Ctrl+b"));
    refreshIt->setToolTip(tr("Clean & play Babe'd tracks"));
    connect(refreshIt, &QAction::triggered, [this]() { calibrateMainList(); });

    auto clearIt = new QAction(tr("Clear out..."), this);
    clearIt->setShortcut(QKeySequence("Ctrl+e"));
    clearIt->setToolTip(tr("Remove unselected tracks"));
    connect(clearIt, &QAction::triggered, [this]()
    {
        this->clearMainList();
        if(this->mainList->rowCount() > 0)
        {
            this->mainList->setCurrentCell(this->current_song_pos, static_cast<int>(BAE::KEY::TITLE));
            this->mainList->getItem(this->current_song_pos, BAE::KEY::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));
        }
    });

    auto cleanIt = new QAction(tr("Clean..."),this);
    cleanIt->setShortcut(QKeySequence("Ctrl+c"));
    cleanIt->setToolTip(tr("Remove repeated tracks"));
    connect(cleanIt, &QAction::triggered, [this]()
    {
        this->mainList->removeRepeated();
    });

    auto clearQueue = new QAction(tr("Clear Queue..."), this);
    clearQueue->setToolTip(tr("Remove queued tracks"));
    connect(clearQueue, &QAction::triggered, [this]()
    {
        this->removeQueuedTracks();
    });

    auto open = new QAction(tr("Open..."), this);
    open->setShortcut(QKeySequence("Ctrl+o"));
    connect(open, &QAction::triggered, this, &BabeWindow::on_open_btn_clicked);

    auto fileMenu = this->menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(open);

    auto panelMenu = this->menuBar()->addMenu(tr("&Panel"));
    panelMenu->addAction(moveIt);
    panelMenu->addAction(popPanel);
    panelMenu->addAction(hideTimeLabels);

    auto playlistMenu = this->menuBar()->addMenu(tr("&Playlist"));
    playlistMenu->addAction(refreshIt);
    playlistMenu->addAction(clearIt);
    playlistMenu->addAction(cleanIt);
    playlistMenu->addAction(refreshIt);
    playlistMenu->addAction(clearQueue);


    auto toolBarText = new QAction(tr("Show text"), this);
    connect(toolBarText, &QAction::triggered, [toolBarText,this]()
    {
        switch(this->mainToolbar->toolButtonStyle())
        {
            case Qt::ToolButtonStyle::ToolButtonIconOnly:
                this->mainToolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
                toolBarText->setText(tr("Hide text"));
                break;
            case Qt::ToolButtonStyle::ToolButtonTextBesideIcon:
                this->mainToolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
                toolBarText->setText(tr("Show text"));
                break;
            default:break;
        }

        this->mainToolbar->update();
    });


    auto toolBarMenu = this->menuBar()->addMenu(tr("&Toolbar"));
    toolBarMenu->addAction(toolBarText);

    auto sizes = toolBarMenu->addMenu(tr("Icon size"));
    auto iconSize16 = new QAction("16", this);
    auto iconSize22 = new QAction("22", this);
    auto iconSize32 = new QAction("32", this);
    sizes->addAction(iconSize16);
    sizes->addAction(iconSize22);
    sizes->addAction(iconSize32);

    connect(sizes, &QMenu::triggered, [this](QAction *action)
    {
        auto size = action->text().toInt();
        this->setToolbarIconSize(static_cast<uint>(size));
    });

    auto position = toolBarMenu->addMenu(tr("Position"));
    auto top = new QAction(tr("Top"), this);
    auto bottom = new QAction(tr("Bottom"), this);
    auto left = new QAction(tr("Left"), this);
    auto right = new QAction(tr("Right"), this);
    position->addAction(top);
    position->addAction(bottom);
    position->addAction(left);
    position->addAction(right);

    connect(position, &QMenu::triggered, [this](QAction *action)
    {
        auto area = Qt::ToolBarArea::LeftToolBarArea;

        if(action->text()=="Top")  area =  Qt::ToolBarArea::TopToolBarArea;
        else if(action->text()=="Bottom")  area =  Qt::ToolBarArea::BottomToolBarArea;
        else if(action->text()=="Left")  area =  Qt::ToolBarArea::LeftToolBarArea;
        else if(action->text()=="Right")  area =  Qt::ToolBarArea::RightToolBarArea;

        this->removeToolBar(this->mainToolbar);
        this->addToolBar(area, this->mainToolbar);
        this->mainToolbar->show();
    });

    auto expandMode = new QAction(tr("Collection mode"), this);
    connect(expandMode, &QAction::triggered, this, &BabeWindow::expand);

    auto playlistMode = new QAction(tr("Playlist mode"), this);
    connect(playlistMode, &QAction::triggered, this, &BabeWindow::go_playlistMode);

    auto miniMode = new QAction(tr("Mini mode"), this);
    connect(miniMode, &QAction::triggered, this, &BabeWindow::go_mini);

    auto viewMenu = this->menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(expandMode);
    viewMenu->addAction(playlistMode);
    viewMenu->addAction(miniMode);

    auto moods = new QAction(tr("Assign Moods"), this);
    connect(moods, &QAction::triggered, [this]()
    {
        auto moodForm = new MoodForm(this);
        connect(moodForm, &MoodForm::saved, moodForm, &MoodForm::deleteLater);
        moodForm->show();
    });

    auto pulpoDeamon = new QAction(tr("Run Pulpo Deamon"), this);
    connect(pulpoDeamon, &QAction::triggered, this->settings_widget, &settings::fetchArt);

    auto refreshCollection = new QAction(tr("Refresh Collection"), this);
    connect(refreshCollection, &QAction::triggered, [this]
    {
        this->refreshTables({{TABLE::TRACKS, true}, {TABLE::ALBUMS, true}, {TABLE::ARTISTS, true}, {TABLE::PLAYLISTS, true}});

    });

    auto cleanCollection = new QAction(tr("Clean up Collection"), this);
    connect(cleanCollection, &QAction::triggered, []
    {
        if(BabeWindow::connection->cleanAlbums())
            BabeWindow::connection->cleanArtists();
    });

    auto settings = new QAction(tr("Settings"), this);
    connect(settings, &QAction::triggered, this, &BabeWindow::settingsView);

    auto toolsMenu = this->menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(moods);
    toolsMenu->addAction(pulpoDeamon);
    toolsMenu->addAction(refreshCollection);
    toolsMenu->addAction(cleanCollection);
    toolsMenu->addAction(settings);

    auto aboutBabe = new QAction(tr("About Babe"), this);
    connect(aboutBabe, &QAction::triggered, this->settings_widget->about_ui, &About::show);

    auto aboutQt = new QAction(tr("About Qt"), this);
    connect(aboutQt, &QAction::triggered,  qApp, &QApplication::aboutQt);

    auto helpMenu = this->menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutBabe);
    helpMenu->addAction(aboutQt);
}

void BabeWindow::albumDoubleClicked(const BAE::DB &info)
{
    auto artist = info[BAE::KEY::ARTIST];
    auto album = info[BAE::KEY::ALBUM];

    BAE::DB_LIST mapList;

    switch(BAE::albumType(info))
    {
        case TABLE::ARTISTS:
            mapList = BabeWindow::connection->getArtistTracks(artist);
            break;
        case TABLE::ALBUMS:
            mapList = BabeWindow::connection->getAlbumTracks(album,artist);
            break;
        default: break;
    }

    this->addToPlaylist(mapList, false, APPEND::APPENDBOTTOM);
}

void BabeWindow::playItNow(const BAE::DB_LIST &list)
{
    if(!list.isEmpty())
    {
        /* if in the list there's a single track, check its position on the mainPlaylist*/
        if(list.size() == 1)
        {
            auto pos = this->mainList->getAllTableContent().indexOf(list.first());

            if( pos != -1)
                this->mainList->setCurrentCell(pos, static_cast<int>(BAE::KEY::TITLE));
            else
            {
                this->addToPlaylist(list, false, APPEND::APPENDBOTTOM);
                this->mainList->setCurrentCell(mainList->rowCount()-list.size(), static_cast<int>(BAE::KEY::TITLE));
            }

        }else
        {
            this->addToPlaylist(list, false, APPEND::APPENDBOTTOM);
            this->mainList->setCurrentCell(this->mainList->rowCount()-list.size(), static_cast<int>(BAE::KEY::TITLE));
        }

        this->loadTrack();
    }

}

void BabeWindow::putAlbumOnPlay(const BAE::DB &info)
{
    if(!info.isEmpty())
    {
        BAE::DB_LIST mapList;

        switch(BAE::albumType(info))
        {
            case TABLE::ARTISTS:
                mapList = BabeWindow::connection->getArtistTracks(info[BAE::KEY::ARTIST]);
                break;
            case TABLE::ALBUMS:
                mapList = BabeWindow::connection->getAlbumTracks(info[BAE::KEY::ALBUM], info[BAE::KEY::ARTIST]);
                break;
            default:break;
        }

        if(!mapList.isEmpty()) this->putOnPlay(mapList);
    }

}

void BabeWindow::putOnPlay(const BAE::DB_LIST &mapList)
{
    if(!mapList.isEmpty())
    {
        this->mainList->flushTable();
        this->addToPlaylist(mapList, false, APPEND::APPENDBOTTOM);

        if(this->mainList->rowCount()>0)
        {
            this->current_song_pos=0;
            this->prev_song_pos = current_song_pos;
            this->mainList->setCurrentCell(this->current_song_pos, static_cast<int>(BAE::KEY::TITLE));

            this->loadTrack();
        }
    }
}

void BabeWindow::addToPlayed(const QString &url)
{
    qDebug()<<"Song totally played"<<url;
    if(BabeWindow::connection->check_existance(BAE::TABLEMAP[BAE::TABLE::TRACKS], BAE::KEYMAP[BAE::KEY::URL],url))
        BabeWindow::connection->playedTrack(url);
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

            BAE::DB_LIST mapList;
            QList<QUrl> urls = dropEvent->mimeData()->urls();

            if(urls.isEmpty())
            {
                auto info = dropEvent->mimeData()->text();
                auto infoList = info.split("/by/");

                if(infoList.size() == 2)
                {
                    auto artist = infoList.at(1).simplified();
                    auto album = infoList.at(0).simplified();

                    mapList = BabeWindow::connection->getAlbumTracks(album,artist);

                }else mapList = BabeWindow::connection->getArtistTracks(info);


                this->addToPlaylist(mapList, false, APPEND::APPENDBOTTOM);

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
        msgBox.setText(tr("Babe is still collecting important information about your collection."));
        msgBox.setInformativeText(tr("Do you want to quit?"));
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
        BAE::saveSettings("GEOMETRY",this->geometry(),"MAINWINDOW");

    BAE::saveSettings("PLAYLIST", this->mainList->getTableColumnContent(BAE::KEY::URL), "MAINWINDOW");
    BAE::saveSettings("PLAYLIST_POS", this->current_song_pos, "MAINWINDOW");
    BAE::saveSettings("TOOLBAR_ICON_SIZE", this->iconSize, "MAINWINDOW");
    BAE::saveSettings("MINIPLAYBACK", this->miniPlayback, "MAINWINDOW");
    BAE::saveSettings("PANEL_POS", this->playlistPos, "MAINWINDOW");
    BAE::saveSettings("TIME_LABEL", ui->time->isVisible() && ui->duration->isVisible(), "MAINWINDOW");
    BAE::saveSettings("TOOLBAR_POS", this->toolBarArea(this->mainToolbar), "MAINWINDOW");

    event->accept();

    QMainWindow::closeEvent(event);
}

void BabeWindow::refreshTables(const QMap<BAE::TABLE, bool> &tableReset)
{
    nof->notify("Loading collection","this might take some time depending on your colleciton size");
    for (auto table : tableReset.keys())
    {
        QString queryTxt;

        switch(table)
        {
            case BAE::TABLE::TRACKS:
            {
                if(tableReset[table]) this->collectionTable->flushTable();
                queryTxt = QString("SELECT * FROM %1 ORDER BY  %2").arg(BAE::TABLEMAP[BAE::TABLE::TRACKS],BAE::KEYMAP[BAE::KEY::ARTIST]);
                this->collectionTable->populateTableView(queryTxt);
                break;
            }

            case BAE::TABLE::ALBUMS:
            {
                if(tableReset[table]) this->albumsTable->flushView();
                queryTxt = QString("SELECT * FROM %1 ORDER BY  %2").arg(BAE::TABLEMAP[BAE::TABLE::ALBUMS],BAE::KEYMAP[BAE::KEY::ALBUM]);
                this->albumsTable->populate(queryTxt);
                this->albumsTable->hideAlbumFrame();
                break;
            }

            case BAE::TABLE::ARTISTS:
            {
                if(tableReset[table]) this->artistsTable->flushView();
                queryTxt = QString("SELECT * FROM %1 ORDER BY  %2").arg(BAE::TABLEMAP[BAE::TABLE::ARTISTS],BAE::KEYMAP[BAE::KEY::ARTIST]);
                this->artistsTable->populate(queryTxt);
                this->artistsTable->hideAlbumFrame();
                break;
            }

            case BAE::TABLE::PLAYLISTS:
            {
                this->playlistTable->list->clear();
                this->playlistTable->setDefaultPlaylists();
                this->playlistTable->setPlaylists();
                break;
            }

            default: continue;
        }
    }

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

void BabeWindow::fetchCoverArt(BAE::DB &song)
{
    if(BAE::artworkCache(song, KEY::ALBUM)) return;

    if(BAE::artworkCache(song, KEY::ARTIST)) return;

    Pulpo pulpo;
    pulpo.registerServices({SERVICES::LastFm, SERVICES::Spotify, SERVICES::MusicBrainz});
    pulpo.setOntology(PULPO::ONTOLOGY::ALBUM);
    pulpo.setInfo(PULPO::INFO::ARTWORK);

    QEventLoop loop;

    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(1000);

    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    connect(&pulpo, &Pulpo::infoReady, [&](const BAE::DB &track,const PULPO::RESPONSE  &res)
    {
        Q_UNUSED(track);
        if(!res[PULPO::ONTOLOGY::ALBUM][PULPO::INFO::ARTWORK].isEmpty())
        {
            auto artwork = res[PULPO::ONTOLOGY::ALBUM][PULPO::INFO::ARTWORK][PULPO::CONTEXT::IMAGE].toByteArray();
            BAE::saveArt(song,artwork, BAE::CachePath);
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

    else this->views->setCurrentIndex(VIEWS::COLLECTION);
    ui->tracks_view->setChecked(true);

    if(this->viewMode != VIEW_MODE::FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::albumsView()
{
    this->views->setCurrentIndex(VIEWS::ALBUMS);
    ui->albums_view->setChecked(true);

    if(this->viewMode != VIEW_MODE::FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::playlistsView()
{
    this->views->setCurrentIndex(VIEWS::PLAYLISTS);
    ui->playlists_view->setChecked(true);

    if(this->viewMode != VIEW_MODE::FULLMODE) this->expand();

    if(this->playlistTable->list->currentRow() == 0)
    {
        this->playlistTable->list->setCurrentRow(0);
        emit this->playlistTable->list->clicked(this->playlistTable->list->model()->index(0,0));
    }

    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::rabbitView()
{
    this->views->setCurrentIndex(VIEWS::RABBIT);
    ui->rabbit_view->setChecked(true);

    if(this->viewMode != VIEW_MODE::FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::infoView()
{
    this->views->setCurrentIndex(VIEWS::INFO);
    ui->info_view->setChecked(true);

    if(this->viewMode != VIEW_MODE::FULLMODE) this->expand();
    this->prevIndex = views->currentIndex();
}

void BabeWindow::artistsView()
{
    this->views->setCurrentIndex(VIEWS::ARTISTS);
    ui->artists_view->setChecked(true);

    if(this->viewMode != VIEW_MODE::FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::settingsView()
{
    this->views->setCurrentIndex(VIEWS::SETTINGS);
    ui->settings_view->setChecked(true);

    if(this->viewMode != VIEW_MODE::FULLMODE) this->expand();
    this->prevIndex = this->views->currentIndex();
}

void BabeWindow::expand()
{
    this->setMinimumSize(this->minimumSizeHint().width(),0);

    this->viewMode = VIEW_MODE::FULLMODE;

    if(!this->views->isVisible()) this->viewsWidget->setVisible(true);
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
    animation->setEndValue(BAE::loadSettings("GEOMETRY","MAINWINDOW", this->geometry()).toRect());

    animation->start();

    ui->hide_sidebar_btn->setToolTip(tr("Go Mini"));

}

void BabeWindow::go_mini()
{
    this->setMinimumSize(this->minimumSizeHint().width(), 0);
    this->setFixedWidth(rightFrame->minimumSizeHint().width()+12);

    this->viewMode = VIEW_MODE::MINIMODE;

    this->mainToolbar->setVisible(false);
    this->viewsWidget->setVisible(false);
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

    ui->hide_sidebar_btn->setToolTip(tr("Expand"));
}

void BabeWindow::go_playlistMode()
{
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    this->setMinimumHeight(0);
    this->setMinimumSize(this->minimumSizeHint().width(), 0);

    if(playlistSta != POSITION::OUT)
    {
        BAE::saveSettings("GEOMETRY",this->geometry(),"MAINWINDOW");
        this->viewMode = VIEW_MODE::PLAYLISTMODE;

        if(!ui->frame_4->isVisible()) ui->frame_4->setVisible(true);
        if(!mainList->isVisible()) mainListView->setVisible(true);
        if(!ui->frame_5->isVisible()) ui->frame_5->setVisible(true);

        this->mainToolbar->setVisible(false);

        this->setFixedWidth(rightFrame->minimumSizeHint().width()+12);

        auto animation = new QPropertyAnimation(this, "maximumWidth",this);
        connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);

        animation->setDuration(200);
        animation->setStartValue(this->size().width());
        animation->setEndValue(rightFrame->minimumSizeHint().width()+12);

        animation->start();

        this->viewsWidget->setVisible(false);

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
    if(this->playlistSta == POSITION::OUT)
        emit this->popPanel->triggered(true);

    switch(this->viewMode)
    {
        case VIEW_MODE::FULLMODE:
            this->go_playlistMode(); break;

        case VIEW_MODE::PLAYLISTMODE:
            this->go_mini(); break;

        case VIEW_MODE::MINIMODE:
            this->expand(); break;
    }
}

void BabeWindow::on_shuffle_btn_clicked() //TODO
{
    if(this->shuffle_state == PLAY_MODE::REGULAR)
    {
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-shuffle"));
        ui->shuffle_btn->setToolTip(tr("Repeat"));
        this->shuffle_state = PLAY_MODE::SHUFFLE;

    }else if (this->shuffle_state == PLAY_MODE::SHUFFLE)
    {
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-repeat"));
        ui->shuffle_btn->setToolTip("Consecutive");
        this->shuffle_state = PLAY_MODE::REGULAR;
    }
}

void BabeWindow::on_open_btn_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),QDir().homePath()+"/Music/", tr("Audio (*.mp3 *.wav *.mp4 *.flac *.ogg *.m4a)"));
    if(!files.isEmpty()) this->appendFiles(files);
}

void BabeWindow::appendFiles(const QStringList &paths, const APPEND &pos)
{
    if(!paths.isEmpty())
    {
        QStringList trackList;
        for(auto url : paths)
        {
            if(QFileInfo(url).isDir())
            {
                QDirIterator it(url, BAE::formats, QDir::Files, QDirIterator::Subdirectories);

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
    auto results = BabeWindow::connection->getBabedTracks();
    this->mainList->populateTableView(results);
    this->mainList->resizeRowsToContents();
}

void BabeWindow::on_mainList_clicked(const BAE::DB_LIST &list)
{
    Q_UNUSED(list);
    this->loadTrack();
}

void BabeWindow::loadTrack()
{
    if(stopped) this->updater->start(100);

    this->prev_song = this->current_song;
    this->prev_song_pos = this->current_song_pos;

    if(this->prev_song_pos < this->mainList->rowCount())
        this->mainList->getItem(prev_song_pos,BAE::KEY::TITLE)->setIcon(QIcon());

    this->current_song_pos = this->mainList->getIndex() >= 0 ? this->mainList->getIndex() : 0 ;
    this->current_song = this->mainList->getRowData(this->current_song_pos);

    this->mainList->getItem(this->current_song_pos,BAE::KEY::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));

    this->mainList->scrollTo(this->mainList->model()->index(this->current_song_pos, static_cast<int>(BAE::KEY::TITLE)));

    qDebug()<<"in mainlist=" << this->current_song[BAE::KEY::URL];

    if(BAE::fileExists(this->current_song[BAE::KEY::URL]))
    {
        this->player->setMedia(QUrl::fromLocalFile(this->current_song[BAE::KEY::URL]));
        this->play();

        this->album_art->setTitle(this->current_song[BAE::KEY::ARTIST], this->current_song[BAE::KEY::ALBUM]);

        this->babedIcon(this->isBabed(this->current_song));

        if(!this->isActiveWindow())
            this->nof->notifySong(current_song,QPixmap(current_song[BAE::KEY::ARTWORK]));

        this->loadMood();
        this->loadCover(this->current_song);

        if(this->miniPlayback)
        {
            this->blurWidget(*this->album_art, 28);
            this->album_art->saturatePixmap(100);
        }

        this->loadInfo(this->current_song);
        this->rabbitTable->seed(this->current_song);

    }else this->mainList->removeRow(this->current_song_pos);
}

int BabeWindow::isBabed(const BAE::DB &track)
{
    return BabeWindow::connection->getTrackBabe(track[BAE::KEY::URL]);
}

void BabeWindow::loadMood()
{
    auto color = BabeWindow::connection->getTrackArt(this->current_song[BAE::KEY::URL]);

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
    auto artist = track[BAE::KEY::ARTIST];
    auto album = track[BAE::KEY::ALBUM];
    auto title = track[BAE::KEY::TITLE];

    auto artistHead = BabeWindow::connection->getArtistArt(artist);

    if(!artistHead.isEmpty())
    {
        this->infoTable->setArtistArt(artistHead);
        this->infoTable->artist->setTitle(artist, "");

    }else this->infoTable->setArtistArt(QString(":Data/data/cover.svg"));

    auto albumArt = BabeWindow::connection->getAlbumArt(album, artist);

    this->current_song.insert(BAE::KEY::ARTWORK,albumArt.isEmpty()? "" : albumArt);

    if(!this->current_song[BAE::KEY::ARTWORK].isEmpty())
        this->album_art->putPixmap(this->current_song[BAE::KEY::ARTWORK]);

    else if (!artistHead.isEmpty())
    {
        this->current_song.insert(BAE::KEY::ARTWORK, artistHead);
        this->album_art->putPixmap(this->current_song[BAE::KEY::ARTWORK]);

    }else
    {
        this->fetchCoverArt(track);
        this->album_art->putPixmap(this->current_song[BAE::KEY::ARTWORK]);
    }

    return true;
}

void BabeWindow::addToQueue(const BAE::DB_LIST &tracks)
{
    for(auto track : tracks)
        this->queueList->addRow(track);

    this->queueList->setVisible(true);
    this->queueList->scrollToBottom();

    if(tracks.size() == 1)
        this->nof->notify("Song added to Queue",tracks[0][KEY::TITLE]);
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
        ui->time->setText(BAE::transformTime(player->position()/1000));
        ui->duration->setText(BAE::transformTime(player->duration()/1000));

        if(this->player->state() == QMediaPlayer::StoppedState)
        {
            this->prev_song = this->current_song;
            this->addToPlayed(prev_song[BAE::KEY::URL]);
            this->next();
        }

    }else
    {
        this->seekBar->setValue(0);
        this->seekBar->setEnabled(false);
    }
}

void BabeWindow::removeQueuedTrack(const int &pos)
{
    this->queueList->removeRow(pos);
    if(this->queueList->rowCount() < 1)
        this->queueList->setVisible(false);
}

void BabeWindow::removeQueuedTracks()
{
    this->queueList->flushTable();
    this->queueList->setVisible(false);
}

void BabeWindow::playQueuedTrack(const int &pos)
{
    this->addToPlaylist({this->queueList->getRowData(pos)}, false, APPEND::APPENDAFTER);
    this->removeQueuedTrack(pos);
}

void BabeWindow::next()
{
    int nextSong=0;

    if(this->queueList->rowCount()>0)
        this->playQueuedTrack(0);

    qDebug()<<"Tracks on queue:"<<this->queueList->rowCount();

    nextSong = this->current_song_pos+1;

    if(nextSong >= this->mainList->rowCount())
        nextSong = 0;

    this->mainList->setCurrentCell(this->shuffle_state == PLAY_MODE::SHUFFLE ? this->shuffleNumber(): nextSong, static_cast<int>(BAE::KEY::TITLE));

    this->loadTrack();
}

void BabeWindow::back()
{
    auto lCounter =  this->current_song_pos-1;

    if(lCounter < 0)
        lCounter =  this->mainList->rowCount() - 1;

    this->mainList->setCurrentCell(this->shuffle_state != PLAY_MODE::SHUFFLE ? lCounter :  this->shuffleNumber(), static_cast<int>(BAE::KEY::TITLE));

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
    this->setWindowTitle( this->current_song[BAE::KEY::TITLE]+" \xe2\x99\xa1 "+ this->current_song[BAE::KEY::ARTIST]);
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

void BabeWindow::babeAlbum(const BAE::DB &info)
{
    auto artist =info[BAE::KEY::ARTIST];
    auto album = info[BAE::KEY::ALBUM];

    BAE::DB_LIST mapList;

    switch(BAE::albumType(info))
    {
        case TABLE::ARTISTS:
            mapList = BabeWindow::connection->getArtistTracks(artist);
            break;
        case TABLE::ALBUMS:
            mapList = BabeWindow::connection->getAlbumTracks(album, artist);
            break;
        default: break;
    }

    if(!mapList.isEmpty())
        this->babeIt(mapList);
}

bool BabeWindow::unbabeIt(const BAE::DB &track)
{
    if(BabeWindow::connection->babeTrack(track[BAE::KEY::URL],0))
    {
        nof->notify("Song unBabe'd it",track[BAE::KEY::TITLE]+" by "+track[BAE::KEY::ARTIST]);
        return  true;
    }
    return false;
}

bool BabeWindow::babeTrack(const BAE::DB &track)
{
    auto url = track[BAE::KEY::URL];

    if(this->isBabed(track))
    {
        if(this->unbabeIt(track)) return true;
        return false;

    }else
    {
        if(BabeWindow::connection->check_existance(BAE::TABLEMAP[BAE::TABLE::TRACKS],BAE::KEYMAP[BAE::KEY::URL],url))
        {
            if(BabeWindow::connection->babeTrack(url,true))
            {
                this->nof->notify("Song Babe'd",track[BAE::KEY::TITLE]+" by "+track[BAE::KEY::ARTIST]);
                this->addToPlaylist({track}, true, APPEND::APPENDBOTTOM);
                return true;
            }
            return false;

        }else
        {
            ui->fav_btn->setEnabled(false);

            auto newTrack = track;
            newTrack.insert(BAE::KEY::BABE, "1");
            BabeWindow::connection->addTrack(newTrack);

            ui->fav_btn->setEnabled(true);

            return true;
        }
    }
}

void BabeWindow::loadInfo(const BAE::DB &track)
{
    auto lyrics = BabeWindow::connection->getTrackLyrics(track[BAE::KEY::URL]);
    auto albumTags = BabeWindow::connection->getAlbumTags(track[BAE::KEY::ALBUM],track[BAE::KEY::ARTIST]);
    auto albumWiki = BabeWindow::connection->getAlbumWiki(track[BAE::KEY::ALBUM],track[BAE::KEY::ARTIST]);
    auto artistWiki = BabeWindow::connection->getArtistWiki(track[BAE::KEY::ARTIST]);
    auto artistTags = BabeWindow::connection->getArtistTags(track[BAE::KEY::ARTIST]);

    this->infoTable->setTrack(track);

    this->infoTable->setLyrics(lyrics);
    this->infoTable->setAlbumInfo(albumWiki);
    this->infoTable->setArtistInfo(artistWiki);
    this->infoTable->setArtistTags(artistTags);
    this->infoTable->setAlbumTags(albumTags);
}

void BabeWindow::babeIt(const BAE::DB_LIST &tracks)
{
    for(auto track : tracks)
        if(!babeTrack(track)) qWarning()<<"couldn't Babe track:"<<track[BAE::KEY::URL];
}

void  BabeWindow::infoIt(const BAE::DB &track)
{
    infoView();
    this->loadInfo(track);
}

void BabeWindow::addToPlaylist(const BAE::DB_LIST &mapList, const bool &notRepeated, const APPEND &pos)
{
    if(!mapList.isEmpty())
    {
        if(notRepeated)
        {
            BAE::DB_LIST newList;
            QStringList alreadyInList = this->mainList->getTableColumnContent(BAE::KEY::URL);
            for(auto track: mapList)
            {
                if(!alreadyInList.contains(track[BAE::KEY::URL]))
                {
                    newList<<track;
                    switch(pos)
                    {
                        case APPEND::APPENDBOTTOM:
                            this->mainList->addRow(track);
                            this->mainList->scrollToBottom();

                            break;
                        case APPEND::APPENDTOP:
                            this->mainList->addRowAt(0, track);
                            this->mainList->scrollToItem(this->mainList->getItem(0, BAE::KEY::TITLE), QAbstractItemView::PositionAtTop);

                            break;
                        case APPEND::APPENDAFTER:
                            this->mainList->addRowAt(current_song_pos+1, track);
                            this->mainList->scrollToItem(this->mainList->getItem(current_song_pos+1, BAE::KEY::TITLE), QAbstractItemView::PositionAtTop);

                            break;
                        case APPEND::APPENDBEFORE:
                            this->mainList->addRowAt(current_song_pos, track);
                            this->mainList->scrollToItem(this->mainList->getItem(current_song_pos, BAE::KEY::TITLE), QAbstractItemView::PositionAtTop);

                            break;
                        case APPEND::APPENDINDEX:
                            this->mainList->addRowAt(this->mainList->getIndex(), track);
                            this->mainList->scrollToItem(this->mainList->getItem(this->mainList->getIndex(), BAE::KEY::TITLE), QAbstractItemView::PositionAtTop);

                            break;
                    }
                }
            }

        }else
        {
            for(auto track : mapList)
                switch(pos)
                {
                    case APPEND::APPENDBOTTOM:
                        this->mainList->addRow(track);
                        this->mainList->scrollToBottom();

                        break;
                    case APPEND::APPENDTOP:
                        this->mainList->addRowAt(0,track);
                        this->mainList->scrollToItem(this->mainList->getItem(0, BAE::KEY::TITLE), QAbstractItemView::PositionAtTop);

                        break;
                    case APPEND::APPENDAFTER:
                        this->mainList->addRowAt(this->current_song_pos+1, track);
                        this->mainList->scrollToItem(this->mainList->getItem(this->current_song_pos+1, BAE::KEY::TITLE), QAbstractItemView::PositionAtTop);

                        break;
                    case APPEND::APPENDBEFORE:
                        this->mainList->addRowAt(this->current_song_pos, track);
                        this->mainList->scrollToItem(this->mainList->getItem(this->current_song_pos, BAE::KEY::TITLE), QAbstractItemView::PositionAtTop);

                        break;
                    case APPEND::APPENDINDEX:
                        this->mainList->addRowAt(this->mainList->getIndex(), track);
                        this->mainList->scrollToItem(this->mainList->getItem(mainList->getIndex(),BAE::KEY::TITLE),QAbstractItemView::PositionAtTop);

                        break;
                }
        }

        if(stopped)
        {
            this->mainList->setCurrentCell(this->current_song_pos, static_cast<int>(BAE::KEY::TITLE));
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
        QStringList searchList = ui->search->text().split(",");
        auto searchResults = searchFor(searchList);

        if(!searchResults.isEmpty())
        {
            resultsTable->flushTable();
            albumsTable->filter(searchResults,BAE::KEY::ALBUM);
            artistsTable->filter(searchResults,BAE::KEY::ARTIST);
            populateResultsTable(searchResults);

        }else
        {
            this->resultsTable->flushTable();
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

void BabeWindow::populateResultsTable(const BAE::DB_LIST &mapList)
{
    if(views->currentIndex()!=ALBUMS&&views->currentIndex()!=ARTISTS)
        views->setCurrentIndex(RESULTS);
    resultsTable->populateTableView(mapList);
}

BAE::DB_LIST BabeWindow::searchFor(const QStringList &queries)
{
    BAE::DB_LIST mapList;
    bool hasKey=false;

    for(auto searchQuery : queries)
    {
        if(searchQuery.contains(BAE::SearchTMap[BAE::SearchT::LIKE]+":") || searchQuery.startsWith("#"))
        {
            if(searchQuery.startsWith("#"))
                searchQuery=searchQuery.replace("#","").trimmed();
            else
                searchQuery=searchQuery.replace(BAE::SearchTMap[BAE::SearchT::LIKE]+":","").trimmed();


            searchQuery=searchQuery.trimmed();
            if(!searchQuery.isEmpty())
            {
                mapList += BabeWindow::connection->getSearchedTracks(BAE::KEY::WIKI, searchQuery);
                mapList += BabeWindow::connection->getSearchedTracks(BAE::KEY::TAG, searchQuery);
                mapList += BabeWindow::connection->getSearchedTracks(BAE::KEY::LYRICS, searchQuery);
            }

        }else if(searchQuery.contains((BAE::SearchTMap[BAE::SearchT::SIMILAR]+":")))
        {
            searchQuery=searchQuery.replace(BAE::SearchTMap[BAE::SearchT::SIMILAR]+":","").trimmed();
            searchQuery=searchQuery.trimmed();
            if(!searchQuery.isEmpty())
                mapList += BabeWindow::connection->getSearchedTracks(BAE::KEY::TAG,searchQuery);

        }else
        {
            BAE::KEY key;

            QMapIterator<BAE::KEY, QString> k(BAE::KEYMAP);
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

            searchQuery = searchQuery.trimmed();
            qDebug()<<"Searching for: "<<searchQuery;

            if(!searchQuery.isEmpty())
            {
                if(hasKey)
                    mapList += BabeWindow::connection->getSearchedTracks(key, searchQuery);
                else
                {
                    auto queryTxt = QString("SELECT * FROM tracks WHERE title LIKE \"%"+searchQuery+"%\" OR artist LIKE \"%"+searchQuery+"%\" OR album LIKE \"%"+searchQuery+"%\"OR genre LIKE \"%"+searchQuery+"%\"OR url LIKE \"%"+searchQuery+"%\" LIMIT 1000");
                    mapList += BabeWindow::connection->getDBData(queryTxt);
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
    mainList->scrollTo(mainList->model()->index(x,static_cast<int>(BAE::KEY::TITLE)),QAbstractItemView::PositionAtCenter);

}

void BabeWindow::clearMainList()
{

    BAE::DB_LIST mapList;
    if (!current_song.isEmpty()) mapList<<current_song;
    for(auto row : mainList->getSelectedRows(false))
        mapList<<mainList->getRowData(row);

    this->mainList->flushTable();
    this->addToPlaylist(mapList, true, APPEND::APPENDBOTTOM);
    mainList->removeRepeated();

    this->current_song_pos=0;
    this->prev_song_pos=current_song_pos;

    //    this->player->stop();
}

void BabeWindow::calibrateMainList()
{
    clearMainList();
    populateMainList();
    mainList->scrollToTop();

    if(mainList->rowCount()>0)
    {
        this->mainList->setCurrentCell(current_song_pos,static_cast<int>(BAE::KEY::TITLE));
        this->mainList->getItem(current_song_pos,BAE::KEY::TITLE)->setIcon(QIcon::fromTheme("media-playback-start"));
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
