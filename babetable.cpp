#include "babetable.h"
#include <QMenu>
#include <QWidgetAction>
#include <QTableWidgetItem>
#include <QEvent>
#include <QTableWidget>
#include <QHeaderView>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QFileInfo>
#include <QMessageBox>

BabeTable::BabeTable(QTableWidget *parent) :
    QTableWidget(parent)
{

   /* connection = new CollectionDB();
    connection->openCollection("../player/collection.db");*/

connect(this,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(on_tableWidget_doubleClicked(QModelIndex)));
this->setColumnCount(5);
this->setHorizontalHeaderLabels({"Tile","Artist","Album","Location","Stars","Babe","Art"});
this->horizontalHeader()->setDefaultSectionSize(150);
this->verticalHeader()->setVisible(false);
this->setEditTriggers(QAbstractItemView::NoEditTriggers);
   this->setSelectionBehavior(QAbstractItemView::SelectRows);
this->setSelectionMode(QAbstractItemView::SingleSelection);
this->setAlternatingRowColors(true);
this->setSortingEnabled(true);
this->horizontalHeader()->setHighlightSections(false);
this->horizontalHeader()->setStretchLastSection(true);
//this->setGridStyle(Qt::PenStyle);
this->setShowGrid(false);
    this->hideColumn(LOCATION);
   this->hideColumn(STARS);
    this->hideColumn(BABE);

    fav1= new QToolButton ();
    fav2= new QToolButton ();
    fav3= new QToolButton ();
    fav4= new QToolButton ();
    fav5= new QToolButton ();
    fav1->setAutoRaise(true);
    fav1->setMaximumSize(16,16);
    fav2->setAutoRaise(true);
    fav2->setMaximumSize(16,16);
    fav3->setAutoRaise(true);
    fav3->setMaximumSize(16,16);
    fav4->setAutoRaise(true);
    fav4->setMaximumSize(16,16);
    fav5->setAutoRaise(true);
    fav5->setMaximumSize(16,16);
    fav1->setIcon(QIcon::fromTheme("rating-unrated"));
    fav2->setIcon(QIcon::fromTheme("rating-unrated"));
    fav3->setIcon(QIcon::fromTheme("rating-unrated"));
    fav4->setIcon(QIcon::fromTheme("rating-unrated"));
    fav5->setIcon(QIcon::fromTheme("rating-unrated"));

    //this->horizontalHeaderItem(0);
    //this->horizontalHeaderItem(0)->setResizeMode(1, QHeaderView::Interactive);
    //this->horizontalHeader()->setHighlightSections(true);
    contextMenu = new QMenu(this);
    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    auto babeIt = new QAction("Babe it \xe2\x99\xa1",contextMenu);
    this->addAction(babeIt);



    auto queueIt = new QAction("Queue",contextMenu);
    this->addAction(queueIt);

    auto editIt = new QAction("Edit",contextMenu);
    this->addAction(editIt);


    auto infoIt = new QAction("Info + ",contextMenu);
    this->addAction(infoIt);

    auto removeIt = new QAction("Remove",contextMenu);
    this->addAction(removeIt);

    auto playlistIt = new QAction("Add to Playlist...",contextMenu);
    this->addAction(playlistIt);

    connect(this,SIGNAL(rightClicked(QPoint)),this,SLOT(setUpContextMenu(QPoint)));

    connect(babeIt, SIGNAL(triggered()), this, SLOT(babeIt_action()));
    connect(removeIt, SIGNAL(triggered()), this, SLOT(babeIt_action()));

    QButtonGroup *bg = new QButtonGroup(contextMenu);
               bg->addButton(fav1,1);
               bg->addButton(fav2,2);
               bg->addButton(fav3,3);
               bg->addButton(fav4,4);
               bg->addButton(fav5,5);
//connect(fav1,SIGNAL(enterEvent(QEvent)),this,hoverEvent());
               connect(bg, SIGNAL(buttonClicked(int)),this, SLOT(rateGroup(int)));
auto gr = new QWidget();
auto ty = new QHBoxLayout();
ty->addWidget(fav1);
ty->addWidget(fav2);
ty->addWidget(fav3);
ty->addWidget(fav4);
ty->addWidget(fav5);

gr->setLayout(ty);


               QWidgetAction *chkBoxAction= new QWidgetAction(contextMenu);
               chkBoxAction->setDefaultWidget(gr);

    this->addAction(chkBoxAction);



}

BabeTable::~BabeTable()
{
    delete this;
}

void BabeTable::enterEvent(QEvent *event)
{
    //qDebug()<<"entered the playlist";
    emit enteredTable();
}

void BabeTable::leaveEvent(QEvent *event)
{
   // qDebug()<<"left the playlist";
    emit leftTable();
}


void BabeTable::passStyle(QString style)
{
    this->setStyleSheet(style);
}



void BabeTable::addRow(QString title, QString artist, QString album,QString location, QString stars,QString babe)
{
    this->insertRow(this->rowCount());
    qDebug()<<title<<artist<<album<<location<<stars<<babe;
    this->setItem(this->rowCount()-1,TITLE, new QTableWidgetItem( title));
    this->setItem(this->rowCount()-1,ARTIST,new QTableWidgetItem( artist));
    this->setItem(this->rowCount()-1,ALBUM,new QTableWidgetItem( album));
    this->setItem(this->rowCount()-1,LOCATION,new QTableWidgetItem( location));
    this->setItem(this->rowCount()-1,STARS,new QTableWidgetItem( stars));
    this->setItem(this->rowCount()-1,BABE,new QTableWidgetItem( babe));
}

void BabeTable::populateTableView(QString indication)
{
   //this->clearContents();
this->setSortingEnabled(false);
    QSqlQuery query= connection->getQuery(indication);
bool missingDialog=false;
QStringList missingFiles;
    qDebug()<<"ON POPULATE:";
       while (query.next())
       {

            if( !QFileInfo(query.value(3).toString()).exists())
            {
               qDebug ()<<"That file doesn't exists anymore: "<<query.value(3).toString();
               missingFiles<<query.value(3).toString();
               missingDialog=true;
            }else
            {
           this->insertRow(this->rowCount());

           auto *title= new QTableWidgetItem( query.value(0).toString());
           //title->setFlags(title->flags() & ~Qt::ItemIsEditable);

           this->setItem(this->rowCount()-1, TITLE, title);

           auto *artist= new QTableWidgetItem( query.value(1).toString());
           this->setItem(this->rowCount()-1, ARTIST, artist);

           //qDebug()<<query.value(2).toString();
           auto *album= new QTableWidgetItem( query.value(2).toString());
           this->setItem(this->rowCount()-1, ALBUM, album);

           auto *location= new QTableWidgetItem( query.value(3).toString());
           this->setItem(this->rowCount()-1, LOCATION, location);

          QString rating;
          switch(query.value((4)).toInt())
          {
              case 0: rating="\xe2\x99\xa1 "; break;
              case 1: rating="\xe2\x98\x86 "; break;
              case 2: rating="\xe2\x98\x86 \xe2\x98\x86 "; break;
              case 3: rating="\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 "; break;
              case 4: rating="\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 "; break;
              case 5: rating="\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 "; break;
          }

           auto *stars= new QTableWidgetItem( rating);
           this->setItem(this->rowCount()-1, STARS, stars);

           QString bb;
           switch(query.value((5)).toInt())
           {
               case 0: bb=" - "; break;
               case 1: bb="\xe2\x99\xa1 "; break;

           }



           auto *babe= new QTableWidgetItem( bb);
           this->setItem(this->rowCount()-1, BABE, babe);
            }

       }
    //this->sortByColumn(1);
      if (missingDialog)
      {
          //QMessageBox::about(this,"Removing missing files",missingFiles.join("\n"));
          for (auto file_r : missingFiles)
          {
              if(connection->removeQuery("DELETE FROM tracks WHERE location =  \""+file_r+"\"")) qDebug()<<"deleted from db: "<<file_r;
              else qDebug()<< "couldn't delete file" << file_r;

          }
      }


       this->setSortingEnabled(true);
       emit finishedPopulating();
//
  //this->sortByColumn(1,Qt::AscendingOrder);
    /*for (Track track : collection.getTracks() )
    {
     this->insertRow(this->rowCount());
     auto *title= new QTableWidgetItem( QString::fromStdString(track.getTitle()));
     //title->setFlags(title->flags() & ~Qt::ItemIsEditable);

     this->setItem(this->rowCount()-1, TITLE, title);

     auto *artist= new QTableWidgetItem( QString::fromStdString(track.getArtist()));
     this->setItem(this->rowCount()-1, ARTIST, artist);

     auto *album= new QTableWidgetItem( QString::fromStdString(track.getAlbum()));
     this->setItem(this->rowCount()-1, ALBUM, album);

     auto *location= new QTableWidgetItem( QString::fromStdString(track.getLocation()));
     this->setItem(this->rowCount()-1, LOCATION, location);

    }*/
}


void BabeTable::setRating(int rate)
{
    switch (rate)
    {
    case 0: fav1->setIcon(QIcon::fromTheme("rating-unrated"));
            fav2->setIcon(QIcon::fromTheme("rating-unrated"));
            fav3->setIcon(QIcon::fromTheme("rating-unrated"));
            fav4->setIcon(QIcon::fromTheme("rating-unrated"));
            fav5->setIcon(QIcon::fromTheme("rating-unrated"));
            break;
        case 1: fav1->setIcon(QIcon::fromTheme("rating"));
                fav2->setIcon(QIcon::fromTheme("rating-unrated"));
                fav3->setIcon(QIcon::fromTheme("rating-unrated"));
                fav4->setIcon(QIcon::fromTheme("rating-unrated"));
                fav5->setIcon(QIcon::fromTheme("rating-unrated"));
                break;
        case 2: fav1->setIcon(QIcon::fromTheme("rating"));
                fav2->setIcon(QIcon::fromTheme("rating"));
                fav3->setIcon(QIcon::fromTheme("rating-unrated"));
                fav4->setIcon(QIcon::fromTheme("rating-unrated"));
                fav5->setIcon(QIcon::fromTheme("rating-unrated"));
                break;
        case 3: fav1->setIcon(QIcon::fromTheme("rating"));
                fav2->setIcon(QIcon::fromTheme("rating"));
                fav3->setIcon(QIcon::fromTheme("rating"));
                fav4->setIcon(QIcon::fromTheme("rating-unrated"));
                fav5->setIcon(QIcon::fromTheme("rating-unrated"));

                break;
        case 4: fav1->setIcon(QIcon::fromTheme("rating"));
                fav2->setIcon(QIcon::fromTheme("rating"));
                fav3->setIcon(QIcon::fromTheme("rating"));
                fav4->setIcon(QIcon::fromTheme("rating"));
                 fav5->setIcon(QIcon::fromTheme("rating-unrated"));
                break;
        case 5: fav1->setIcon(QIcon::fromTheme("rating"));
                fav2->setIcon(QIcon::fromTheme("rating"));
                fav3->setIcon(QIcon::fromTheme("rating"));
                fav4->setIcon(QIcon::fromTheme("rating"));
                fav5->setIcon(QIcon::fromTheme("rating"));
                break;

    }
}

void BabeTable::setTableOrder(int column, int order)
{
    if (order==DESCENDING)
    {
        this->sortByColumn(column,Qt::DescendingOrder);
    }
    else if (order==ASCENDING)
    {
         this->sortByColumn(column,Qt::AscendingOrder);
    }
}

void BabeTable::setVisibleColumn(int column)
{
    if(column==3)
    {
        this->showColumn(LOCATION);
    }
    else if (column==4)
    {
       this->showColumn(STARS);
    }
    else if(column==5)
    {
        this->showColumn(BABE);
    }

}

void BabeTable::setUpContextMenu(QPoint pos)

{
qDebug()<<"setUpContextMenu";


 int rate;
bool babe= false;
row=this->indexAt(pos).row();

//row= this->currentIndex().row(), rate;

    QString url=this->model()->data(this->model()->index(row,LOCATION)).toString();
    //
    QSqlQuery query= connection->getQuery("SELECT * FROM tracks WHERE location = \""+url+"\"");

       while (query.next())
       {
           rate = query.value(STARS).toInt();
           babe = query.value(BABE).toInt()==1? true:false ;

           qDebug()<<"se llamó a menu contextual con url: "<<url<<"rated: "<<rate;
       }

       setRating(rate);
       if(babe)  this->actions().at(0)->setText("Un-Babe it");
       else this->actions().at(0)->setText("Babe it");



}


void BabeTable::mousePressEvent(QMouseEvent* evt)
{
    //QTableView::mouseReleaseEvent( event );

    if(evt->button()==Qt::RightButton) {
qDebug()<<"table right clicked";
        emit rightClicked(evt->pos());
    }else
    {
        QTableWidget::mousePressEvent(evt);
    }
}




void BabeTable::rateGroup(int id)
{
    qDebug()<<"rated with: "<<id;
    // int row= this->currentIndex().row();
    QString location=this->model()->data(this->model()->index(row,LOCATION)).toString();


    QSqlQuery query =connection->getQuery("SELECT * FROM tracks WHERE location = \""+location+"\"");

    int rate;

    while(query.next()) rate = query.value(4).toInt();



    if(connection->check_existance("tracks","location",location))
    {
        if(connection->insertInto("tracks","stars",location,id))
        {
            setRating(id);


            //this->model()->data(this->model()->index(row,1)).

        }
        qDebug()<<"rating the song of rowffff: "<< row;

        QString stars;
        for(int i=0; i<id; i++)
        {
         stars+="\xe2\x98\x86 ";
        }
        this->item(row,STARS)->setText(stars);



        if(id>3 && rate<4  )
        {
            QString title =this->model()->data(this->model()->index(row,TITLE)).toString();
            QString artist =this->model()->data(this->model()->index(row,ARTIST)).toString();
            QString album =this->model()->data(this->model()->index(row,ALBUM)).toString();
            QString star =this->model()->data(this->model()->index(row,STARS)).toString();
            QString babe =this->model()->data(this->model()->index(row,BABE)).toString();

            qDebug()<<"rated and trying to add to favs";
            emit songRated({title,artist,album,location,star,babe});
        }else
        {
            qDebug()<<"rated and trying to add to favs failed";
        }
        //this->update();
    }else
    {

    }

}

void BabeTable::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    //QMessageBox::information(NULL,"QTableView Item Double Clicked",index.sibling(this->currentIndex().row(),LOCATION).data().toString());

   /*
   player->setMedia(QUrl::fromLocalFile(index.sibling(this->currentIndex().row(),LOCATION).data().toString()));
   player->play();
   updater->start();
   this->setWindowTitle(index.sibling(this->currentIndex().row(),TITLE).data().toString() +" \xe2\x99\xa1 " +index.sibling(this->currentIndex().row(),ARTIST).data().toString());
   */
    qDebug()<<"huuuuuuuuuuuu" << index.sibling(this->currentIndex().row(),LOCATION).data().toString();
    QStringList files;
    files << index.sibling(this->currentIndex().row(),LOCATION).data().toString();
    emit tableWidget_doubleClicked(files);

   /* playlist.add(files);
    updateList();

    if(shuffle) shufflePlaylist();*/

}


void BabeTable::babeIt_action()
{
    qDebug()<<"right clicked!";
    //int row= this->currentIndex().row();
    qDebug()<<this->model()->data(this->model()->index(row,LOCATION)).toString();
    emit babeIt_clicked({this->model()->data(this->model()->index(row,LOCATION)).toString()});

}

void BabeTable::flushTable()
{
    this->clearContents();
   this->setRowCount(0);

}

void BabeTable::passCollectionConnection(CollectionDB *con)
{
    connection=con;
}

QStringList BabeTable::getTableContent(int column)
{
    QStringList result;
    for(int i = 0; i<this->rowCount();i++)
    {
        result<< this->model()->data(this->model()->index(i,column)).toString();

    }

    return result;
}
