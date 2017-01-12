#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QStatusBar>
#include <QStringList>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QStringList>
#include "collectionDB.h"
#include<QSqlQuery>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QMimeData>
#include <QMenu>
#include <QWidgetAction>
#include <QButtonGroup>
#include<QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //setWindowFlags(Qt::WindowStaysOnTopHint);
    ui->setupUi(this);
    this->setWindowTitle(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    this->setAcceptDrops(true);

    /*THE VIEWS*/

    settings_widget = new settings(); //this needs to go fist

    collectionTable = new BabeTable();
    collectionTable->passCollectionConnection(&settings_widget->getCollectionDB());
    connect(collectionTable,SIGNAL(tableWidget_doubleClicked(QStringList)),this,SLOT(addToPlaylist(QStringList)));
    connect(collectionTable,SIGNAL(songRated(QStringList)),this,SLOT(addToFavorites(QStringList)));

    favoritesTable =new BabeTable();
    connect(favoritesTable,SIGNAL(tableWidget_doubleClicked(QStringList)),this,SLOT(addToPlaylist(QStringList)));

    resultsTable=new BabeTable();
    resultsTable->passStyle("QHeaderView::section { background-color:#232323; color:white; }");
    connect(resultsTable,SIGNAL(songRated(QStringList)),this,SLOT(addToFavorites(QStringList)));
    connect(resultsTable,SIGNAL(tableWidget_doubleClicked(QStringList)),this,SLOT(addToPlaylist(QStringList)));


    if(settings_widget->checkCollection())
    {
        collectionTable->populateTableView("SELECT * FROM tracks");
        favoritesTable->populateTableView("SELECT * FROM tracks WHERE stars = \"4\" OR stars =  \"5\" OR babe =  \"1\"");
        populateMainList();
    }
    favoritesTable->setVisibleColumn(BabeTable::STARS);
   favoritesTable->setTableOrder(4,BabeTable::DESCENDING);
    //babes_widget= new babes();
    collectionTable->setTableOrder(1,BabeTable::ASCENDING);


    /*THE MAIN WIDGETS*/

 playback = new QToolBar();
 utilsBar = new QToolBar();

    /*THE STREAMING / PLAYLIST*/
    connect(updater, SIGNAL(timeout()), this, SLOT(update()));
    player->setVolume(100);
    ui->listWidget->setCurrentRow(0);

    if(ui->listWidget->count() != 0)
    {
        loadTrack();
        player->pause();
        updater->start();

    }

    //playback->setMovable(false);

    //this->adjustSize();
   // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    //this->setMinimumSize (200, 250);

    //checkCollection();


    //settings_widget->setContentsMargins(0,0,0,0);
    //settings_widget->setStyleSheet("background:red;");
    //collection_db.openCollection("../player/collection.db");



    settings_widget->readSettings();
    setToolbarIconSize(settings_widget->getToolbarIconSize());
    connect(settings_widget, SIGNAL(toolbarIconSizeChanged(int)), this, SLOT(setToolbarIconSize(int)));
    connect(settings_widget, SIGNAL(collectionDBFinishedAdding(bool)), this, SLOT(collectionDBFinishedAdding(bool)));

   // setUpViews();



    QAction *babe, *remove;
    babe = new QAction("Babe it");
    remove = new QAction("Remove from list");
    ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->listWidget->addAction(babe);
    ui->listWidget->addAction(remove);




    /*SETUP MAIN TOOLBAR*/

    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->tracks_view->setToolTip("Search...");
    ui->mainToolBar->addWidget(ui->searchField);
    ui->searchField->setChecked(true);

    ui->mainToolBar->addWidget(left_spacer);

    ui->tracks_view->setToolTip("Collection");
    ui->mainToolBar->addWidget(ui->tracks_view);
    ui->tracks_view->setChecked(true);

    ui->albums_view->setToolTip("Albums");
    ui->mainToolBar->addWidget(ui->albums_view);

    ui->babes_view->setToolTip("Babes");
    ui->mainToolBar->addWidget(ui->babes_view);

    ui->playlists_view->setToolTip("Playlists");
    ui->mainToolBar->addWidget(ui->playlists_view);

    ui->queue_view->setToolTip("Queue");
    ui->mainToolBar->addWidget(ui->queue_view);

    ui->info_view->setToolTip("Info");
    ui->mainToolBar->addWidget(ui->info_view);

    ui->settings_view->setToolTip("Setings");
    ui->mainToolBar->addWidget(ui->settings_view);

    ui->mainToolBar->addWidget(right_spacer);

    ui->open_btn->setToolTip("Open...");
    ui->mainToolBar->addWidget(ui->open_btn);

    this->addToolBar(Qt::BottomToolBarArea, ui->mainToolBar);
   // this->setCentralWidget(ui->listView);



    //playback->addWidget();
    //playback->addWidget(ui->horizontalSlider);

    //playback->setIconSize(QSize(16, 16));
    //this->addToolBar(Qt::BottomToolBarArea, playback);


    /*
    status = new QToolBar();
    info=new QLabel(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    //info->setStyleSheet("color:white;");

    status->addWidget(ui->shuffle_btn);
    auto *sp = new QWidget();
    sp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    status->addWidget(sp);
    status->addWidget(info);
    auto *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    status->addWidget(spacer);
    status->addWidget(ui->hide_sidebar_btn);    
    status->setIconSize(QSize(16, 16));
    status->setMovable(false);

    this->addToolBar(Qt::BottomToolBarArea,status);
    */

    /* SEARCH BAR & UTILS BAR*/

    /*auto clearSearch = new QToolButton(ui->search);
    clearSearch->setIcon(QIcon::fromTheme("clearSearch"));
    clearSearch->setAutoRaise(false);
    clearSearch->setGeometry(50, ui->search->sizeHint().height()/2,16,16);*/
    ui->search->setClearButtonEnabled(true);
    utilsBar->setMovable(false);
    utilsBar->addWidget(ui->search);
    utilsBar->addWidget(ui->resultsPLaylist);
    utilsBar->addWidget(ui->saveResults);



    ui->search->setPlaceholderText("Search...");

    //this->addToolBar(Qt::BottomToolBarArea,utilsBar);

    /*COMPOSE THE VIEWS*/

    views = new QStackedWidget;
    views->addWidget(collectionTable);
    auto* testing = new QLabel("albums view... todo");
    views->addWidget(testing);
    views->addWidget(favoritesTable);
    views->addWidget(settings_widget);
    views->addWidget(resultsTable);


    connect(ui->tracks_view, SIGNAL(clicked()), this, SLOT(collectionView()));
    connect(ui->albums_view, SIGNAL(clicked()), this, SLOT(albumsView()));
    connect(ui->babes_view, SIGNAL(clicked()), this, SLOT(favoritesView()));
    connect(ui->playlists_view, SIGNAL(clicked()), this, SLOT(playlistsView()));
    connect(ui->queue_view, SIGNAL(clicked()), this, SLOT(queueView()));
    connect(ui->info_view, SIGNAL(clicked()), this, SLOT(infoView()));
    connect(ui->settings_view, SIGNAL(clicked()), this, SLOT(settingsView()));


    /*MAIN WINDOW*/

    layout = new QGridLayout();
    layout->setContentsMargins(6,0,6,0);
    main_widget= new QWidget();
    main_widget->setLayout(layout);
    this->setCentralWidget(main_widget);



    /*album view*/
    auto *album_widget= new QWidget();
    auto *album_view = new QGridLayout();
    auto *album_art = new QLabel();
    album_art->setPixmap(QPixmap("../player/cover.jpg").scaled(200,200,Qt::KeepAspectRatio));

    /* PLAYBACK CONTROL BOX*/






    ui->hide_sidebar_btn->setToolTip("Go Mini");
    playback->addWidget(ui->hide_sidebar_btn);

    playback->addWidget(ui->backward_btn);
    playback->addWidget(ui->fav_btn);
    playback->addWidget(ui->play_btn);
    playback->addWidget(ui->foward_btn);

    ui->shuffle_btn->setToolTip("Shuffle");
    playback->addWidget(ui->shuffle_btn);



    controls = new QWidget(album_art);
    auto controls_layout = new QGridLayout();
    controls->setLayout(controls_layout);
    controls->setGeometry(0,150,200,50);
    controls->setStyleSheet(" background-color: rgba(255, 255, 255, 200);");

//ui->seekBar->setStyleSheet("background:transparent; ");
    album_view->addWidget(album_art, 0,0,Qt::AlignTop);
    album_view->addWidget(ui->listWidget,1,0);
    album_view->setContentsMargins(0,0,0,0);

    controls_layout->addWidget(playback,0,0,Qt::AlignHCenter);
    controls_layout->addWidget(ui->seekBar,1,0,Qt::AlignTop);



    album_widget->setStyleSheet("QWidget { padding:0; margin:0; }");
    //album_art->setStyleSheet("background-color:red; padding:0; margin:0;");
    album_art->setStyleSheet("padding:0; margin:0");
    playback->setStyleSheet(" background:transparent;");

    album_widget->setLayout(album_view);
    album_widget->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding  );

    layout->addWidget(views, 0,0 );
    layout->addWidget(utilsBar, 1,0 );
    layout->addWidget(album_widget,0,1,0,1, Qt::AlignRight);
    //this->setStyle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::enterEvent(QEvent *event)
{
    //qDebug()<<"entered the window";
    controls->show();

}


 void MainWindow::leaveEvent(QEvent *event)
{
    //qDebug()<<"left the window";
    controls->hide();
    //timer = new QTimer(this);
      /*connect(timer, SIGNAL(timeout()), this, SLOT(hideControls()));

      connect(timer,SIGNAL(timeout()), this, [&timer, this]() {
          qDebug()<<"ime is up";
          timer->stop();
      });*/

        //timer->start(3000);


}

 void MainWindow::hideControls()
 {
     /*qDebug()<<"ime is up";
     controls->hide();
     timer->stop();*/
 }
  void	MainWindow::dragEnterEvent(QDragEnterEvent *event)
 {
     event->accept();
 }

  void	MainWindow::dragLeaveEvent(QDragLeaveEvent *event){
     event->accept();
 }

  void	MainWindow::dragMoveEvent(QDragMoveEvent *event)
 {
     event->accept();
 }

  void	MainWindow::dropEvent(QDropEvent *event)
 {
     QList<QUrl> urls;
     urls = event->mimeData()->urls();
    QStringList list;
     for( auto url  : urls)
     {
         //qDebug()<<url.path();

         if(QFileInfo(url.path()).isDir())
         {
             //QDir dir = new QDir(url.path());
                 QDirIterator it(url.path(), QStringList() << "*.mp4" << "*.mp3" << "*.wav" <<"*.flac" <<"*.ogg", QDir::Files, QDirIterator::Subdirectories);
                 while (it.hasNext())
                 {
                     list<<it.next();

                     //qDebug() << it.next();
                 }

         }else if(QFileInfo(url.path()).isFile())
         {
         list<<url.path();
         }


     }

     playlist.add(list);
     updateList();
     //populateTableView();
     //ui->save->setChecked(false);
     if(shuffle) shufflePlaylist();
 }


void MainWindow::addToFavorites(QStringList list)
{
    favoritesTable->addRow(list.at(0),list.at(1),list.at(2),list.at(3),list.at(4),list.at(5));
    //qDebug()<<list.at(0)<<list.at(1)<<list.at(2)<<list.at(3)<<list.at(4)<<list.at(5);
}

void MainWindow::setToolbarIconSize(int iconSize)
{
    qDebug()<< "Toolbar icons size changed";
    ui->mainToolBar->setIconSize(QSize(iconSize,iconSize));
    playback->setIconSize(QSize(iconSize,iconSize));
    utilsBar->setIconSize(QSize(iconSize,iconSize));
    ui->mainToolBar->update();
    playback->update();
   // this->update();
}

void MainWindow::setUpViews()
{

}


void MainWindow::collectionView()
{
    qDebug()<< "All songs view";
    views->setCurrentIndex(0);
    if(mini_mode!=0) expand();

}

void MainWindow::albumsView()
{
    views->setCurrentIndex(1);
    //if(hideSearch)utilsBar->show();
    if(mini_mode!=0) expand();
}
void MainWindow::playlistsView()
{
    views->setCurrentIndex(3);
    if(mini_mode!=0) expand();
}
void MainWindow::queueView()
{
    views->setCurrentIndex(1);
    if(mini_mode!=0) expand();
}
void MainWindow::infoView()
{

    views->setCurrentIndex(0);

    if(mini_mode!=0) expand();
    //if(!hideSearch)utilsBar->hide();
}
void MainWindow::favoritesView()
{
    views->setCurrentIndex(2);
    if(mini_mode!=0) expand();

   /* QString url= QFileDialog::getExistingDirectory();

qDebug()<<url;

    QStringList urlCollection;
//QDir dir = new QDir(url);
    QDirIterator it(url, QStringList() << "*.mp4" << "*.mp3" << "*.wav" <<"*.flac" <<"*.ogg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        urlCollection<<it.next();

        //qDebug() << it.next();
    }

   // collection.add(urlCollection);
    //updateList();
    populateTableView();*/
}
void MainWindow::settingsView()
{
    views->setCurrentIndex(3);
    if(mini_mode!=0) expand();
    //if(!hideSearch) utilsBar->hide();
}

void MainWindow::expand()
{
    views->show();
    utilsBar->show();
    ui->searchField->setChecked(true);
    hideSearch=false;
    this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    this->resize(600,400);
    ui->hide_sidebar_btn->setToolTip("Go Mini");
    mini_mode=0;
    ui->mainToolBar->actions().at(0)->setVisible(true);
//keepOnTop(false);
}

void MainWindow::go_mini()
{
    //this->setMaximumSize (0, 0);

    views->hide();
    utilsBar->hide();
    ui->searchField->setChecked(false);
    hideSearch=true;
    ui->mainToolBar->actions().at(0)->setVisible(false);
   // ui->searchField->setVisible(false);
    this->resize(minimumSizeHint());
    main_widget->resize(minimumSizeHint());
    this->setFixedSize(minimumSizeHint());
    ui->hide_sidebar_btn->setToolTip("Go Extra-Mini");
    mini_mode=1;
//keepOnTop(true);

}

void MainWindow::keepOnTop(bool state)
{

    if (state)
        {//Qt::WindowFlags flags = windowFlags();
setWindowFlags(Qt::WindowStaysOnTopHint);
show();
    }else
    {
        //setWindowFlags(~ Qt::WindowStaysOnTopHint);
       //show();
    }
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
    if(mini_mode==0)
    {
        go_mini();

    }else if(mini_mode==1)
    {

        ui->listWidget->hide();
        ui->mainToolBar->hide();
       // ui->mainToolBar->hide();
       // ui->tableWidget->hide();
        //this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
        main_widget->resize(minimumSizeHint());
        this->resize(minimumSizeHint());

        this->setFixedSize(205,205);
        ui->hide_sidebar_btn->setToolTip("Expand");
        mini_mode=2;

    }else if(mini_mode==2)
    {
         ui->mainToolBar->show();
        ui->listWidget->show();
        this->resize(minimumSizeHint());
        main_widget->resize(minimumSizeHint());
        this->setFixedSize(minimumSizeHint());
        ui->hide_sidebar_btn->setToolTip("Full View");
        mini_mode=3;
    }else if (mini_mode==3)
    {
        expand();
    }


}

void MainWindow::on_shuffle_btn_clicked()
{
    /*state 0: media-playlist-consecutive-symbolic
            1: media-playlist-shuffle
            2:media-playlist-repeat-symbolic
    */
    if(shuffle_state==0)
    {
        shuffle = true;
        shufflePlaylist();
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-shuffle"));
        ui->shuffle_btn->setToolTip("Repeat");
        shuffle_state=1;

    }else if (shuffle_state==1)
    {

        repeat = true;
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-repeat"));
        ui->shuffle_btn->setToolTip("Consecutive");
        shuffle_state=2;


    }else if(shuffle_state==2)
    {
        repeat = false;
        shuffle = false;
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-consecutive-symbolic"));
        ui->shuffle_btn->setToolTip("Shuffle");
        shuffle_state=0;
    }



}

void MainWindow::on_open_btn_clicked()
{
    //bool startUpdater = false;

    //if(ui->listWidget->count() == 0) startUpdater = true;




      QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),"/home/Music", tr("Audio (*.mp3 *.wav *.mp4 *.flac *.ogg)"));
    if(!files.empty())
    {



        playlist.add(files);
        updateList();
        //populateTableView();
        //ui->save->setChecked(false);
        if(shuffle) shufflePlaylist();
        //if(startUpdater) updater->start();
    }
}



void MainWindow::populateMainList()
{
    QSqlQuery query= settings_widget->getCollectionDB().getQuery("SELECT * FROM tracks WHERE babe = 1");

    QStringList files;
       while (query.next())
       {



        files << query.value(3).toString();

       }

       playlist.add(files);
       updateList();
       //populateTableView();
       //ui->save->setChecked(false);
       if(shuffle) shufflePlaylist();
}

void MainWindow::updateList()
{
    ui->listWidget->clear();
    ui->listWidget->addItems(playlist.getTracksNameList());
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    lCounter = getIndex();

    //ui->play_btn->setChecked(false);
    //ui->searchBar->clear();
    loadTrack();
    player->play();
    updater->start();
    playing= true;
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));

}



void MainWindow::loadTrack()
{
     current_song_url = QString::fromStdString(playlist.tracks[getIndex()].getLocation());
     player->setMedia(QUrl::fromLocalFile(current_song_url));
     auto qstr = QString::fromStdString(playlist.tracks[getIndex()].getTitle()+" \xe2\x99\xa1 "+playlist.tracks[getIndex()].getArtist());
     this->setWindowTitle(qstr);

     //here check if the song to play is already babe'd and if so change the icon
      if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM tracks WHERE location = \""+current_song_url+"\" AND babe = \"1\""))
      {
          ui->fav_btn->setIcon(QIcon::fromTheme("face-in-love"));
      }else
      {
          ui->fav_btn->setIcon(QIcon::fromTheme("love"));
      }




     qDebug()<<"Current song playing is: "<< current_song_url;
}

int MainWindow::getIndex()
{
    return ui->listWidget->currentIndex().row();
}



void MainWindow::on_seekBar_sliderMoved(int position)
{
    player->setPosition(player->duration() / 1000 * position);
}






void MainWindow::update()
{   if(!ui->seekBar->isSliderDown())
        ui->seekBar->setValue((double)player->position()/player->duration() * 1000);

    if(player->state() == QMediaPlayer::StoppedState)
    {
        next();
    }
}

void MainWindow::next()
{
    lCounter++;

    if(repeat)
    {
        lCounter--;
    }

    if(lCounter >= ui->listWidget->count())
        lCounter = 0;

    (!shuffle or repeat) ? ui->listWidget->setCurrentRow(lCounter) : ui->listWidget->setCurrentRow(shuffledPlaylist[lCounter]);

    //ui->play->setChecked(false);
    //ui->searchBar->clear();

    loadTrack();
    player->play();

}

void MainWindow::back()
{
     lCounter--;

     if(lCounter < 0)
        lCounter = ui->listWidget->count() - 1;


     (!shuffle) ? ui->listWidget->setCurrentRow(lCounter) : ui->listWidget->setCurrentRow(shuffledPlaylist[lCounter]);

     //ui->play->setChecked(false);
     //ui->searchBar->clear();

     loadTrack();
     player->play();
}

void MainWindow::shufflePlaylist()
{
    shuffledPlaylist.resize(0);

    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        shuffledPlaylist.push_back(i);
    }

    random_shuffle(shuffledPlaylist.begin(), shuffledPlaylist.end());
}

void MainWindow::on_play_btn_clicked()
{
    if(ui->listWidget->count() != 0)
    {
        if(player->state() == QMediaPlayer::PlayingState)
        {
            player->pause();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-start"));
        }
       else
       {
            player->play();
            updater->start();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));
       }
      }
}

void MainWindow::on_backward_btn_clicked()
{
    if(ui->listWidget->count() != 0)
    {
        if(player->position() > 3000)
        {
           player->setPosition(0);
        }
        else
        {

            back();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));
        }
     }
}

void MainWindow::on_foward_btn_clicked()
{
    if(ui->listWidget->count() != 0)
    {
        if(repeat)
        {
            repeat = !repeat;next();repeat = !repeat;
        }
        else
        {
            next();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));
        }
     }
}


void MainWindow::collectionDBFinishedAdding(bool state)
{
    if(state)
    {
        qDebug()<<"now it i time to put the tracks in the table ;)";
        //settings_widget->getCollectionDB().closeConnection();
        collectionTable->populateTableView( "SELECT * FROM tracks");
    }
}






void MainWindow::on_fav_btn_clicked()
{


    if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM tracks WHERE location = \""+current_song_url+"\" AND babe = \"1\""))
    {
        //ui->fav_btn->setIcon(QIcon::fromTheme("face-in-love"));
        qDebug()<<"The song is already babed";
        if(settings_widget->getCollectionDB().insertInto("tracks","babe",current_song_url,0))
        {
            ui->fav_btn->setIcon(QIcon::fromTheme("love"));
        }

    }else
    {

                  if(settings_widget->getCollectionDB().check_existance("tracks","location",current_song_url))
                  {
                      if(settings_widget->getCollectionDB().insertInto("tracks","babe",current_song_url,1))
                      {
                          ui->fav_btn->setIcon(QIcon::fromTheme("face-in-love"));
                      }
                      qDebug()<<"trying to babe sth";
                  }else
                  {
                     qDebug()<<"Sorry but that song is not in the database";

                        Playlist song;
                       song.add({current_song_url});
                      settings_widget->getCollectionDB().addSong(song.getTracks(),1);
                       ui->fav_btn->setIcon(QIcon::fromTheme("face-in-love"));

                       //ui->tableWidget->insertRow(ui->tableWidget->rowCount());



                      //to-do: create a list and a tracks object and send it the new song and then write that track list into the database
                  }
    }






}



void MainWindow::on_searchField_clicked()
{

    if(hideSearch)
    {
        utilsBar->hide();
        ui->searchField->setChecked(false);
        hideSearch=false;

    }else
    {

        utilsBar->show();
        ui->searchField->setChecked(true);
        hideSearch=true;

    }

    if(mini_mode!=0)
    {
        expand();
        utilsBar->show();
        ui->searchField->setChecked(true);
        hideSearch=true;
    }
}


void MainWindow::addToPlaylist(QStringList list)
{
    qDebug()<<"hayatuususu";

    playlist.add(list);
        updateList();

        if(shuffle) shufflePlaylist();
}

void MainWindow::on_search_returnPressed()
{

    if(ui->search->text().size()!=0) views->setCurrentIndex(4);


}

void MainWindow::on_search_textChanged(const QString &arg1)
{
    QString search=arg1;
    if(search.size()>2)
    {
        views->setCurrentIndex(4);
        qDebug()<<search;
        resultsTable->flushTable();
        resultsTable->populateTableView("SELECT * FROM tracks WHERE title LIKE '%"+search+"%' OR artist LIKE '%"+search+"%' OR album LIKE '%"+search+"%'");


    }

}



void MainWindow::on_resultsPLaylist_clicked()
{

    addToPlaylist(resultsTable->getTableContent(3));
}
