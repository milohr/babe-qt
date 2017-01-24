#include "playlistsview.h"
#include "colortag.h"


PlaylistsView::PlaylistsView(QWidget *parent) :
    QWidget(parent)
{
   layout = new QGridLayout();
   layout->setContentsMargins(0,0,0,0);

   table = new BabeTable();
   list = new QListWidget();
   list->setFixedWidth(150);
   list->setAlternatingRowColors(true);
   list->setFrameShape(QFrame::NoFrame);


   auto item =new QListWidgetItem();

   list->addItem(item);
   auto color = new ColorTag();
   color->setStyleSheet("background-color: blue;");
   list->setItemWidget(item,color);


   list->addItem("Favorites");

   list->addItem("Most Played");
   //list->addItem("Favorites");
   table->setFrameShape(QFrame::StyledPanel);
   //table->setSizePolicy(QSizePolicy::Expanding);

   frame = new QFrame();
   frame->setFrameShadow(QFrame::Raised);
   frame->setFrameShape(QFrame::StyledPanel);

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


   btnContainer = new QWidget();
   //btnContainer->setGeometry(0,150,150,30);
   auto *left_spacer = new QWidget();
   left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   auto btnLayout = new QHBoxLayout();
   btnLayout->addWidget(left_spacer);
   btnLayout->addWidget(removeBtn);
   btnLayout->addWidget(addBtn);
   btnContainer->setLayout(btnLayout);

   auto line = new QFrame();

       line->setFrameShape(QFrame::HLine);
       line->setFrameShadow(QFrame::Sunken);
       //line->setMaximumHeight(2);
    // btnContainer->setFixedHeight(32);


   auto sidebarLayout = new QGridLayout();
   sidebarLayout->setContentsMargins(0,0,0,0);
   sidebarLayout->setSpacing(0);
   sidebarLayout->addWidget(list,0,0);
   sidebarLayout->addWidget(line,1,0,Qt::AlignBottom);
   sidebarLayout->addWidget(btnContainer,2,0,Qt::AlignBottom);
   frame->setLayout(sidebarLayout);




   layout->addWidget(frame,0,0,Qt::AlignLeft);
   layout->addWidget(table,0,1);



   //layout->addWidget(btnContainer,1,0);
  // auto container = new QGridLayout();
  // container->addWidget(frame);
   //container->setContentsMargins(0,0,0,0);
this->setLayout(layout);

}

PlaylistsView::~PlaylistsView()
{

}
