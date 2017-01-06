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
#include<QDir>
#include<QDirIterator>
#include <QStringList>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    playback = new QToolBar();
    //playback->setMovable(false);
    this->setWindowTitle(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    //this->adjustSize();
   // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    //this->setMinimumSize (200, 250);
    this->mini_mode=0;

    setUpViews();

    connect(updater, SIGNAL(timeout()), this, SLOT(update()));
    player->setVolume(100);
    ui->listWidget->setCurrentRow(0);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setColumnHidden(LOCATION, true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    if(ui->listWidget->count() != 0)
    {
        loadTrack();
        player->pause();
        updater->start();

    }





    /*sidebar toolbar*/

    auto *left_spacer = new QWidget();
     left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
     right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->mainToolBar->addWidget(left_spacer);

    ui->tracks_view->setToolTip("Songs");
    ui->mainToolBar->addWidget(ui->tracks_view);

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

    /* playback toolbar*/


    playback->addWidget(ui->hide_sidebar_btn);
    playback->addWidget(ui->backward_btn);
    playback->addWidget(ui->fav_btn);
    playback->addWidget(ui->play_btn);
    playback->addWidget(ui->foward_btn);
    playback->addWidget(ui->shuffle_btn);


    //playback->addWidget();
    //playback->addWidget(ui->horizontalSlider);

    playback->setIconSize(QSize(16, 16));
    //this->addToolBar(Qt::BottomToolBarArea, playback);

    /*status bar*/
    ui->hide_sidebar_btn->setToolTip("Go Mini");
    ui->shuffle_btn->setToolTip("Shuffle");

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

    /* setup widgets*/
    settings_widget = new settings();
    connect(settings_widget, SIGNAL(toolbarIconSizeChanged(int)),
                         this, SLOT(setToolbarIconSize(int)));

    views = new QStackedWidget;
    views->addWidget(ui->tableWidget);
    auto* testing = new QLabel("hahaha tetsing this if it might work");
    views->addWidget(testing);
    views->addWidget(settings_widget);
    views->addWidget(new babes());

    connect(ui->tracks_view, SIGNAL(clicked()), this, SLOT(tracksView()));
    connect(ui->albums_view, SIGNAL(clicked()), this, SLOT(albumsView()));
    connect(ui->babes_view, SIGNAL(clicked()), this, SLOT(babesView()));
    connect(ui->playlists_view, SIGNAL(clicked()), this, SLOT(playlistsView()));
    connect(ui->queue_view, SIGNAL(clicked()), this, SLOT(queueView()));
    connect(ui->info_view, SIGNAL(clicked()), this, SLOT(infoView()));
    connect(ui->settings_view, SIGNAL(clicked()), this, SLOT(settingsView()));

    main_widget= new QWidget();
    layout = new QGridLayout();
    main_widget->setLayout(layout);
    this->setCentralWidget(main_widget);
    layout->setContentsMargins(0,0,0,0);


    /*album view*/
    auto *album_widget= new QWidget();
    auto *album_view = new QGridLayout();
    auto *album_art = new QLabel();
    album_art->setPixmap(QPixmap("../player/cover.jpg").scaled(200,200,Qt::KeepAspectRatio));

    album_view->addWidget(album_art, 0,0,Qt::AlignTop);
    album_view->addWidget(playback,1,0,Qt::AlignHCenter);
    album_view->addWidget(ui->seekBar,2,0,Qt::AlignTop);
    album_view->addWidget(ui->listWidget,3,0);
    album_view->setContentsMargins(5,0,5,0);


    album_widget->setStyleSheet("QWidget { padding:0; margin:0; }");
    album_art->setStyleSheet("background-color:red; padding:0; margin:0;");
    album_art->setStyleSheet("background-color:yellow; padding:0; margin:0");
    album_widget->setLayout(album_view);
    //auto *btn = new QPushButton("babe me");


    album_widget->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding  );

    layout->addWidget(views, 0,0 );
    layout->addWidget(album_widget,0,1, Qt::AlignRight);
    //this->setStyle();



}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setToolbarIconSize(int iconSize)
{
    qDebug()<< "Toolbar icons size changed";
    ui->mainToolBar->setIconSize(QSize(iconSize,iconSize));
    playback->setIconSize(QSize(iconSize,iconSize));
    ui->mainToolBar->update();
    playback->update();
}

void MainWindow::setUpViews()
{

}


void MainWindow::tracksView()
{
    qDebug()<< "All songs view";
    views->setCurrentIndex(0);
    if(mini_mode!=0) expand();

}

void MainWindow::albumsView()
{
    views->setCurrentIndex(1);
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
}
void MainWindow::babesView()
{
    views->setCurrentIndex(0);
    if(mini_mode!=0) expand();

    QString url= QFileDialog::getExistingDirectory();

qDebug()<<url;

    QStringList urlCollection;
//QDir dir = new QDir(url);
    QDirIterator it(url, QStringList() << "*.mp4" << "*.mp3" << "*.wav" <<"*.flac" <<"*.ogg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        urlCollection<<it.next();

        //qDebug() << it.next();
    }

    collection.add(urlCollection);
    //updateList();
    populateTableView();



}
void MainWindow::settingsView()
{
    views->setCurrentIndex(2);
    if(mini_mode!=0) expand();
}

void MainWindow::expand()
{
    views->show();
    this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    this->resize(600,400);
    ui->hide_sidebar_btn->setToolTip("Go Mini");
    mini_mode=0;

}

void MainWindow::go_mini()
{
    //this->setMaximumSize (0, 0);
    views->hide();
    this->resize(minimumSizeHint());
    main_widget->resize(minimumSizeHint());
    this->setFixedSize(minimumSizeHint());
    ui->hide_sidebar_btn->setToolTip("Go Extra-Mini");
    mini_mode=1;

}

void MainWindow::setStyle()
{

    ui->mainToolBar->setStyleSheet(" QToolBar { border-right: 1px solid #575757; } QToolButton:hover { background-color: #d8dfe0; border-right: 1px solid #575757;}");
    playback->setStyleSheet("QToolBar { border:none;} QToolBar QToolButton { border:none;} QToolBar QSlider { border:none;}");
    this->setStyleSheet("QToolButton { border: none; padding: 5px; }  QMainWindow { border-top: 1px solid #575757; }");
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

        this->setFixedSize(210,250);
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

    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"));
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

void MainWindow::populateTableView()
{
    for (Track track : collection.getTracks() )
    {
     ui->tableWidget->insertRow(ui->tableWidget->rowCount());
     auto *title= new QTableWidgetItem( QString::fromStdString(track.getTitle()));
     //title->setFlags(title->flags() & ~Qt::ItemIsEditable);

     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, TITLE, title);

     auto *artist= new QTableWidgetItem( QString::fromStdString(track.getArtist()));
     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, ARTIST, artist);

     auto *album= new QTableWidgetItem( QString::fromStdString(track.getAlbum()));
     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, ALBUM, album);

     auto *location= new QTableWidgetItem( QString::fromStdString(track.getLocation()));
     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, LOCATION, location);

    }
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

void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    //QMessageBox::information(NULL,"QTableView Item Double Clicked",index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString());

   /*
   player->setMedia(QUrl::fromLocalFile(index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString()));
   player->play();
   updater->start();
   this->setWindowTitle(index.sibling(ui->tableWidget->currentIndex().row(),TITLE).data().toString() +" \xe2\x99\xa1 " +index.sibling(ui->tableWidget->currentIndex().row(),ARTIST).data().toString());
   */
    QStringList files;
    files << index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString();
    playlist.add(files);
    updateList();
    //populateTableView();
    //ui->save->setChecked(false);
    if(shuffle) shufflePlaylist();
    //if(startUpdater) updater->start();
}

void MainWindow::loadTrack()
{
     QString qstr = QString::fromStdString(playlist.tracks[getIndex()].getLocation());
     player->setMedia(QUrl::fromLocalFile(qstr));
     qstr = QString::fromStdString(playlist.tracks[getIndex()].getTitle()+" \xe2\x99\xa1 "+playlist.tracks[getIndex()].getArtist());
     this->setWindowTitle(qstr);
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









void MainWindow::on_info_view_clicked(bool checked)
{

}

void MainWindow::on_tracks_view_clicked(bool checked)
{



}


