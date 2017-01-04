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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    playback = new QToolBar();
    playback->setMovable(false);
    this->setWindowTitle("Babe ...");
    //this->adjustSize();
   // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    //this->setMinimumSize (200, 250);
    this->mini_mode=0;
    //this->setStyle();

    connect(updater, SIGNAL(timeout()), this, SLOT(update()));
    player->setVolume(100);
    ui->listWidget->setCurrentRow(0);

    if(ui->listWidget->count() != 0)
    {
        loadTrack();
        player->pause();
        updater->start();

    }



   auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

   auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /*sidebar toolbar*/

    ui->toolButton->setToolTip("helllooo");
    ui->mainToolBar->addWidget(left_spacer);
    ui->mainToolBar->addWidget(ui->toolButton);
    ui->mainToolBar->addWidget(ui->toolButton_2);
    ui->mainToolBar->addWidget(ui->toolButton_3);
    ui->mainToolBar->addWidget(ui->toolButton_4);
    ui->mainToolBar->addWidget(ui->toolButton_5);
    ui->mainToolBar->addWidget(ui->toolButton_6);
    ui->mainToolBar->addWidget(right_spacer);
    ui->mainToolBar->addWidget(ui->open_btn);

    this->addToolBar(Qt::LeftToolBarArea, ui->mainToolBar);
   // this->setCentralWidget(ui->listView);

    /* playback toolbar*/

    playback->addWidget(ui->backward_btn);
    playback->addWidget(ui->fav_btn);
    playback->addWidget(ui->play_btn);
    playback->addWidget(ui->foward_btn);


    //playback->addWidget();
    //playback->addWidget(ui->horizontalSlider);

    playback->setIconSize(QSize(16, 16));
    //this->addToolBar(Qt::BottomToolBarArea, playback);

    /*status bar*/
    auto *status = new QToolBar();
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
    //status->setStyleSheet("QToolButton { color:#fff; } QToolBar {background-color:#575757; color:#fff; border:1px solid #575757;} QToolBar QLabel { color:#fff;}" );

    /* setup widgets*/

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
    album_view->setContentsMargins(0,0,5,0);


    album_widget->setStyleSheet("QWidget { padding:0; margin:0; }");
    album_art->setStyleSheet("background-color:red; padding:0; margin:0;");
    album_art->setStyleSheet("background-color:yellow; padding:0; margin:0");
    album_widget->setLayout(album_view);
    //auto *btn = new QPushButton("babe me");

    album_widget->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding  );

    layout->addWidget(ui->tableWidget, 0,0 );
    layout->addWidget(album_widget,0,1, Qt::AlignRight);




}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setStyle()
{

    ui->mainToolBar->setStyleSheet(" QToolBar { border-right: 1px solid #575757; } QToolButton:hover { background-color: #d8dfe0; border-right: 1px solid #575757;}");
    playback->setStyleSheet("QToolBar { border:none;} QToolBar QToolButton { border:none;} QToolBar QSlider { border:none;}");
    this->setStyleSheet("QToolButton { border: none; padding: 5px; }  QMainWindow { border-top: 1px solid #575757; }");


}

void MainWindow::on_toolButton_4_clicked()
{

}

void MainWindow::on_toolButton_3_clicked()
{
    ui->listWidget->hide();
   // ui->mainToolBar->hide();
    ui->tableWidget->hide();
    //this->setMaximumSize (200, 300);
    //this->setFixedSize(200, 250);
}

void MainWindow::on_hide_up_btn_clicked()
{

}

void MainWindow::on_hide_sidebar_btn_clicked()
{
    if(mini_mode==0)
    {
        //this->setMaximumSize (0, 0);
        ui->tableWidget->hide();
        this->resize(minimumSizeHint());
        main_widget->resize(minimumSizeHint());
        this->setFixedSize(minimumSizeHint());
        mini_mode=1;
    }else if(mini_mode==1)
    {

        ui->listWidget->hide();
        ui->mainToolBar->hide();
       // ui->mainToolBar->hide();
       // ui->tableWidget->hide();
        //this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
        main_widget->resize(minimumSizeHint());
        this->resize(minimumSizeHint());

        this->setFixedSize(210,300);
        mini_mode=2;

    }else if(mini_mode==2)
    {
         ui->mainToolBar->show();
        ui->listWidget->show();
        this->resize(minimumSizeHint());

        mini_mode=3;
    }else if (mini_mode==3)
    {
        ui->tableWidget->show();
        this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);

        mini_mode=0;
    }


}

void MainWindow::on_shuffle_btn_clicked()
{

    shuffle = !shuffle;
    if(shuffle)
    {
        shufflePlaylist();
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-shuffle-symbolic"));
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
        //ui->save->setChecked(false);
        if(shuffle) shufflePlaylist();
        //if(startUpdater) updater->start();
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

    ui->play_btn->setChecked(false);
    //ui->searchBar->clear();
    loadTrack();
    player->play();
    updater->start();
    playing= true;
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause-symbolic"));

}

void MainWindow::loadTrack()
{
     QString qstr = QString::fromStdString(playlist.tracks[getIndex()].getLocation());
     player->setMedia(QUrl::fromLocalFile(qstr));
     qstr = QString::fromStdString(playlist.tracks[getIndex()].getTitle()+" by "+playlist.tracks[getIndex()].getArtist());
     info->setText(qstr);
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
        if(player->state() == QMediaPlayer::PlayingState)
        {
            player->pause();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-start-symbolic"));
        }
       else
       {
            player->play();
            updater->start();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause-symbolic"));
       }
}

void MainWindow::on_backward_btn_clicked()
{
    if(ui->listWidget->count() != 0)
        if(player->position() > 3000)
        {
           player->setPosition(0);
        }
        else
        {

            back();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause-symbolic"));
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
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause-symbolic"));
        }
     }
}








