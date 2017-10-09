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

    layout = new QGridLayout;
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
    connect(this,&PlaylistsView::addedToPlaylist,[this](const Bae::DB_LIST &tracks, const QString &playlist)
    {
        nof.notify(playlist, QString ("%1 Track%2 added to %3").arg(QString::number(tracks.size()),tracks.size()>1?"s":"",playlist));
    });
    //    list->setFixedWidth(160);
    list->setAlternatingRowColors(true);
    list->setFrameShape(QFrame::NoFrame);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto syncPlaylist = new QAction("Sync to device...",this->list);
    list->addAction(syncPlaylist);
    list->setContextMenuPolicy(Qt::ActionsContextMenu);


    table->setFrameShape(QFrame::NoFrame);
    frame = new QFrame(this);
    frame->setFrameShadow(QFrame::Raised);
    frame->setFrameShape(QFrame::NoFrame);

    addBtn = new QToolButton(this);
    removeBtn = new QToolButton(this);
    connect(addBtn, SIGNAL(clicked()), this, SLOT(createPlaylist()));
    connect(removeBtn, SIGNAL(clicked()), this, SLOT(removePlaylist()));

    addBtn->setAutoRaise(true);
    removeBtn->setAutoRaise(true);
    addBtn->setIconSize(QSize(16, 16));
    removeBtn->setIconSize(QSize(16, 16));
    addBtn->setIcon(QIcon::fromTheme("list-add"));
    removeBtn->setIcon(QIcon::fromTheme("entry-delete"));

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Plain);
    line->setMaximumWidth(1);

    btnContainer = new QWidget(list);

    auto btnLayout = new QHBoxLayout;
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


    moodWidget = new QWidget(this);
    moodWidget->setAutoFillBackground(true);
    moodWidget->setBackgroundRole(QPalette::Light);
    this->setPlaylistsMoods();

    auto playlistsWidget = new QWidget(this);
    auto playlistsWidget_layout = new QVBoxLayout;
    playlistsWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Expanding);
    playlistsWidget->setMinimumWidth(static_cast<int>(ALBUM_SIZE_MEDIUM));
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


    this->removeFromPlaylist = new QAction("Remove from Playlist",this->table);
    connect (this->removeFromPlaylist,&QAction::triggered, [this]()
    {
        for(auto row : this->table->getSelectedRows(true))
        {
            auto url = this->table->getRowData(row)[Bae::DBCols::URL];
            if(!connection.removePlaylistTrack(url, currentPlaylist))
                qDebug()<<"couldn't remove "<<url<< "from:"<<currentPlaylist;

        }


        refreshCurrentPlaylist();

    });
    this->table->addMenuItem(removeFromPlaylist);
    this->removeFromPlaylist->setVisible(false);

    this->setLayout(layout);
}

void PlaylistsView::showPlaylistDialog()
{
    QDialog *playlistDialog = new QDialog(this);
    playlistDialog->show();

}

void PlaylistsView::dummy() { qDebug() << "signal was recived"; }

void PlaylistsView::setDefaultPlaylists()
{
    auto mostPlayed = new QListWidgetItem;
    mostPlayed->setIcon(QIcon::fromTheme("amarok_playcount"));
    mostPlayed->setText("Most Played");
    list->addItem(mostPlayed);

    auto favorites = new QListWidgetItem;
    favorites->setIcon(QIcon::fromTheme("draw-star"));
    favorites->setText("Favorites");
    list->addItem(favorites);

    auto recent = new QListWidgetItem;
    recent->setIcon(QIcon::fromTheme("filename-year-amarok"));
    recent->setText("Recent");
    list->addItem(recent);

    auto babes = new QListWidgetItem;
    babes->setIcon(QIcon::fromTheme("love-amarok"));
    babes->setText("Babes");
    list->addItem(babes);

    auto online = new QListWidgetItem;
    online->setIcon(QIcon::fromTheme("internet-amarok"));
    online->setText("Online");
    list->addItem(online);

}


void PlaylistsView::populatePlaylist(const QModelIndex &index)
{
    Bae::DB_LIST mapList;
    QString queryTxt;
    this->currentPlaylist = index.data().toString();
    this->table->flushTable();

    if (currentPlaylist == "Most Played")
    {
        removeBtn->setEnabled(false);
        this->removeFromPlaylist->setVisible(false);
        table->showColumn(static_cast<int>(Bae::DBCols::PLAYED));
        mapList = connection.getMostPlayedTracks();

    } else if (currentPlaylist == "Favorites")
    {
        removeBtn->setEnabled(false);
        this->removeFromPlaylist->setVisible(false);
        table->showColumn(static_cast<int>(Bae::DBCols::STARS));
        mapList = connection.getFavTracks();

    }else if (currentPlaylist == "Recent")
    {
        removeBtn->setEnabled(false);
        this->removeFromPlaylist->setVisible(false);
        table->showColumn(static_cast<int>(Bae::DBCols::STARS));
        mapList = connection.getRecentTracks();

    }    else if (currentPlaylist == "Babes")
    {
        // table->showColumn(BabeTable::PLAYED);
        this->removeFromPlaylist->setVisible(false);
        removeBtn->setEnabled(true);
        mapList = connection.getBabedTracks();
    }else if (currentPlaylist == "Online")
    {
        // table->showColumn(BabeTable::PLAYED);
        this->removeFromPlaylist->setVisible(false);
        removeBtn->setEnabled(false);
        mapList = connection.getOnlineTracks();
    } else if(!currentPlaylist.isEmpty())
    {
        removeBtn->setEnabled(true);
        this->removeFromPlaylist->setVisible(true);
        table->hideColumn(static_cast<int>(Bae::DBCols::PLAYED));
        mapList = connection.getPlaylistTracks(currentPlaylist);
        //        queryTxt = QString("SELECT * FROM tracks t INNER JOIN tracks_playlists tpl on tpl.tracks_url = t.url INNER JOIN playlists pl on pl.title = tpl.playlists_title WHERE pl.title = \"%1\" ORDER by addDate desc").arg(currentPlaylist);
    }
    table->populateTableView(mapList);
}

void PlaylistsView::createPlaylist()
{
    auto item = new QListWidgetItem;
    item->setText("new playlist");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    this->list->addItem(item);
    currentPlaylist = "";
    emit this->list->doubleClicked(list->model()->index(this->list->count() - 1, 0));
    // item->setFlags (item->flags () & Qt::ItemIsEditable);
}

void PlaylistsView::removePlaylist()
{
    Bae::DB_LIST mapList;

    if (currentPlaylist == "Favorites")
    {
        connection.execQuery("UPDATE tracks SET stars = \"0\" WHERE stars > \"0\"");
        table->flushTable();
        mapList = connection.getFavTracks();

    } else if (currentPlaylist == "Babes")
    {
        connection.execQuery("UPDATE tracks SET babe = \"0\" WHERE babe > \"0\"");
        table->flushTable();
        mapList = connection.getBabedTracks();

    }else if(!currentPlaylist.isEmpty())
    {
        if(connection.removePlaylist(currentPlaylist))
        {
            table->flushTable();
            delete this->list->takeItem(this->list->currentRow());
        }
        return;
    }
    table->populateTableView(mapList);

}


bool PlaylistsView::insertPlaylist(const QString &playlist)
{
    if(connection.addPlaylist(playlist)) return true;
    return false;

}

void PlaylistsView::refreshCurrentPlaylist()
{
    this->table->flushTable();
    this->table->populateTableView(connection.getPlaylistTracks(currentPlaylist));
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
        auto item = new QListWidgetItem;
        item->setText(playlist);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        list->addItem(item);
    }

}

void PlaylistsView::saveToPlaylist(const Bae::DB_LIST &tracks)
{
    auto form = new PlaylistForm (connection.getPlaylists(),tracks,this);
    connect(form,&PlaylistForm::saved,this,&PlaylistsView::addToPlaylist);
    connect(form,&PlaylistForm::created,[this](const QString &playlist)
    {
        if(insertPlaylist(playlist))
        {
            auto item = new QListWidgetItem;
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            list->addItem(item);
        }

    });
    form->show();
}


void PlaylistsView::addToPlaylist(const QString &playlist,const Bae::DB_LIST &tracks)
{
    for (auto track : tracks)
        if(!this->connection.trackPlaylist(track[Bae::DBCols::URL],playlist))
            qDebug()<<"couldn't insert track:";

    emit addedToPlaylist(tracks,playlist);/*tofix*/
}




void PlaylistsView::setPlaylistsMoods()
{

    auto moodsLayout = new QHBoxLayout;
    auto moodGroup = new QButtonGroup(this->list);
    connect(moodGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [this](const int &mood)
    {
        currentPlaylist = this->moods.at(mood);
        table->flushTable();
        removeBtn->setEnabled(true);
        table->hideColumn(static_cast<int>(Bae::DBCols::PLAYED));
        QSqlQuery query;
        QString queryTxt = "SELECT * FROM tracks WHERE art = \"" + currentPlaylist + "\"";
        query.prepare(queryTxt);
        table->populateTableView(query);
    });

    //    moodsLayout->addStretch();
    for(int i=0; i<this->moods.size(); i++)
    {
        auto  *colorTag = new QToolButton(this->list);
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

