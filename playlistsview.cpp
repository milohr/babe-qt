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
#include "database/tracksdb.h"
#include "database/playlistsdb.h"

#include <QDebug>
#include <QGridLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStandardPaths>
#include <QToolButton>

PlaylistsView::PlaylistsView(QWidget *parent) : QWidget(parent)
  ,m_tracksDB(TracksDB::instance())
  ,m_playlistsDB(PlaylistsDB::instance())
{
    layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    table = new BabeTable(this);

    list = new QListWidget(this);
    list->setFixedWidth(120);
    list->setAlternatingRowColors(true);
    list->setFrameShape(QFrame::NoFrame);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(list, SIGNAL(doubleClicked(QModelIndex)), list, SLOT(edit(QModelIndex)));
    connect(list, SIGNAL(clicked(QModelIndex)), this, SLOT(populatePlaylist(QModelIndex)));
    connect(list, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(playlistName(QListWidgetItem *)));

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
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(5, 0, 5, 0);
    btnContainer->setLayout(btnLayout);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(left_spacer);
    btnLayout->addWidget(removeBtn);

    line_v = new QFrame();
    line_v->setFrameShape(QFrame::VLine);
    line_v->setFrameShadow(QFrame::Plain);
    line_v->setMaximumWidth(1);

    layout->addWidget(list, 0, 0, Qt::AlignLeft);
    layout->addWidget(line_v, 0, 1, Qt::AlignLeft);
    layout->addWidget(table, 0, 2);

    this->setLayout(layout);
}

PlaylistsView::~PlaylistsView()
{
}

void PlaylistsView::dummy()
{
    qDebug() << "signal was recived";
}

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
}

void PlaylistsView::tableClicked(QStringList list)
{
    emit songClicked(list);
}

void PlaylistsView::populatePlaylist(QModelIndex index)
{
    currentPlaylist = index.data().toString();
    emit playlistClicked(currentPlaylist);
    table->flushTable();
    QVariantList list;
    if (currentPlaylist == "Most Played") {
        removeBtn->setEnabled(false);
        table->showColumn(BabeTable::PLAYED);
        list = m_tracksDB->loadTracks(QVariantMap({{"played", 1}}), -1, 0, "played", false, "", ">");
    } else if (currentPlaylist == "Favorites") {
        removeBtn->setEnabled(true);
        table->showColumn(BabeTable::STARS);
        list = m_tracksDB->loadTracks(QVariantMap({{"stars", 0}}), -1, 0, "stars", false, "", ">");
    } else if (currentPlaylist == "Babes") {
        removeBtn->setEnabled(true);
        list = m_tracksDB->loadTracks(QVariantMap({{"babe", 1}}), -1, 0, "played", true, "", ">");
    } else if (currentPlaylist == "Online") {
        removeBtn->setEnabled(false);
        list = m_tracksDB->loadTracks(QVariantMap({{"location", youtubeCachePath}}), -1, 0, "title", false, "", "LIKE");
    } else if(!currentPlaylist.isEmpty()&&!currentPlaylist.contains("#")) {
        removeBtn->setEnabled(true);
        table->hideColumn(BabeTable::PLAYED);
        list = m_tracksDB->loadTracks(QVariantMap({{"playlist", currentPlaylist}}), -1, 0, "title", false, "", "LIKE");
    } else if (currentPlaylist.contains("#")) {
        removeBtn->setEnabled(true);
        table->hideColumn(BabeTable::PLAYED);
        list = m_tracksDB->loadTracks(QVariantMap({{"art", currentPlaylist}}), -1, 0, "title", false, "", "=");
    }
    table->populateTableView(list, false, false);
}

void PlaylistsView::createPlaylist()
{
    auto *item = new QListWidgetItem("new playlist");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    list->addItem(item);
    currentPlaylist = "";
    emit list->doubleClicked(list->model()->index(list->count() - 1, 0));
}

void PlaylistsView::removePlaylist()
{
    if (currentPlaylist == "Most Played") {
        // add this operation
    } else if (currentPlaylist == "Favorites") {
        m_playlistsDB->update(QVariantMap({{"stars", 0}}), QVariantMap({{"stars", 0}}), "", ">");
        table->flushTable();
        table->populateTableView(m_tracksDB->loadTracks(QVariantMap({{"stars", 0}}), -1, 0, "stars", true), false, false);
    } else if (currentPlaylist == "Babes") {
        m_tracksDB->update(QVariantMap({{"babe", 0}}), QVariantMap({{"babe", 0}}), "", ">");
        table->flushTable();
        table->populateTableView(m_tracksDB->loadTracks(QVariantMap({{"babe", 1}}), -1, 0, "played", true), false, false);
    } else if (currentPlaylist == "Online") {
        // add this operation
    } else if (!currentPlaylist.isEmpty() && !currentPlaylist.contains("#")) {
        // add this operation
    } else if (currentPlaylist.contains("#")) {
        m_tracksDB->update(QVariantMap({{"art", ""}}), QVariantMap({{"art", currentPlaylist}}));
        m_playlistsDB->remove(QVariantMap({{"art", currentPlaylist}}));
        table->flushTable();
        table->populateTableView(m_tracksDB->loadTracks(QVariantMap({{"art", currentPlaylist}})), false, false);
    }
}

void PlaylistsView::playlistName(QListWidgetItem *item)
{
    qDebug() << "old playlist name: " << currentPlaylist
             << "new playlist name: " << item->text();
    if (!playlists.contains(item->text())) {
        if (currentPlaylist.isEmpty())
            emit playlistCreated(item->text());
        else if (item->text() != currentPlaylist)
            emit modifyPlaylistName(item->text());
    } else {
        qDebug() << "that playlist already exists";
        list->takeItem(list->count() - 1);
    }
}

void PlaylistsView::on_removeBtn_clicked()
{
}

void PlaylistsView::setPlaylists(const QStringList &playlists)
{
    for (auto playlist : playlists) {
        auto item = new QListWidgetItem(playlist);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        list->addItem(item);
    }
}

void PlaylistsView::setPlaylistsMoods(const QStringList &moods_n)
{
    QColor color;
    for (auto mood : moods_n) {
        auto item = new QListWidgetItem(mood);
        color.setNamedColor(mood);
        color.setAlpha(40);
        item->setBackgroundColor(color);
        list->addItem(item);
    }
}

void PlaylistsView::definePlaylists(const QStringList &playlists)
{
    this->playlists = playlists;
}
