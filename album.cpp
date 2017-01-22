#include "album.h"
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include <QHBoxLayout>
#include <scrolltext.h>
#include <QMenu>

Album::Album(QLabel *parent) : QLabel(parent)
{
    //this->setMaximumSize(100,100);
    //this->set
    //this->setMinimumSize(120,120);
    //this->setStyleSheet("QLabel{border:transparent} QLabel:hover{border:1px solid #3daee9} QLabel:pressed{border:1px solid red}");
    this->setFixedSize(120,120);
    this->setPixmap(QPixmap(":Data/data/cover.svg").scaled(120,120,Qt::KeepAspectRatio));
    //this->setToolTip(getTitle());
    widget = new QWidget(this);
auto layout = new QHBoxLayout();
widget->setLayout(layout);



auto contextMenu = new QMenu(this);
this->setContextMenuPolicy(Qt::ActionsContextMenu);

auto babeIt = new QAction("Babe it \xe2\x99\xa1",contextMenu);
this->addAction(babeIt);

auto removeIt = new QAction("Remove",contextMenu);
this->addAction(removeIt);

//connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(setUpContextMenu()));

//connect(babeIt, SIGNAL(triggered()), this, SLOT(uninstallAppletClickedSlot()));



title = new ScrollText();
title->setMaxSize(120);
//title->hide();
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

layout->addWidget(left_spacer);


layout->addWidget(title);

layout->addWidget(right_spacer);



widget->setMinimumWidth(120);
    widget->setGeometry(0,90,120,30);
    widget->setStyleSheet("background-color: rgba(0,0,0,150)");
    title->setStyleSheet("background:transparent; color:white; border:none;");
    right_spacer->setStyleSheet("background:transparent;  border:none;");
    left_spacer->setStyleSheet("background:transparent;  border:none;");


}



void Album::setCoverArt(QString path)
{
    this->setPixmap(QPixmap(path).scaled(120,120,Qt::KeepAspectRatio));
}

QString Album::getTitle()
{
    return title->text();
}

QString Album::getArtist()
{
    return artist;
}

QString Album::getAlbum()
{
    return album;
}

void Album::setArtist(QString artist)
{
    this->artist=artist;
}

void Album::setAlbum(QString album)
{
     this->album=album;
}

void Album::setTitle(QString artist, QString album)
{
    title->setText(album+" - "+artist);
   // scrollText->setText(album+" - "+artist);
    //this->setToolTip(album+" - "+artist);
}

void Album::mousePressEvent ( QMouseEvent * evt)
{
    qDebug()<<"the cover art was clicked: "<<getTitle();
    if(evt->button()==Qt::LeftButton)
    {

        emit albumCoverClicked({artist,album});
    }

   // evt->ContextMenu()

}

void  Album::QContextMenuEvent()
{
    qDebug()<<"context menu call";
}

void Album::mouseReleaseEvent ( QMouseEvent * evt)
{

}

void Album::enterEvent(QEvent *event)
{
    //title->show();
//title->setSpeed(1);
  //  widget->setStyleSheet("background:rgba(180, 225, 230, 150)");
  //  this->setStyleSheet("border:1px solid #f85b79");
emit albumCoverEnter();
}

 void Album::leaveEvent(QEvent *event)
{
     //title->hide();
     //title->setSpeed(0);
    // title->reset();
  // widget->setStyleSheet("background-color: rgba(0,0,0,150);");
     // this->setStyleSheet("border:1px solid #333");
emit albumCoverLeft();
 }
