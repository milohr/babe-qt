#include "playlistsview.h"
#include "colortag.h"
#include <mainwindow.h>

PlaylistsView::PlaylistsView(QWidget *parent) :
    QWidget(parent)
{
    layout = new QGridLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    table = new BabeTable();
    table->showColumn(BabeTable::PLAYED);
    list = new QListWidget();
    list->setFixedWidth(150);
    list->setAlternatingRowColors(true);
    list->setFrameShape(QFrame::NoFrame);


    connect(list, SIGNAL(doubleClicked(QModelIndex)), list, SLOT(edit(QModelIndex)));
    connect(list,SIGNAL(clicked(QModelIndex)),this,SLOT(populatePlaylist(QModelIndex)));
    connect(list,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(playlistName(QListWidgetItem*)));


    connect(table,SIGNAL(tableWidget_doubleClicked(QStringList)),this,SLOT(tableClicked(QStringList)));

   //auto item =new QListWidgetItem();

   //list->addItem(item);
   //auto color = new ColorTag();
   //color->setStyleSheet("background-color: blue;");
   //list->setItemWidget(list->item(1),color);



    auto mostPlayed = new QListWidgetItem();
    mostPlayed->setIcon(QIcon::fromTheme("favorite-genres-amarok"));
    mostPlayed->setText("Most Played");
    list->addItem(mostPlayed);


   //list->addItem("Favorites");
    table->setFrameShape(QFrame::NoFrame);
   //table->setSizePolicy(QSizePolicy::Expanding);

    frame = new QFrame();
    frame->setFrameShadow(QFrame::Raised);
    frame->setFrameShape(QFrame::NoFrame);

    addBtn = new QToolButton();
    removeBtn = new QToolButton();
   //addBtn->setGeometry(50,50,16,16);
    connect(addBtn,SIGNAL(clicked()),this,SLOT(createPlaylist()));
    addBtn->setAutoRaise(true);
    removeBtn->setAutoRaise(true);
    addBtn->setMaximumSize(16,16);
    removeBtn->setMaximumSize(16,16);
    addBtn->setIcon(QIcon::fromTheme("list-add"));
    removeBtn->setIcon(QIcon::fromTheme("entry-delete"));


   auto line = new QFrame();
   line->setFrameShape(QFrame::VLine);
   line->setFrameShadow(QFrame::Plain);
    line->setMaximumWidth(1);

   btnContainer = new QWidget();
   btnContainer->setFixedWidth(150);
   //btnContainer->setGeometry(0,150,150,30);
   auto *left_spacer = new QWidget();
   left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   auto btnLayout = new QHBoxLayout();
   btnLayout->setSpacing(0);
   btnLayout->setContentsMargins(5,0,5,0);
    btnContainer->setLayout(btnLayout);
   btnLayout->addWidget(addBtn);
    btnLayout->addWidget(left_spacer);
btnLayout->addWidget(removeBtn);
//btnLayout->addWidget(line);




    line_v = new QFrame();
   line_v->setFrameShape(QFrame::VLine);
   line_v->setFrameShadow(QFrame::Plain);
   line_v->setMaximumWidth(1);
       //line->setMaximumHeight(2);
    // btnContainer->setFixedHeight(32);


   //auto sidebarLayout = new QGridLayout();
   //sidebarLayout->setContentsMargins(0,0,0,0);
   //sidebarLayout->setSpacing(0);
   //sidebarLayout->addWidget(list,0,0);
   //sidebarLayout->addWidget(line,1,0,Qt::AlignBottom);
   //sidebarLayout->addWidget(btnContainer,2,0,Qt::AlignBottom);
   //frame->setLayout(sidebarLayout);




   layout->addWidget(list,0,0,Qt::AlignLeft);
   layout->addWidget(line_v,0,1,Qt::AlignLeft);
   layout->addWidget(table,0,2);



   //layout->addWidget(btnContainer,1,0);
  // auto container = new QGridLayout();
  // container->addWidget(frame);
   //container->setContentsMargins(0,0,0,0);
this->setLayout(layout);

}

void PlaylistsView::tableClicked(QStringList list)

{

    emit songClicked(list);
}

void PlaylistsView::populatePlaylist(QModelIndex index)
{
    QString playlist = index.data().toString();
    if(playlist=="Most Played")
    {
        table->flushTable();
        table->populateTableView("SELECT * FROM tracks WHERE played > \"1\" ORDER  by played desc");
    }else
    {
        table->flushTable();
    }

}

void PlaylistsView::createPlaylist()
{

    auto  *item = new QListWidgetItem("new playlist");
    item->setFlags (item->flags () | Qt::ItemIsEditable);
    list->addItem(item);

    emit list->doubleClicked(list->model()->index(list->count()-1,0));

   // item->setFlags (item->flags () & Qt::ItemIsEditable);

}

void PlaylistsView::playlistName(QListWidgetItem* item)
{
    qDebug()<<"new playlist name: "<<item->text();
    emit playlistCreated(item->text());
}

void PlaylistsView::on_removeBtn_clicked()
{

}

void PlaylistsView::setPlaylists(QStringList playlists)
{
    list->addItems(playlists);
    for (auto o: playlists) qDebug( )<<o;
}

PlaylistsView::~PlaylistsView()
{

}
