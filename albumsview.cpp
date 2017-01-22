#include "albumsview.h"
#include <QHeaderView>
#include <QGridLayout>
#include <album.h>
#include <QFrame>
#include <QScrollArea>
#include <QSqlQuery>
#include <QDebug>
#include <playlist.h>
#include <scrolltext.h>
#include <QListWidgetItem>
#include "mainwindow.h"

AlbumsView::AlbumsView(QWidget *parent) :
    QWidget(parent)
{


    //auto frame = new QFrame();
    //frame->setFrameShadow(QFrame::Raised);
   // frame->setFrameShape(QFrame::StyledPanel);
   // grid = new QGridLayout();
         grid = new QListWidget();
         grid->setViewMode(QListWidget::IconMode);
        grid->setResizeMode(QListWidget::Adjust);
      //  grid->setUniformItemSizes(true);
        grid->setFrameShadow(QFrame::Plain);
        grid->setFrameShape(QFrame::NoFrame);
        grid->setStyleSheet("QListWidget {background:#2E2F30; border:1px solid black; border-radius: 2px; }");
        //grid->setMovement(QListView::Static);
        //grid->setMaximumWidth(128);
       // grid->setFlow(QListView::LeftToRight);
    //grid->setFlow(QListWidget::TopToBottom);
   // grid->setMovement(QListView::Snap);
    //grid->setWrapping(false);
//grid->setSpacing(20);
         //grid->setIconSize(QSize(120,120));
         //grid->setGridSize(QSize(130,130));
        //grid->setAlignment(Qt::AlignLeading);

       auto scroll= new QScrollArea();
       scroll->setWidgetResizable(true);
       scroll->setAlignment(Qt::AlignCenter);
       //grid->setMinimumWidth(137*4);

       //scroll->setLayoutDirection(Qt::AlignCenter);
       auto scrollWidget = new QWidget();
       //scrollWidget->setLayout(grid);
       scroll->setWidget(scrollWidget   );
       albumTable = new BabeTable();
       albumTable->setMaximumHeight(120);
       albumTable->horizontalHeader()->setVisible(false);
       albumTable->hideColumn(BabeTable::ARTIST);
      albumTable->hideColumn(BabeTable::ALBUM);
      albumTable->showColumn(BabeTable::STARS);
      albumTable->setFrameShape(QFrame::NoFrame);

       connect(albumTable,SIGNAL(tableWidget_doubleClicked(QStringList)),this,SLOT(albumTable_clicked(QStringList)));
       connect(albumTable,SIGNAL( babeIt_clicked(QStringList)),this,SLOT(albumTable_babeIt(QStringList)));
       connect(albumTable,SIGNAL(songRated(QStringList)),this,SLOT(albumTable_rated(QStringList)));


   //auto albumsFrame = new QGridLayout();
  // albumsFrame->addWidget();
   //grid->setSizeConstraint(QLayout::SetMaximumSize);


   //frame->setLayout(albumsFrame);
   auto layout = new QGridLayout;
   layout->setMargin(0);
   layout->addWidget(grid,0,0);

   //scroll->setMaximumSize(120*4,120*4);
 scroll->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding  );

   auto albumBox = new QGridLayout();
   albumBox_frame = new QFrame();
   /*QPalette palette = albumBox_frame->palette();
   palette.setColor( backgroundRole(), QColor( 0, 0, 0,200 ) );
   albumBox_frame->setPalette( palette );
   albumBox_frame->setAutoFillBackground( true );*/
    albumBox->setContentsMargins(0,0,0,0);
   // ui->listWidget->setSpacing(0);
   albumBox_frame->setFrameShadow(QFrame::Raised);
   albumBox_frame->setFrameShape(QFrame::StyledPanel);
   albumBox_frame->setLayout(albumBox);
   layout->addWidget(albumBox_frame,1,0,Qt::AlignBottom);
    cover= new Album();
    cover->setStyleSheet("border:none;");
    //cover->setSizeHint( QSize( 120, 120) );

    //connect(cover, SIGNAL(albumCoverEnter()),this,SLOT(albumHover()));
   albumBox->addWidget(cover,0,0,Qt::AlignLeft);
   albumBox->addWidget(albumTable,0,1);
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

}

void AlbumsView::albumHover()
{
    //cover->setCoverArt("../player/data/cover_hover.svg");
}

void  AlbumsView::flushGrid()
{
    grid->clear();
  // grid->setRowCount(0);

}

void AlbumsView::populateTableView(QSqlQuery query)
{
    qDebug()<<"ON POPULATE ALBUM VIEW:";
    QString albumBefore = "", artistBefore = "";
    QStringList albums;
    QMap<QString, QString> example;

    //albums<<"!";
//int row=0, column=0;
    while (query.next())
    {



       if(!albums.contains(query.value(1).toString()+" "+query.value(2).toString()))
       {
           auto album= new Album();

           album->setArtist(query.value(1).toString());
           album->setAlbum(query.value(2).toString());
           album->setTitle(query.value(1).toString(),query.value(2).toString());
           //album->setToolTip(query.value(2).toString());
           connect(album, SIGNAL(albumCoverClicked(QStringList)),this,SLOT(getAlbumInfo(QStringList)));
//album->setStyleSheet(":hover {background:#3daee9; }");
           auto item =new QListWidgetItem();
           item->setSizeHint( QSize( 121, 121) );

          // item->setTextAlignment(Qt::AlignCenter);
           grid->addItem(item);

          grid->setItemWidget(item,album);

          albums<<query.value(1).toString()+" "+query.value(2).toString();
          //column++;

         // if(column==4) {row++; column=0;}
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
   // QSqlQuery query = connection->getQuery("SELECT * FROM tracks WHERE artist = \""+info.at(0)+"\" and album = \""+info.at(1)+"\"");
    //QStringList tracks;
    //playlist = new Playlist();

    //while(query.next())  tracks<<query.value(3).toString();


cover->setTitle(info.at(0),info.at(1));
        //playlist->add(tracks);
        albumTable->flushTable();
      albumTable->populateTableView("SELECT * FROM tracks WHERE artist = \""+info.at(0)+"\" and album = \""+info.at(1)+"\"");


}

void AlbumsView::albumTable_clicked(QStringList list)
{

    emit songClicked(list);
}

void AlbumsView::albumTable_rated(QStringList list)
{

    emit songRated(list);
}

void AlbumsView::albumTable_babeIt(QStringList list)
{

    emit songBabeIt(list);
}
