/*
   Babe - tiny music player
   Copyright (C) 2017  Camilo Higuita
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

   */


#include "playlistsview.h"


PlaylistsView::PlaylistsView(QWidget *parent) : QWidget(parent) {
    layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    table = new BabeTable(this);
    table->setAddMusicMsg("\nSelect a Playlist...","face-hug-right");
    list = new QListWidget(this);
    list->setFixedWidth(120);
    list->setAlternatingRowColors(true);
    list->setFrameShape(QFrame::NoFrame);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto syncPlaylist = new QAction("Sync to device...");
    list->addAction(syncPlaylist);
    list->setContextMenuPolicy(Qt::ActionsContextMenu);

    // list->setStyleSheet("background: #575757; color:white;");

    connect(list, SIGNAL(doubleClicked(QModelIndex)), list,
            SLOT(edit(QModelIndex)));
    connect(list, SIGNAL(clicked(QModelIndex)), this,
            SLOT(populatePlaylist(QModelIndex)));
    connect(list, SIGNAL(itemChanged(QListWidgetItem *)), this,
            SLOT(playlistName(QListWidgetItem *)));


    // connect(table,SIGNAL(tableWidget_doubleClicked(QStringList)),this,SLOT(tableClicked(QStringList)));
    // connect(table,SIGNAL(createPlaylist_clicked()),this,SLOT(createPlaylist()));
    // auto item =new QListWidgetItem();

    // list->addItem(item);
    // auto color = new ColorTag();
    // color->setStyleSheet("background-color: blue;");
    // list->setItemWidget(list->item(1),color);
    // list->setStyleSheet("background-color:transparent;");

    // list->addItem("Favorites");
    table->setFrameShape(QFrame::NoFrame);
    // table->setSizePolicy(QSizePolicy::Expanding);

    frame = new QFrame();
    frame->setFrameShadow(QFrame::Raised);
    frame->setFrameShape(QFrame::NoFrame);

    addBtn = new QToolButton();
    removeBtn = new QToolButton();
    // addBtn->setGeometry(50,50,16,16);
    connect(addBtn, SIGNAL(clicked()), this, SLOT(createPlaylist()));
    connect(removeBtn, SIGNAL(clicked()), this, SLOT(removePlaylist()));
    addBtn->setAutoRaise(true);
    removeBtn->setAutoRaise(true);
    addBtn->setMaximumSize(16, 16);
    removeBtn->setMaximumSize(16, 16);
    addBtn->setIcon(QIcon::fromTheme("list-add"));
    removeBtn->setIcon(QIcon::fromTheme("entry-delete"));

    auto line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Plain);
    line->setMaximumWidth(1);

    btnContainer = new QWidget();
    btnContainer->setFixedWidth(120);
    // btnContainer->setGeometry(0,150,150,30);
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(5, 0, 5, 0);
    btnContainer->setLayout(btnLayout);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(left_spacer);
    btnLayout->addWidget(removeBtn);
    // btnLayout->addWidget(line);

    line_v = new QFrame();
    line_v->setFrameShape(QFrame::VLine);
    line_v->setFrameShadow(QFrame::Plain);
    line_v->setMaximumWidth(1);
    // line->setMaximumHeight(2);
    // btnContainer->setFixedHeight(32);

    // auto sidebarLayout = new QGridLayout();
    // sidebarLayout->setContentsMargins(0,0,0,0);
    // sidebarLayout->setSpacing(0);
    // sidebarLayout->addWidget(list,0,0);
    // sidebarLayout->addWidget(line,1,0,Qt::AlignBottom);
    // sidebarLayout->addWidget(btnContainer,2,0,Qt::AlignBottom);
    // frame->setLayout(sidebarLayout);

    layout->addWidget(list, 0, 0, Qt::AlignLeft);
    layout->addWidget(line_v, 0, 1, Qt::AlignLeft);
    layout->addWidget(table, 0, 2);

    // layout->addWidget(btnContainer,1,0);
    // auto container = new QGridLayout();
    // container->addWidget(frame);
    // container->setContentsMargins(0,0,0,0);
    this->setLayout(layout);
}
void PlaylistsView::showPlaylistDialog()
{
    QDialog *playlistDialog = new QDialog();
    playlistDialog->show();

}

void PlaylistsView::dummy() { qDebug() << "signal was recived"; }

void PlaylistsView::setDefaultPlaylists() {
    /* auto title = new QListWidgetItem("PLAYLISTS");
    title->setTextAlignment(Qt::AlignCenter);
    list->addItem(title);*/

    auto mostPlayed = new QListWidgetItem();
    mostPlayed->setIcon(QIcon::fromTheme("favorite-genres-amarok"));
    mostPlayed->setText("Most Played");
    list->addItem(mostPlayed);

    auto favorites = new QListWidgetItem();
    favorites->setIcon(QIcon::fromTheme("draw-star"));
    favorites->setText("Favorites");
    list->addItem(favorites);


    auto babes = new QListWidgetItem();
    babes->setIcon(QIcon::fromTheme("love-amarok"));
    babes->setText("Babes");
    list->addItem(babes);

    auto online = new QListWidgetItem();
    online->setIcon(QIcon::fromTheme("kstars_constellationart"));
    online->setText("Online");
    list->addItem(online);

}


void PlaylistsView::populatePlaylist(const QModelIndex &index)
{
    QString query;
    currentPlaylist = index.data().toString();
    emit playlistClicked(currentPlaylist);
    table->flushTable();
    if (currentPlaylist == "Most Played") {
        removeBtn->setEnabled(false);
        table->showColumn(BabeTable::PLAYED);
        query = "SELECT * FROM tracks WHERE played > \"1\" ORDER  by played desc";
    } else if (currentPlaylist == "Favorites") {
        removeBtn->setEnabled(true);
        table->showColumn(BabeTable::STARS);
        query ="SELECT * FROM tracks WHERE stars > \"0\" ORDER  by stars desc";

    } else if (currentPlaylist == "Babes") {
        // table->showColumn(BabeTable::PLAYED);
        removeBtn->setEnabled(true);
        query = "SELECT * FROM tracks WHERE babe = \"1\" ORDER  by played desc";
    }else if (currentPlaylist == "Online") {
        // table->showColumn(BabeTable::PLAYED);
        removeBtn->setEnabled(false);
        query = "SELECT * FROM tracks WHERE location LIKE \"%" + youtubeCachePath + "%\"";
    } else if(!currentPlaylist.isEmpty()&&!currentPlaylist.contains("#")) {
        removeBtn->setEnabled(true);
        table->hideColumn(BabeTable::PLAYED);
        query = "SELECT * FROM tracks WHERE playlist LIKE \"%" + currentPlaylist + "%\"";
    }else if (currentPlaylist.contains("#")) {
        removeBtn->setEnabled(true);
        table->hideColumn(BabeTable::PLAYED);
        query = "SELECT * FROM tracks WHERE art = \"" + currentPlaylist + "\"";
    }

    table->populateTableView(query,false,false);

}

void PlaylistsView::createPlaylist()
{

    auto *item = new QListWidgetItem("new playlist");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    list->addItem(item);
    currentPlaylist = "";
    emit list->doubleClicked(list->model()->index(list->count() - 1, 0));

    // item->setFlags (item->flags () & Qt::ItemIsEditable);
}

void PlaylistsView::removePlaylist()
{
    QString query;
    if (currentPlaylist == "Most Played")
    {

    } else if (currentPlaylist == "Favorites") {
        connection.execQuery("UPDATE tracks SET stars = \"0\" WHERE stars > \"0\"");
        table->flushTable();
        query = "SELECT * FROM tracks WHERE stars > \"0\" ORDER  by stars desc";

    } else if (currentPlaylist == "Babes") {
        connection.execQuery("UPDATE tracks SET babe = \"0\" WHERE babe > \"0\"");
        table->flushTable();
        query = "SELECT * FROM tracks WHERE babe = \"1\" ORDER  by played desc";
    }else if (currentPlaylist == "Online") {
        // table->showColumn(BabeTable::PLAYED);
        /* table->populateTableView("SELECT * FROM tracks WHERE location LIKE \"%" +
                                 youtubeCachePath + "%\"");*/
    } else if(!currentPlaylist.isEmpty()&&!currentPlaylist.contains("#")) {

        /* table->hideColumn(BabeTable::PLAYED);
        table->populateTableView("SELECT * FROM tracks WHERE playlist LIKE \"%" +
                                 currentPlaylist + "%\"");*/
    }else if (currentPlaylist.contains("#")) {


        connection.execQuery("UPDATE tracks SET art = \"\" WHERE art = \"" +
                             currentPlaylist + "\"");

        connection.execQuery("DELETE FROM playlists  WHERE art = \""+currentPlaylist+"\"");
        table->flushTable();

        query = "SELECT * FROM tracks WHERE art = \"" + currentPlaylist+ "\"";
    }

    table->populateTableView(query,false,false);

}


void PlaylistsView::playlistName(QListWidgetItem *item) {
    qDebug() << "old playlist name: " << currentPlaylist
             << "new playlist name: " << item->text();
    //  qDebug()<<"new playlist name: "<<item->text();

    if(!playlists.contains(item->text()))
    {
        if (currentPlaylist.isEmpty())
            emit playlistCreated(item->text());
        else if (item->text() != currentPlaylist)
            emit modifyPlaylistName(item->text());
    }else
    {
        qDebug()<<"that playlist already exists";
        list->takeItem(list->count() - 1);
    }
}

void PlaylistsView::on_removeBtn_clicked() {}

void PlaylistsView::setPlaylists(QStringList playlists) {
    // list->addItems(playlists);

    for (auto playlist : playlists) {

        auto item = new QListWidgetItem(playlist);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        list->addItem(item);
    }


    // for (auto o: playlists) qDebug( )<<o;
}

void PlaylistsView::setPlaylistsMoods(QStringList moods_n) {
    // list->addItems(playlists);

    for (auto mood : moods_n)
    {
        auto item = new QListWidgetItem(mood);
        QColor color;
        color.setNamedColor(mood);
        color.setAlpha(40);
        item->setBackgroundColor(color);
        /*QBrush brush;
        brush.setColor(color.darker(160));
        item->setForeground(brush);*/
        list->addItem(item);
    }

    // for (auto o: playlists) qDebug( )<<o;
}

void PlaylistsView::definePlaylists(QStringList playlists)
{
    this->playlists=playlists;
}


PlaylistsView::~PlaylistsView() {}
