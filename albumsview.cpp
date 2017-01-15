#include "albumsview.h"
#include "ui_albumsview.h"
#include <QGridLayout>
#include <album.h>
#include <QFrame>
#include <QScrollArea>
#include <QSqlQuery>
#include <QDebug>
#include <playlist.h>
#include <scrolltext.h>

AlbumsView::AlbumsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlbumsView)
{
    ui->setupUi(this);

    //auto frame = new QFrame();
    //frame->setFrameShadow(QFrame::Raised);
   // frame->setFrameShape(QFrame::StyledPanel);
    grid = new QGridLayout();
    //grid->setAlignment(Qt::AlignLeading);

   auto scroll= new QScrollArea();
   scroll->setWidgetResizable(true);
   scroll->setAlignment(Qt::AlignCenter);
   scroll->setMinimumWidth(140*4);
   //scroll->setBackgroundRole(QPalette :: Dark);
   //scroll->setLayoutDirection(Qt::AlignCenter);
   auto scrollWidget = new QWidget();
   scrollWidget->setLayout(grid);
   scroll->setWidget(scrollWidget   );
   //auto albumsFrame = new QGridLayout();
  // albumsFrame->addWidget();
   grid->setSizeConstraint(QLayout::SetMaximumSize);

   //frame->setLayout(albumsFrame);
   auto layout = new QGridLayout;
   layout->setMargin(0);
   layout->addWidget(scroll,0,0);

   //scroll->setMaximumSize(120*4,120*4);
 scroll->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding  );

   auto albumBox = new QGridLayout();
   albumBox_frame = new QFrame();
   albumBox_frame->setFrameShadow(QFrame::Raised);
   albumBox_frame->setFrameShape(QFrame::StyledPanel);
   albumBox_frame->setLayout(albumBox);
   layout->addWidget(albumBox_frame,1,0,Qt::AlignBottom);
    cover= new Album();
    connect(cover, SIGNAL(albumCoverEnter()),this,SLOT(albumHover()));
   albumBox->addWidget(cover,0,0,Qt::AlignLeft);
   albumBox->addWidget(ui->listWidget,0,1);
albumBox_frame->hide();
   this->setLayout(layout);
   /* for(int i =0; i<5; i++)
    {
        for(int j=0;j<5; j++)
        {


        }


    }*/

}

AlbumsView::~AlbumsView()
{
    delete ui;
}

void AlbumsView::albumHover()
{
    cover->setCoverArt("../player/data/cover_hover.svg");
}

void AlbumsView::populateTableView(QSqlQuery query)
{
    qDebug()<<"ON POPULATE ALBUM VIEW:";
    QString albumBefore = "", artistBefore = "";
    QStringList albums;
    QMap<QString, QString> example;

    //albums<<"!";
int row=0, column=0;
    while (query.next())
    {



       if(!albums.contains(query.value(1).toString()+" "+query.value(2).toString()))
       {
           auto album= new Album();

           album->setArtist(query.value(1).toString());
           album->setAlbum(query.value(2).toString());
           album->setTitle(query.value(1).toString(),query.value(2).toString());
           album->setToolTip(query.value(2).toString());
           connect(album, SIGNAL(albumCoverClicked(QStringList)),this,SLOT(getAlbumInfo(QStringList)));
           grid->addWidget(album,row,column);
          albums<<query.value(1).toString()+" "+query.value(2).toString();
          column++;

          if(column==4) {row++; column=0;}
       }


    }

   /* for(auto out:albums)
    {
        qDebug()<<out;
    }*/


}

void AlbumsView::passConnection(CollectionDB *con)
{
    this->connection=con;
}


void AlbumsView::getAlbumInfo(QStringList info)
{
    albumBox_frame->show();
    QSqlQuery query = connection->getQuery("SELECT * FROM tracks WHERE artist = \""+info.at(0)+"\" and album = \""+info.at(1)+"\"");
    QStringList tracks;
    playlist = new Playlist();

    while(query.next())  tracks<<query.value(3).toString();


cover->setTitle(info.at(0),info.at(1));
        playlist->add(tracks);
        ui->listWidget->clear();
        ui->listWidget->addItems(playlist->getTracksNameListSimple());


}

void AlbumsView::on_listWidget_doubleClicked(const QModelIndex &index)
{
    int row = ui->listWidget->currentIndex().row();
    qDebug()<< QString::fromStdString(playlist->tracks[row].getLocation());
    emit songClicked({QString::fromStdString(playlist->tracks[row].getLocation())});
}
