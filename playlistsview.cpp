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
    connect(this,&PlaylistsView::finishedPopulatingPlaylist,[this](const Bae::TRACKMAP_LIST &tracks, const QString &playlist)
    {
        nof.notify(playlist, QString ("%1 Track%2 added to %3").arg(QString::number(tracks.size()),tracks.size()>1?"s":"",playlist));
    });
    //    list->setFixedWidth(160);
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

    btnContainer = new QWidget(list);

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
    line_v->setFixedWidth(1);

    auto line_h = new QFrame(this);
    line_h->setFrameShape(QFrame::HLine);
    line_h->setFrameShadow(QFrame::Plain);
    line_h->setFixedHeight(1);

    auto line_h2 = new QFrame(this);
    line_h2->setFrameShape(QFrame::HLine);
    line_h2->setFrameShadow(QFrame::Plain);
    line_h2->setFixedHeight(1);


    moodWidget = new QWidget();
    moodWidget->setAutoFillBackground(true);
    moodWidget->setBackgroundRole(QPalette::Light);
    this->setPlaylistsMoods();

    auto playlistsWidget = new QWidget(this);
    auto playlistsWidget_layout = new QVBoxLayout();
    playlistsWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    playlistsWidget->setMinimumWidth(ALBUM_SIZE_MEDIUM);
    playlistsWidget_layout->setContentsMargins(0,0,0,0);
    playlistsWidget_layout->setMargin(0);
    playlistsWidget_layout->setSpacing(0);

    playlistsWidget->setLayout(playlistsWidget_layout);
    playlistsWidget_layout->addWidget(btnContainer);

    playlistsWidget_layout->addWidget(line_h2);
    playlistsWidget_layout->addWidget(list);

    playlistsWidget_layout->addWidget(line_h);
    playlistsWidget_layout->addWidget(moodWidget);


    QSplitter *splitter = new QSplitter(parent);
    splitter->setChildrenCollapsible(false);

    splitter->addWidget(playlistsWidget);
    splitter->addWidget(line_v);
    splitter->addWidget(table);

    splitter->setSizes({0,0,0});
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(2, 1);

    layout->addWidget(splitter, 0, 0);


    this->removeFromPlaylist = new QAction("Remove from Playlist");

    this->table->addMenuItem(removeFromPlaylist);
    this->removeFromPlaylist->setVisible(false);

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

    auto recent = new QListWidgetItem();
    recent->setIcon(QIcon::fromTheme("draw-star"));
    recent->setText("Recent");
    list->addItem(recent);

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
    this->currentPlaylist = index.data().toString();
    this->table->flushTable();

    if (currentPlaylist == "Most Played") {
        removeBtn->setEnabled(false);
        this->removeFromPlaylist->setVisible(false);
        table->showColumn(Bae::TracksCols::PLAYED);
        query = "SELECT * FROM tracks WHERE played > \"1\" ORDER by played desc";
    } else if (currentPlaylist == "Favorites") {
        removeBtn->setEnabled(false);
        this->removeFromPlaylist->setVisible(false);
        table->showColumn(Bae::TracksCols::STARS);
        query ="SELECT * FROM tracks WHERE stars > \"0\" ORDER  by stars desc";

    }else if (currentPlaylist == "Recent")
    {
        removeBtn->setEnabled(false);
        this->removeFromPlaylist->setVisible(false);
        table->showColumn(Bae::TracksCols::STARS);
        query ="SELECT * FROM tracks ORDER by addDate desc LIMIT 15";

    }    else if (currentPlaylist == "Babes") {
        // table->showColumn(BabeTable::PLAYED);
        this->removeFromPlaylist->setVisible(false);
        removeBtn->setEnabled(true);
        query = "SELECT * FROM tracks WHERE babe = \"1\" ORDER  by played desc";
    }else if (currentPlaylist == "Online") {
        // table->showColumn(BabeTable::PLAYED);
        this->removeFromPlaylist->setVisible(false);
        removeBtn->setEnabled(false);
        query = "SELECT * FROM tracks WHERE location LIKE \"" + youtubeCachePath + "%\"";
    } else if(!currentPlaylist.isEmpty()) {
        removeBtn->setEnabled(true);
        this->removeFromPlaylist->setVisible(true);
        table->hideColumn(Bae::TracksCols::PLAYED);
        query = QString("SELECT * FROM tracks t INNER JOIN tracks_playlists tpl on tpl.tracks_url = t.url INNER JOIN playlists pl on pl.title = tpl.playlists_title WHERE pl.title = \"%1\" ORDER by addDate desc").arg(currentPlaylist);
    }

    table->populateTableView(query,false);

}

void PlaylistsView::createPlaylist()
{

    auto *item = new QListWidgetItem("new playlist");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    this->list->addItem(item);
    currentPlaylist = "";
    emit this->list->doubleClicked(list->model()->index(this->list->count() - 1, 0));

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

    }else if(!currentPlaylist.isEmpty()) {
        connection.execQuery(QString("DELETE FROM tracks_playlists WHERE playlists_title = \"%1\"").arg(currentPlaylist));
        connection.execQuery(QString("DELETE FROM playlists WHERE title = \"%1\"").arg(currentPlaylist));
        table->flushTable();
        delete this->list->takeItem(this->list->currentRow());
        return;
    }

    table->populateTableView(query,false);

}


bool PlaylistsView::insertPlaylist(const QString &playlist)
{
    if(connection.addPlaylist(playlist)) return true;
    return false;

}

void PlaylistsView::playlistName(QListWidgetItem *item) {
    qDebug() << "old playlist name: " << currentPlaylist
             << "new playlist name: " << item->text();
    //  qDebug()<<"new playlist name: "<<item->text();

    if(!this->playlists.contains(item->text()))
    {
        if (this->currentPlaylist.isEmpty())
        {
            if(!insertPlaylist(item->text()))
                delete this->list->takeItem(this->list->count()-1);
            else
                this->playlists<<item->text();
        }
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

void PlaylistsView::saveToPlaylist(const Bae::TRACKMAP_LIST &tracks)
{
    auto form = new PlaylistForm (connection.getPlaylists(),tracks,this);
    connect(form,&PlaylistForm::saved,this,&PlaylistsView::addToPlaylist);
    connect(form,&PlaylistForm::created,[this](const QString &playlist)
    {
        if(insertPlaylist(playlist))
        {
            auto *item = new QListWidgetItem(playlist);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            list->addItem(item);
        }

    });
    form->show();
}


void PlaylistsView::addToPlaylist(const QString &playlist,const Bae::TRACKMAP_LIST &tracks)
{
    populatePlaylist(tracks, playlist);
}

void PlaylistsView::populatePlaylist(const Bae::TRACKMAP_LIST &tracks, const QString &playlist)  //this needs to get fixed
{    
    for (auto track : tracks)
        if(!this->connection.trackPlaylist(track[Bae::TracksCols::URL],playlist))
            qDebug()<<"couldn't insert track:" << track;

    emit finishedPopulatingPlaylist(tracks,playlist);/*tofix*/
}



void PlaylistsView::setPlaylistsMoods()
{

    auto moodsLayout = new QHBoxLayout();
    QButtonGroup *moodGroup = new QButtonGroup(list);
    connect(moodGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [this](int mood)
    {
        currentPlaylist = this->moods.at(mood);
        table->flushTable();
        removeBtn->setEnabled(true);
        table->hideColumn(Bae::TracksCols::PLAYED);
        QString query = "SELECT * FROM tracks WHERE art = \"" + currentPlaylist + "\"";


        table->populateTableView(query,false);
    });

    //    moodsLayout->addStretch();
    for(int i=0; i<this->moods.size(); i++)
    {
        auto  *colorTag = new QToolButton();
        //colorTag->setIconSize(QSize(10,10));
        colorTag->setFixedSize(15,15);
        // colorTag->setAutoRaise(true);
        colorTag->setStyleSheet(QString("QToolButton { background-color: %1;}").arg(this->moods.at(i)));
        moodGroup->addButton(colorTag,i);
        moodsLayout->addWidget(colorTag);
    }
    //    moodsLayout->addStretch();

    moodWidget->setLayout(moodsLayout);



}


PlaylistsView::~PlaylistsView() {}
