#include "babetable.h"
#include "ui_babetable.h"
#include <QMenu>
#include <QWidgetAction>
#include <QTableWidgetItem>

BabeTable::BabeTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BabeTable)
{

   /* connection = new CollectionDB();
    connection->openCollection("../player/collection.db");*/

    ui->setupUi(this);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->hideColumn(LOCATION);
    ui->tableWidget->hideColumn(STARS);
     ui->tableWidget->hideColumn(BABE);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui->tableWidget->horizontalHeader()->setHighlightSections(true);
    auto contextMenu = new QMenu(ui->tableWidget);
    ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    auto babeIt = new QAction("Babe it \xe2\x99\xa1",contextMenu);
    ui->tableWidget->addAction(babeIt);

    auto removeIt = new QAction("Remove",contextMenu);
    ui->tableWidget->addAction(removeIt);

    connect(ui->tableWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(setUpContextMenu()));

    connect(babeIt, SIGNAL(triggered()), this, SLOT(uninstallAppletClickedSlot()));
    connect(removeIt, SIGNAL(triggered()), this, SLOT(uninstallAppletClickedSlot()));

    QButtonGroup *bg = new QButtonGroup(contextMenu);
               bg->addButton(ui->fav1,1);
               bg->addButton(ui->fav2,2);
               bg->addButton(ui->fav3,3);
               bg->addButton(ui->fav4,4);
               bg->addButton(ui->fav5,5);
//connect(ui->fav1,SIGNAL(enterEvent(QEvent)),this,hoverEvent());
               connect(bg, SIGNAL(buttonClicked(int)),this, SLOT(rateGroup(int)));
auto gr = new QWidget();
auto ty = new QHBoxLayout();
ty->addWidget(ui->fav1);
ty->addWidget(ui->fav2);
ty->addWidget(ui->fav3);
ty->addWidget(ui->fav4);
ty->addWidget(ui->fav5);

gr->setLayout(ty);


               QWidgetAction *chkBoxAction= new QWidgetAction(contextMenu);
               chkBoxAction->setDefaultWidget(gr);

    ui->tableWidget->addAction(chkBoxAction);



}

BabeTable::~BabeTable()
{
    delete ui;
}

void BabeTable::passStyle(QString style)
{
    ui->tableWidget->setStyleSheet(style);
}

void BabeTable::addRow(QString title, QString artist, QString album,QString location, QString stars,QString babe)
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    qDebug()<<title<<artist<<album<<location<<stars<<babe;
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,TITLE, new QTableWidgetItem( title));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,ARTIST,new QTableWidgetItem( artist));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,ALBUM,new QTableWidgetItem( album));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,LOCATION,new QTableWidgetItem( location));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,STARS,new QTableWidgetItem( stars));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,BABE,new QTableWidgetItem( babe));
}

void BabeTable::populateTableView(QString indication)
{
   //ui->tableWidget->clearContents();

    QSqlQuery query= connection->getQuery(indication);

    qDebug()<<"ON POPULATE:";
       while (query.next())
       {


           ui->tableWidget->insertRow(ui->tableWidget->rowCount());

           auto *title= new QTableWidgetItem( query.value(0).toString());
           //title->setFlags(title->flags() & ~Qt::ItemIsEditable);

           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, TITLE, title);

           auto *artist= new QTableWidgetItem( query.value(1).toString());
           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, ARTIST, artist);

           //qDebug()<<query.value(2).toString();
           auto *album= new QTableWidgetItem( query.value(2).toString());
           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, ALBUM, album);

           auto *location= new QTableWidgetItem( query.value(3).toString());
           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, LOCATION, location);

          QString rating;
          switch(query.value((4)).toInt())
          {
              case 0: rating=" - "; break;
              case 1: rating="\xe2\x98\x86 "; break;
              case 2: rating="\xe2\x98\x86 \xe2\x98\x86 "; break;
              case 3: rating="\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 "; break;
              case 4: rating="\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 "; break;
              case 5: rating="\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 "; break;
          }

           auto *stars= new QTableWidgetItem( rating);
           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, STARS, stars);

           QString bb;
           switch(query.value((5)).toInt())
           {
               case 0: bb=" - "; break;
               case 1: bb="\xe2\x99\xa1 "; break;

           }



           auto *babe= new QTableWidgetItem( bb);
           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, BABE, babe);

       }

  //ui->tableWidget->sortByColumn(1,Qt::AscendingOrder);
    /*for (Track track : collection.getTracks() )
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

    }*/
}


void BabeTable::setRating(int rate)
{
    switch (rate)
    {
    case 0: ui->fav1->setIcon(QIcon::fromTheme("rating-unrated"));
            ui->fav2->setIcon(QIcon::fromTheme("rating-unrated"));
            ui->fav3->setIcon(QIcon::fromTheme("rating-unrated"));
            ui->fav4->setIcon(QIcon::fromTheme("rating-unrated"));
            ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));
            break;
        case 1: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav3->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav4->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));
                break;
        case 2: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating"));
                ui->fav3->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav4->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));
                break;
        case 3: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating"));
                ui->fav3->setIcon(QIcon::fromTheme("rating"));
                ui->fav4->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));

                break;
        case 4: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating"));
                ui->fav3->setIcon(QIcon::fromTheme("rating"));
                ui->fav4->setIcon(QIcon::fromTheme("rating"));
                 ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));
                break;
        case 5: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating"));
                ui->fav3->setIcon(QIcon::fromTheme("rating"));
                ui->fav4->setIcon(QIcon::fromTheme("rating"));
                ui->fav5->setIcon(QIcon::fromTheme("rating"));
                break;

    }
}

void BabeTable::setTableOrder(int column, int order)
{
    if (order==DESCENDING)
    {
        ui->tableWidget->sortByColumn(column,Qt::DescendingOrder);
    }
    else if (order==ASCENDING)
    {
         ui->tableWidget->sortByColumn(column,Qt::AscendingOrder);
    }
}

void BabeTable::setVisibleColumn(int column)
{
    if(column==3)
    {
        ui->tableWidget->showColumn(LOCATION);
    }
    else if (column==4)
    {
       ui->tableWidget->showColumn(STARS);
    }
    else if(column==5)
    {
        ui->tableWidget->showColumn(BABE);
    }

}

void BabeTable::setUpContextMenu()

{

    int row= ui->tableWidget->currentIndex().row(), rate;

    QString url=ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,LOCATION)).toString();
    //
    QSqlQuery query= connection->getQuery("SELECT * FROM tracks WHERE location = \""+url+"\"");

       while (query.next())
       {
           rate = query.value(4).toInt();
           qDebug()<<"se llamó a menu contextual con url: "<<url<<"rated: "<<rate;
       }

       setRating(rate);



}

void BabeTable::rateGroup(int id)
{
    qDebug()<<"rated with: "<<id;
     int row= ui->tableWidget->currentIndex().row();
    QString location=ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,LOCATION)).toString();


    QSqlQuery query =connection->getQuery("SELECT * FROM tracks WHERE location = \""+location+"\"");

    int rate;

    while(query.next()) rate = query.value(4).toInt();



    if(connection->check_existance("tracks","location",location))
    {
        if(connection->insertInto("tracks","stars",location,id))
        {
            setRating(id);


            //ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,1)).

        }
        qDebug()<<"rating the song of rowffff: "<< row;

        QString stars;
        for(int i=0; i<id; i++)
        {
         stars+="\xe2\x98\x86 ";
        }
        ui->tableWidget->item(row,STARS)->setText(stars);



        if(id>3 && rate<4  )
        {
            QString title =ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,TITLE)).toString();
            QString artist =ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,ARTIST)).toString();
            QString album =ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,ALBUM)).toString();
            QString star =ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,STARS)).toString();
            QString babe =ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,BABE)).toString();

            qDebug()<<"rated and trying to add to favs";
            emit songRated({title,artist,album,location,star,babe});
        }else
        {
            qDebug()<<"rated and trying to add to favs failed";
        }
        //ui->tableWidget->update();
    }else
    {

    }

}

void BabeTable::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    //QMessageBox::information(NULL,"QTableView Item Double Clicked",index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString());

   /*
   player->setMedia(QUrl::fromLocalFile(index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString()));
   player->play();
   updater->start();
   this->setWindowTitle(index.sibling(ui->tableWidget->currentIndex().row(),TITLE).data().toString() +" \xe2\x99\xa1 " +index.sibling(ui->tableWidget->currentIndex().row(),ARTIST).data().toString());
   */
    qDebug()<<"huuuuuuuuuuuu" << index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString();
    QStringList files;
    files << index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString();
    emit tableWidget_doubleClicked(files);

   /* playlist.add(files);
    updateList();

    if(shuffle) shufflePlaylist();*/

}


void BabeTable::uninstallAppletClickedSlot()
{
    qDebug()<<"right clicked!";
    int row= ui->tableWidget->currentIndex().row();
    qDebug()<<ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,LOCATION)).toString();

}

void BabeTable::flushTable()
{
    ui->tableWidget->clearContents();
   ui->tableWidget->setRowCount(0);

}

void BabeTable::passCollectionConnection(CollectionDB *con)
{
    connection=con;
}

QStringList BabeTable::getTableContent(int column)
{
    QStringList result;
    for(int i = 0; i<ui->tableWidget->rowCount();i++)
    {
        result<< ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,column)).toString();

    }

    return result;
}
