#include "album.h"
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include <QHBoxLayout>
#include <scrolltext.h>

Album::Album(QLabel *parent) : QLabel(parent)
{
    //this->setMaximumSize(100,100);
    //this->set
    this->setMinimumSize(120,120);
    //this->setFixedSize(100,150);
    this->setPixmap(QPixmap("../player/data/cover.svg").scaled(120,120,Qt::KeepAspectRatio));
    //this->setToolTip(getTitle());
    auto widget = new QWidget(this);
auto layout = new QHBoxLayout();
widget->setLayout(layout);


title = new ScrollText();
//title->hide();
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

layout->addWidget(left_spacer);


layout->addWidget(title);

layout->addWidget(right_spacer);




    widget->setGeometry(0,90,120,30);
    widget->setStyleSheet("background-color: rgba(0,0,0,150);");
    title->setStyleSheet("background:transparent; color:white;");
    right_spacer->setStyleSheet("background:transparent;");
    left_spacer->setStyleSheet("background:transparent;");


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
    emit albumCoverClicked({artist,album});

}

void Album::mouseReleaseEvent ( QMouseEvent * evt)
{

}

void Album::enterEvent(QEvent *event)
{
    //title->show();
title->setSpeed(1);
emit albumCoverEnter();
}

 void Album::leaveEvent(QEvent *event)
{
     //title->hide();
     title->setSpeed(0);
     title->reset();
emit albumCoverLeft();
 }
