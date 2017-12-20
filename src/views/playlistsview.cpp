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
#include "../views/babewindow.h"

#include "../widget_models/babetable.h"
#include "../dialogs/playlistform.h"
#include "../kde/notify.h"
#include "../db/collectionDB.h"

PlaylistsView::PlaylistsView(QWidget *parent) : QWidget(parent)
{
    this->connection = new CollectionDB(this);

    this->layout = new QGridLayout;
    this->layout->setContentsMargins(0, 0, 0, 0);
    this->layout->setSpacing(0);

    this->table = new BabeTable(this);
    this->table->setFrameShape(QFrame::StyledPanel);
    this->table->setFrameShadow(QFrame::Sunken);
    this->table->setAddMusicMsg("\nPlaylist is empty...","face-hug-right");

    this->list = new QListWidget(this);
    connect(this->list, SIGNAL(doubleClicked(QModelIndex)), list, SLOT(edit(QModelIndex)));
    connect(this->list, SIGNAL(clicked(QModelIndex)), this, SLOT(populatePlaylist(QModelIndex)));
    connect(this->list, &QListWidget::itemChanged, this, &PlaylistsView::playlistName);

    connect(this,&PlaylistsView::addedToPlaylist,[](const BAE::DB_LIST &tracks, const QString &playlist)
    {
        BabeWindow::nof->notify(playlist, QString ("%1 Track%2 added to %3").arg(QString::number(tracks.size()),tracks.size()>1?"s":"",playlist));
    });
    //    list->setFixedWidth(160);
    this->list->setAlternatingRowColors(true);
    this->list->setFrameShape(QFrame::NoFrame);
    this->list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->list->setContextMenuPolicy(Qt::ActionsContextMenu);


    this->tagList = new QListWidget(this);
    connect(this->tagList, &QListWidget::clicked,[this](QModelIndex index)
    {
        auto tag = index.data().toString();
        this->table->flushTable();
        QString query;
        {
            switch(this->list->currentRow())
            {
                case PLAYLIST::LIST::TAGS:
                {
                    query = QString("select * from tracks where url in (select url from tracks_tags where tag = '%1')").arg(tag);
                    break;

                }
                case PLAYLIST::LIST::RELATIONS:
                {
                    query = QString("select * from tracks where artist in (select artist from artists_tags where tag = '%1')").arg(tag);
                    break;
                }
                case PLAYLIST::LIST::POPULAR:
                {
                    query = QString("select t.* from tracks t "
                                    "inner join tracks_tags tt on tt.url = t.url "
                                    "where tt.context = 'track_stat' and t.artist = '%1' "
                                    "group by tt.url order by sum(tag) desc limit 250").arg(tag);
                    break;

                }
                case PLAYLIST::LIST::GENRES:
                {
                    query = QString("select * from tracks where genre = '%1'").arg(tag);
                    break;
                }
            }
        }

        this->table->populateTableView(query);
    });

    this->tagList->setAlternatingRowColors(true);
    this->tagList->setFrameShape(QFrame::NoFrame);
    this->tagList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->tagList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->addBtn = new QToolButton(this);
    this->removeBtn = new QToolButton(this);
    connect(this->addBtn, &QToolButton::clicked, this, &PlaylistsView::createPlaylist);
    connect(this->removeBtn,  &QToolButton::clicked, this, &PlaylistsView::removePlaylist);

    this->addBtn->setAutoRaise(true);
    this->removeBtn->setAutoRaise(true);
    this->addBtn->setIconSize(QSize(16, 16));
    this->removeBtn->setIconSize(QSize(16, 16));
    this->addBtn->setIcon(QIcon::fromTheme("list-add"));
    this->removeBtn->setIcon(QIcon::fromTheme("entry-delete"));

    this->btnContainer = new QWidget(this->list);

    auto btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(5, 0, 5, 0);
    this->btnContainer->setLayout(btnLayout);

    btnLayout->addWidget(new QLabel("Playlists"));
    btnLayout->addStretch();
    btnLayout->addWidget(this->addBtn);
    btnLayout->addWidget(this->removeBtn);

    line_v = new QFrame(this);
    line_v->setFrameShape(QFrame::VLine);
    line_v->setFrameShadow(QFrame::Plain);
    line_v->setFixedWidth(1);
    line_v->setVisible(false);

    auto line_h = new QFrame(this);
    line_h->setFrameShape(QFrame::HLine);
    line_h->setFrameShadow(QFrame::Plain);
    line_h->setFixedHeight(1);

    auto line_h2 = new QFrame(this);
    line_h2->setFrameShape(QFrame::HLine);
    line_h2->setFrameShadow(QFrame::Plain);
    line_h2->setFixedHeight(1);

    this->moodWidget = new QWidget(this);
    this->moodWidget->setAutoFillBackground(true);
    this->moodWidget->setBackgroundRole(QPalette::Light);
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
    playlistsWidget_layout->addWidget(this->moodWidget);

    this->frame = new QFrame(this);
    this->frame->setFrameShadow(QFrame::Raised);
    this->frame->setFrameShape(QFrame::StyledPanel);

    auto frameLayout = new QHBoxLayout;
    frameLayout->setContentsMargins(0,0,0,0);
    frameLayout->setSpacing(0);
    frameLayout->setMargin(0);

    this->frame->setLayout(frameLayout);

    QSplitter *splitterList = new QSplitter(this);
    splitterList->setChildrenCollapsible(false);

    splitterList->addWidget(playlistsWidget);
    splitterList->addWidget(this->line_v);
    splitterList->addWidget(this->tagList);

    frameLayout->addWidget(splitterList);

    QSplitter *splitter = new QSplitter(this);
    splitter->setChildrenCollapsible(false);

    splitter->addWidget(this->frame);
    splitter->addWidget(this->table);

    splitter->setHandleWidth(6);
    splitter->setSizes({0,0});
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter, 0, 0);


    this->removeFromPlaylist = new QAction(tr("Remove from Playlist"), this->table);
    connect(this->removeFromPlaylist, &QAction::triggered, [this]()
    {
        for(auto row : this->table->getSelectedRows(true))
        {
            auto url = this->table->getRowData(row)[BAE::KEY::URL];

            if(!this->connection->removePlaylistTrack(url, this->currentPlaylist))
                qDebug()<<"couldn't remove "<<url<< "from:"<<this->currentPlaylist;
        }

        this->refreshCurrentPlaylist();
    });

    this->table->addMenuItem(this->removeFromPlaylist);
    this->removeFromPlaylist->setVisible(false);

    this->setLayout(this->layout);

    this->tagList->setVisible(false);
}

PlaylistsView::~PlaylistsView()
{
    qDebug()<<"DELETING PLAYLISVIEW";
}

void PlaylistsView::setDefaultPlaylists()
{
    auto mostPlayed = new QListWidgetItem;
    mostPlayed->setIcon(QIcon::fromTheme("amarok_playcount"));
    mostPlayed->setText(tr("Most Played"));
    this->list->insertItem(PLAYLIST::LIST::MOSTPLAYED, mostPlayed);

    auto favorites = new QListWidgetItem;
    favorites->setIcon(QIcon::fromTheme("draw-star"));
    favorites->setText(tr("Favorites"));
    this->list->insertItem(PLAYLIST::LIST::FAVORITES, favorites);

    auto recent = new QListWidgetItem;
    recent->setIcon(QIcon::fromTheme("filename-year-amarok"));
    recent->setText(tr("Recent"));
    this->list->insertItem(PLAYLIST::LIST::RECENT, recent);

    auto babes = new QListWidgetItem;
    babes->setIcon(QIcon::fromTheme("love-amarok"));
    babes->setText(tr("Babes"));
    this->list->insertItem(PLAYLIST::LIST::BABES, babes);

    auto online = new QListWidgetItem;
    online->setIcon(QIcon::fromTheme("internet-amarok"));
    online->setText(tr("Online"));
    this->list->insertItem(PLAYLIST::LIST::ONLINE, online);

    auto tags = new QListWidgetItem;
    tags->setIcon(QIcon::fromTheme("tag"));
    tags->setText(tr("Tags"));
    this->list->insertItem(PLAYLIST::LIST::TAGS, tags);

    auto relations = new QListWidgetItem;
    relations->setIcon(QIcon::fromTheme("similarartists-amarok"));
    relations->setText(tr("Relationships"));
    this->list->insertItem(PLAYLIST::LIST::RELATIONS, relations);

    auto popular = new QListWidgetItem;
    popular->setIcon(QIcon::fromTheme("office-chart-line"));
    popular->setText(tr("Popular"));
    this->list->insertItem(PLAYLIST::LIST::POPULAR, popular);

    auto genres = new QListWidgetItem;
    genres->setIcon(QIcon::fromTheme("filename-track-amarok"));
    genres->setText(tr("Genres"));
    this->list->insertItem(PLAYLIST::LIST::GENRES, genres);
}

void PlaylistsView::populatePlaylist(const QModelIndex &index)
{
    BAE::DB_LIST mapList;
    QString queryTxt;

    this->currentPlaylist = index.data().toString();
    this->table->flushTable();
    this->tagList->setVisible(false);
    this->line_v->setVisible(false);

    switch(index.row())
    {
        case PLAYLIST::LIST::MOSTPLAYED:
        {
            this->removeBtn->setEnabled(false);
            this->removeFromPlaylist->setVisible(false);
            this->table->showColumn(static_cast<int>(BAE::KEY::PLAYED));
            mapList = this->connection->getMostPlayedTracks();
            break;
        }

        case PLAYLIST::LIST::FAVORITES:
        {
            this->removeBtn->setEnabled(false);
            this->removeFromPlaylist->setVisible(false);
            this->table->showColumn(static_cast<int>(BAE::KEY::STARS));
            mapList = this->connection->getFavTracks();
            break;
        }

        case PLAYLIST::LIST::RECENT:
        {
            this->removeBtn->setEnabled(false);
            this->removeFromPlaylist->setVisible(false);
            this->table->showColumn(static_cast<int>(BAE::KEY::STARS));
            mapList = this->connection->getRecentTracks();
            break;

        }

        case PLAYLIST::LIST::BABES:
        {
            // table->showColumn(BabeTable::PLAYED);
            this->removeFromPlaylist->setVisible(false);
            this->removeBtn->setEnabled(true);
            mapList = this->connection->getBabedTracks();
            break;
        }

        case PLAYLIST::LIST::ONLINE:
        {
            // table->showColumn(BabeTable::PLAYED);
            this->removeFromPlaylist->setVisible(false);
            this->removeBtn->setEnabled(false);
            mapList = this->connection->getOnlineTracks();
            break;
        }

        case PLAYLIST::LIST::TAGS:
        {
            // table->showColumn(BabeTable::PLAYED);
            this->removeFromPlaylist->setVisible(false);
            this->removeBtn->setEnabled(false);
            QString query = "select distinct tag from tracks_tags "
                            "where context = 'tag' "
                            "and tag collate nocase not in "
                            "(select artist from artists) "
                            "and tag in "
                            "(select tag from tracks_tags group by tag having count(url) > 1) "
                            "order by tag collate nocase "
                            "limit 1000";
            this->populateTagList(query);
            break;
        }

        case PLAYLIST::LIST::RELATIONS:
        {
            // table->showColumn(BabeTable::PLAYED);
            this->removeFromPlaylist->setVisible(false);
            removeBtn->setEnabled(false);
            this->populateTagList("select distinct tag from tags "
                                  "where context = 'artist_similar' "
                                  "order by tag collate nocase");
            break;
        }

        case PLAYLIST::LIST::POPULAR:
        {
            this->removeFromPlaylist->setVisible(false);
            this->removeBtn->setEnabled(false);

            this->populateTagList("select artist as tag from artists order by artist");

            QString query ("select t.* from tracks t "
                           "inner join tracks_tags tt on tt.url = t.url "
                           "where tt.context = 'track_stat' "
                           "group by tt.url order by sum(tag) desc limit 250");

            mapList = this->connection->getDBData(query);
            break;
        }

        case PLAYLIST::LIST::GENRES:
        {
            this->removeFromPlaylist->setVisible(false);
            this->removeBtn->setEnabled(false);

            this->populateTagList("select distinct genre as tag from tracks order by genre");

            break;
        }

        default:
        {
            this->removeBtn->setEnabled(true);
            this->removeFromPlaylist->setVisible(true);
            this->table->hideColumn(static_cast<int>(BAE::KEY::PLAYED));
            mapList = this->connection->getPlaylistTracks(this->currentPlaylist, KEY::ADD_DATE, W::DESC);
            //        queryTxt = QString("SELECT * FROM tracks t INNER JOIN tracks_playlists tpl on tpl.tracks_url = t.url INNER JOIN playlists pl on pl.title = tpl.playlists_title WHERE pl.title = \"%1\" ORDER by addDate desc").arg(currentPlaylist);
            break;
        }
    }

    this->table->populateTableView(mapList);
}

void PlaylistsView::createPlaylist()
{
    auto item = new QListWidgetItem;
    item->setText("new playlist");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    this->list->addItem(item);
    this->currentPlaylist = "";
    emit this->list->doubleClicked(list->model()->index(this->list->count() - 1, 0));
    // item->setFlags (item->flags () & Qt::ItemIsEditable);
}

void PlaylistsView::removePlaylist()
{
    BAE::DB_LIST mapList;

    switch(this->list->currentRow())
    {
        case PLAYLIST::LIST::FAVORITES:
        {
            this->connection->execQuery("UPDATE tracks SET stars = \"0\" WHERE stars > \"0\"");
            this->table->flushTable();
            mapList = this->connection->getFavTracks();
            break;

        }
        case PLAYLIST::LIST::BABES:
        {
            this->connection->execQuery("UPDATE tracks SET babe = \"0\" WHERE babe > \"0\"");
            this->table->flushTable();
            mapList = this->connection->getBabedTracks();
            break;

        }
        default:
        {
            if(this->connection->removePlaylist(this->currentPlaylist))
            {
                this->table->flushTable();
                delete this->list->takeItem(this->list->currentRow());
            }
            return;
        }
    }

    this->table->populateTableView(mapList);
}


bool PlaylistsView::insertPlaylist(const QString &playlist)
{
    if(this->connection->addPlaylist(playlist)) return true;
    return false;

}

void PlaylistsView::refreshCurrentPlaylist()
{
    this->table->flushTable();
    this->table->populateTableView(this->connection->getPlaylistTracks(this->currentPlaylist));
}

void PlaylistsView::populateTagList(const QString &queryTxt)
{
    this->tagList->clear();
    this->line_v->setVisible(true);
    this->tagList->setVisible(true);

    for(auto tag :  this->connection->getDBData(queryTxt))
        this->tagList->addItem(tag[BAE::KEY::TAG]);
}

void PlaylistsView::playlistName(QListWidgetItem *item)
{
    qDebug() << "old playlist name: " << currentPlaylist
             << "new playlist name: " << item->text();
    //  qDebug()<<"new playlist name: "<<item->text();

    if(!this->playlists.contains(item->text()))
    {
        if (this->currentPlaylist.isEmpty())
        {
            if(!insertPlaylist(item->text()))
                delete this->list->takeItem(this->list->count() - 1);
            else this->playlists << item->text();

        } else if (item->text() != this->currentPlaylist)
        {
            emit this->modifyPlaylistName(item->text());
        }

    }else
    {
        qDebug()<<"that playlist already exists";
        this->list->takeItem(this->list->count() - 1);
    }
}

void PlaylistsView::setPlaylists()
{
    for (auto playlist : this->connection->getPlaylists())
    {
        auto item = new QListWidgetItem;
        item->setText(playlist);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        this->list->addItem(item);
    }
}

void PlaylistsView::saveToPlaylist(const BAE::DB_LIST &tracks)
{
    auto form = new PlaylistForm (this->connection->getPlaylists(), tracks);
    connect(form, &PlaylistForm::saved, this, &PlaylistsView::addToPlaylist);
    connect(form, &PlaylistForm::created, [=](const QString &playlist)
    {
        if(this->insertPlaylist(playlist))
        {
            auto item = new QListWidgetItem;
            item->setText(playlist);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            this->list->addItem(item);
        }
        form->deleteLater();
    });

    form->show();
}

void PlaylistsView::addToPlaylist(const QString &playlist, const BAE::DB_LIST &tracks)
{
    for (auto track : tracks)
        if(this->connection->trackPlaylist(track[BAE::KEY::URL], playlist))
            emit this->addedToPlaylist(tracks, playlist);
        else qDebug()<<"couldn't insert track:";
}

void PlaylistsView::setPlaylistsMoods()
{
    auto moodsLayout = new QHBoxLayout;
    this->moodWidget->setLayout(moodsLayout);

    auto moodGroup = new QButtonGroup(this->list);
    connect(moodGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [=](const int &mood)
    {
        this->currentPlaylist = this->moods[mood];

        this->table->flushTable();
        this->table->hideColumn(static_cast<int>(BAE::KEY::PLAYED));

        this->removeBtn->setEnabled(true);

        auto queryTxt = QString("SELECT * FROM tracks WHERE art = '%1'").arg(this->currentPlaylist);
        this->table->populateTableView(queryTxt);

        auto moodMap = BAE::loadSettings("MOODS", "SETTINGS", QMap<QString, QVariant>()).toMap();

        QStringList strValues;
        for(auto tag : moodMap[this->currentPlaylist].toStringList())
            if(!tag.isEmpty())
                strValues.append(QString("'%%1%'").arg(tag.trimmed()));

        queryTxt = QString("SELECT DISTINCT t.* FROM tracks t INNER JOIN tracks_tags tt ON tt.url = t.url WHERE tag LIKE %1 LIMIT 100").arg(strValues.join(" OR tag LIKE "));
        qDebug()<<  queryTxt;
        this->table->populateTableView(queryTxt);

    });

    for(int i=0; i<this->moods.size(); i++)
    {
        auto colorTag = new QToolButton(this->list);
        //colorTag->setIconSize(QSize(10,10));
        colorTag->setFixedSize(15, 15);
        // colorTag->setAutoRaise(true);
        colorTag->setStyleSheet(QString("QToolButton { background-color: %1;}").arg(this->moods[i]));
        moodGroup->addButton(colorTag,i);
        moodsLayout->addWidget(colorTag);
    }
}

