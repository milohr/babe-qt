#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QStatusBar>
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

    playback->addWidget(ui->toolButton_7);
    playback->addWidget(ui->fav_btn);
    playback->addWidget(ui->toolButton_9);
    playback->addWidget(ui->toolButton_8);


    //playback->addWidget();
    //playback->addWidget(ui->horizontalSlider);

    playback->setIconSize(QSize(16, 16));
    //this->addToolBar(Qt::BottomToolBarArea, playback);

    /*status bar*/
    auto *status = new QToolBar();
    auto *info=new QLabel("Song Title - Artist Name");
    //info->setStyleSheet("color:white;");
    status->addWidget(ui->shuffle_btn);
    status->addWidget(info);
    auto *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    status->addWidget(spacer);
    status->addWidget(ui->hide_sidebar_btn);    
    status->setIconSize(QSize(16, 16));

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
    album_view->addWidget(ui->horizontalSlider,2,0,Qt::AlignTop);
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
    ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-repeat-symbolic-rtl"));
}
