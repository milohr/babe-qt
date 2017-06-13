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


PlaylistsView::PlaylistsView(QWidget *parent) : QWidget(parent)
{

    layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    table = new BabeTable(this);
    table->setAddMusicMsg("\nPlaylist is empty...","face-hug-right");

    list = new QListWidget(this);
    connect(list, SIGNAL(doubleClicked(QModelIndex)), list,
            SLOT(edit(QModelIndex)));
    connect(list, SIGNAL(clicked(QModelIndex)), this,
            SLOT(populatePlaylist(QModelIndex)));
    connect(list, SIGNAL(itemChanged(QListWidgetItem *)), this,
            SLOT(playlistName(QListWidgetItem *)));

    list->setFixedWidth(160);
    list->setAlternatingRowColors(true);
    list->setFrameShape(QFrame::NoFrame);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto syncPlaylist = new QAction("Sync to device...");
    list->addAction(syncPlaylist);
    list->setContextMenuPolicy(Qt::ActionsContextMenu);


    table->setFrameShape(QFrame::NoFrame);
    frame = new QFrame();
    frame->setFrameShadow(QFrame::Raised);
    frame->setFrameShape(QFrame::NoFrame);

    addBtn = new QToolButton();
    removeBtn = new QToolButton();
    connect(addBtn, SIGNAL(clicked()), this, SLOT(createPlaylist()));
    connect(removeBtn, SIGNAL(clicked()), this, SLOT(removePlaylist()));

    addBtn->setAutoRaise(true);
    removeBtn->setAutoRaise(true);
    addBtn->setIconSize(QSize(16, 16));
    removeBtn->setIconSize(QSize(16, 16));
    addBtn->setIcon(QIcon::fromTheme("list-add"));
    removeBtn->setIcon(QIcon::fromTheme("entry-delete"));

    auto line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Plain);
    line->setMaximumWidth(1);

    btnContainer = new QWidget();
    btnContainer->setFixedWidth(160);

    auto btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(5, 0, 5, 0);
    btnContainer->setLayout(btnLayout);

    btnLayout->addWidget(new QLabel("Playlists"));
    btnLayout->addStretch();
    btnLayout->addWidget(addBtn);


    btnLayout->addWidget(removeBtn);

    line_v = new QFrame(this);
    line_v->setFrameShape(QFrame::VLine);
    line_v->setFrameShadow(QFrame::Plain);
    line_v->setMaximumWidth(1);

    auto line_h = new QFrame(this);
    line_h->setFrameShape(QFrame::HLine);
    line_h->setFrameShadow(QFrame::Plain);
    line_h->setMaximumHeight(1);

    auto playlistsWidget = new QWidget(this);
    auto playlistsWidget_layout = new QVBoxLayout();
    playlistsWidget_layout->setContentsMargins(0,0,0,0);
    playlistsWidget_layout->setMargin(0);
    playlistsWidget_layout->setSpacing(0);
    playlistsWidget->setLayout(playlistsWidget_layout);
    playlistsWidget_layout->addWidget(btnContainer);
    playlistsWidget_layout->addWidget(line_h);
    playlistsWidget_layout->addWidget(list);




    layout->addWidget(playlistsWidget, 0, 0, Qt::AlignLeft);
    layout->addWidget(line_v, 0, 1, Qt::AlignLeft);
    layout->addWidget(table, 0, 2);

    this->setLayout(layout);
}
void PlaylistsView::showPlaylistDialog()
{
    QDialog *playlistDialog = new QDialog();
    playlistDialog->show();

}

void PlaylistsView::dummy() { qDebug() << "signal was recived"; }

void PlaylistsView::setDefaultPlaylists()
{


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

    list->setCurrentRow(0);
    emit list->clicked(list->model()->index(0,0));

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

    table->populateTableView(query,false);

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

    table->populateTableView(query,false);

}


void PlaylistsView::insertPlaylist(const QString &playlist)
{


    connection.insertPlaylist(playlist);
}

void PlaylistsView::playlistName(QListWidgetItem *item) {
    qDebug() << "old playlist name: " << currentPlaylist
             << "new playlist name: " << item->text();
    //  qDebug()<<"new playlist name: "<<item->text();

    if(!playlists.contains(item->text()))
    {
        if (currentPlaylist.isEmpty())
            insertPlaylist(item->text());
        else if (item->text() != currentPlaylist)
            emit modifyPlaylistName(item->text());
    }else
    {
        qDebug()<<"that playlist already exists";
        list->takeItem(list->count() - 1);
    }
}

void PlaylistsView::on_removeBtn_clicked() {}

void PlaylistsView::setPlaylists(const QStringList &playlists)
{
    this->playlists=playlists;
    for (auto playlist : this->playlists)
    {
        auto item = new QListWidgetItem(playlist);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        list->addItem(item);
    }

}

void PlaylistsView::saveToPlaylist(const QList<QMap<int,QString>> &tracks)
{
    auto form = new PlaylistForm (connection.getPlaylists(),tracks,this);
    connect(form,&PlaylistForm::saved,this,&PlaylistsView::addToPlaylist);
    connect(form,&PlaylistForm::created,this,&PlaylistsView::insertPlaylist);
    form->show();
}


void PlaylistsView::addToPlaylist(const QString &playlist,const QList<QMap<int,QString>> &tracks)
{

    QStringList locations;
    for(auto track : tracks) locations<<track[BabeTable::LOCATION];

    populatePlaylist(locations, playlist);

    connect(this,&PlaylistsView::finishedPopulatingPlaylist,[&tracks,this](QString playlist)
    {
        nof.notify(playlist, QString ("%1 Track%2 added to playlist").arg(QString::number(tracks.size()),tracks.size()>1?"s":""));
    });
}

void PlaylistsView::populatePlaylist(const QStringList &urls, const QString &playlist)  //this needs to get fixed
{
    for (auto location : urls)
    {
        QSqlQuery query = connection.getQuery("SELECT * FROM tracks WHERE location = \"" + location + "\"");

        QString list;
        while (query.next())
            list = query.value(BabeTable::PLAYLIST).toString();
        list += " " + playlist;

        if (connection.insertInto("tracks", "playlist", location, list))
            qDebug() << list;
    }

    emit finishedPopulatingPlaylist(playlist);
}



void PlaylistsView::setPlaylistsMoods()
{

    for (auto mood : this->moods)
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

}


PlaylistsView::~PlaylistsView() {}
