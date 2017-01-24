#include "album.h"
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include <QHBoxLayout>
#include <scrolltext.h>
#include <QMenu>
#include <QPainter>


Album::Album(QString imagePath, int widgetSize, int widgetRadius, bool isDraggable, QWidget *parent) : QLabel(parent)
{
    //this->setMaximumSize(100,100);
    //this->set
    //this->setMinimumSize(120,120);
    //this->setStyleSheet("QLabel{border:transparent} QLabel:hover{border:1px solid #3daee9} QLabel:pressed{border:1px solid red}");

    this->size=widgetSize;
    this->border_radius=widgetRadius;
    this->setFixedSize(size,size);
    this->draggable=isDraggable;
    image.load(imagePath);
//image.scaled(120,120,Qt::KeepAspectRatio);
//this->setPixmap(image.scaled(size,size,Qt::KeepAspectRatio));
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
//title->setMaxSize(size);
//title->hide();
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

layout->addWidget(left_spacer);
layout->addWidget(title);
layout->addWidget(right_spacer);
widget->setMinimumWidth(size);
this->setStyleSheet("border:none");
    widget->setGeometry(0,size-30,size,30);
    //widget->setStyleSheet("background-color: rgba(0,0,0,150)");
     widget->setStyleSheet( QString("background-color: rgba(0,0,0,150); border: 1px solid #333; border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%1px; border-bottom-left-radius:%2px;").arg( QString::number(border_radius),QString::number(border_radius)));

    title->setStyleSheet("background:transparent; color:white; border:none;");
    right_spacer->setStyleSheet("background:transparent;  border:none;");
    left_spacer->setStyleSheet("background:transparent;  border:none;");


}



void Album::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
this->setFixedSize(size,size);
title->setMaxSize(size);
widget->setGeometry(0,size-30,size,30);
widget->setMinimumWidth(size);
//title->setMaximumWidth(size);
//title->setFixedWidth(size);
//widget->setMaximumWidth(size);

    QBrush brush(image.scaled(size,size,Qt::KeepAspectRatio));
   // brush.setStyle(Qt::no);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);
 if (!borderColor)painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0,0, size, size, border_radius, border_radius);

    //this->setStyleSheet("border:1px solid red;");
    this->setPixmap(image);
}

QPixmap Album::getPixmap()
{
    return image;
}

void Album::setCoverArt(QString path)
{
    //this->setPixmap(QPixmap(path).scaled(120,120,Qt::KeepAspectRatio));
    image.load(path);
   // image.scaled(size,size,Qt::KeepAspectRatio,Qt::SmoothTransformation);

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

void Album::setTitle()
{
    title->setText(album+" - "+artist);
   // scrollText->setText(album+" - "+artist);
    //this->setToolTip(album+" - "+artist);
}


void Album::setSize(int value)
{
    this->size=value;
    widget->setGeometry(0,90,size,30);


}

void Album::setTitleGeometry(int x, int y, int w, int h)
{
     widget->setGeometry(x,y,w,h);
}
 void Album::titleVisible(bool state)
 {
     if(state) widget->show();
     else widget->hide();
 }

void Album::mousePressEvent ( QMouseEvent * evt)
{
    //qDebug()<<"the cover art was clicked: "<<getTitle();
    if(evt->button()==Qt::LeftButton && !draggable)
    {

        emit albumCoverClicked({artist,album});
    }else
    {
        QLabel::mousePressEvent(evt);
    }

   // evt->ContextMenu()

}



void Album::mouseReleaseEvent ( QMouseEvent * evt)
{
    qDebug()<<"mouseReleaseEvent";
  QLabel::mouseReleaseEvent(evt);
}

void Album::enterEvent(QEvent *event)
{
    //title->show();
//title->setSpeed(1);
  //  widget->setStyleSheet("background:rgba(180, 225, 230, 150)");
  //  this->setStyleSheet("border:1px solid #f85b79");
   // qDebug()<<"entered the album cover";
emit albumCoverEnter();
}

 void Album::leaveEvent(QEvent *event)
{
     //title->hide();
     //title->setSpeed(0);
    // title->reset();
  // widget->setStyleSheet("background-color: rgba(0,0,0,150);");
     // this->setStyleSheet("border:1px solid #333");
   //  qDebug()<<"left the album cover";
emit albumCoverLeft();
 }
