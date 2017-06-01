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

#include <QButtonGroup>
#include <QDir>
#include <QDebug>
#include <QEvent>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QGraphicsEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMap>
#include <QMenu>
#include <QMouseEvent>
#include <QProcess>
#include <QStringList>
#include <QTableWidgetItem>
#include <QTimer>
#include <QToolButton>
#include <QWidget>
#include <QWidgetAction>
#include <QVector>

#include "babetable.h"
#include <database/albumsdb.h>
#include <database/artistsdb.h>
#include <database/playlistsdb.h>
#include <database/tracksdb.h>

BabeTable::BabeTable(QWidget *parent) : QTableWidget(parent)
{
    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_tableWidget_doubleClicked(QModelIndex)));
    setFrameShape(QFrame::NoFrame);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(columnsCOUNT-1);
    setHorizontalHeaderLabels({"Track", "Title", "Artist", "Album", "Genre",
                               "Location", "Stars", "Babe", "Art", "Played",
                               "Playlist"});
    horizontalHeader()->setDefaultSectionSize(150);
    setMinimumSize(0, 0);
    verticalHeader()->setVisible(false);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAlternatingRowColors(true);
    setSortingEnabled(true);
    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

    setShowGrid(false);
    setColumnWidth(TRACK, 20);
    setColumnWidth(PLAYED, 20);
    setColumnWidth(STARS, 80);
    hideColumn(LOCATION);
    hideColumn(STARS);
    hideColumn(BABE);
    hideColumn(GENRE);
    hideColumn(TRACK);
    hideColumn(PLAYED);
    hideColumn(ART);
    hideColumn(PLAYLIST);

    fav1 = new QToolButton();
    fav2 = new QToolButton();
    fav3 = new QToolButton();
    fav4 = new QToolButton();
    fav5 = new QToolButton();

    fav1->setAutoRaise(true);
    fav1->setMaximumSize(16, 16);
    fav2->setAutoRaise(true);
    fav2->setMaximumSize(16, 16);
    fav3->setAutoRaise(true);
    fav3->setMaximumSize(16, 16);
    fav4->setAutoRaise(true);
    fav4->setMaximumSize(16, 16);
    fav5->setAutoRaise(true);
    fav5->setMaximumSize(16, 16);

    fav1->setIcon(QIcon::fromTheme("rating-unrated"));
    fav2->setIcon(QIcon::fromTheme("rating-unrated"));
    fav3->setIcon(QIcon::fromTheme("rating-unrated"));
    fav4->setIcon(QIcon::fromTheme("rating-unrated"));
    fav5->setIcon(QIcon::fromTheme("rating-unrated"));

    contextMenu = new QMenu(this);
    auto babeIt = new QAction("Babe it \xe2\x99\xa1", contextMenu);
    contextMenu->addAction(babeIt);

    auto queueIt = new QAction("Queue", contextMenu);
    contextMenu->addAction(queueIt);

    QAction *sendEntry = contextMenu->addAction("Send to phone...");
    sendToMenu = new QMenu("...");
    sendEntry->setMenu(sendToMenu);

    auto infoIt = new QAction("Info + ", contextMenu);
    contextMenu->addAction(infoIt);

    auto editIt = new QAction("Edit", contextMenu);
    contextMenu->addAction(editIt);

    auto saveIt = new QAction("Save to... ", contextMenu);
    contextMenu->addAction(saveIt);

    auto removeIt = new QAction("Remove", contextMenu);
    contextMenu->addAction(removeIt);

    QAction *addEntry = contextMenu->addAction("Add to...");
    playlistsMenu = new QMenu("...");
    addEntry->setMenu(playlistsMenu);

    connect(this, SIGNAL(rightClicked(const int, const int)), this, SLOT(setUpContextMenu(const int, const int)));
    connect(playlistsMenu, SIGNAL(triggered(QAction *)), this, SLOT(addToPlaylist(QAction *)));
    connect(sendToMenu, SIGNAL(triggered(QAction *)), this, SLOT(sendIt_action(QAction *)));

    connect(babeIt, SIGNAL(triggered()), this, SLOT(babeIt_action()));
    connect(infoIt, SIGNAL(triggered()), this, SLOT(infoIt_action()));
    connect(editIt, SIGNAL(triggered()), this, SLOT(editIt_action()));
    connect(queueIt, SIGNAL(triggered()), this, SLOT(queueIt_action()));
    connect(removeIt, SIGNAL(triggered()), this, SLOT(removeIt_action()));

    auto gr = new QWidget();
    auto ty = new QHBoxLayout();
    gr->setLayout(ty);

    QButtonGroup *bg = new QButtonGroup(contextMenu);
    connect(bg, SIGNAL(buttonClicked(int)), this, SLOT(rateGroup(int)));

    bg->addButton(fav1, 1);
    bg->addButton(fav2, 2);
    bg->addButton(fav3, 3);
    bg->addButton(fav4, 4);
    bg->addButton(fav5, 5);

    ty->addWidget(fav1);
    ty->addWidget(fav2);
    ty->addWidget(fav3);
    ty->addWidget(fav4);
    ty->addWidget(fav5);

    QWidgetAction *chkBoxAction = new QWidgetAction(contextMenu);
    chkBoxAction->setDefaultWidget(gr);

    contextMenu->addAction(chkBoxAction);
    auto moods = new QWidget();
    auto moodsLayout = new QHBoxLayout();
    QButtonGroup *moodGroup = new QButtonGroup(contextMenu);
    connect(moodGroup, SIGNAL(buttonClicked(int)), this, SLOT(moodTrack(int)));

    for (int i = 0; i < 5; i++) {
        auto *colorTag = new QToolButton();
        colorTag->setFixedSize(15, 15);
        colorTag->setStyleSheet(QString("QToolButton {background-color: %1;}").arg(colors.at(i)));
        moodGroup->addButton(colorTag, i);
        moodsLayout->addWidget(colorTag);
    }

    moods->setLayout(moodsLayout);
    QWidgetAction *moodsAction = new QWidgetAction(contextMenu);
    moodsAction->setDefaultWidget(moods);
    contextMenu->addAction(moodsAction);
    QFont helvetica("Helvetica", 10);

    addMusicTxt = new QLabel();
    addMusicTxt->setStyleSheet("QLabel {background-color:transparent;}");
    addMusicTxt->setText(addMusicMsg);
    addMusicTxt->setFont(helvetica);
    addMusicTxt->setWordWrap(true);
    addMusicTxt->setAlignment(Qt::AlignCenter);

    auto effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.5);
    addMusicTxt->setGraphicsEffect(effect);
    addMusicTxt->setAutoFillBackground(true);

    auto addMusicTxt_layout = new QHBoxLayout(this);
    addMusicTxt_layout->addStretch();
    addMusicTxt_layout->addWidget(addMusicTxt); // center alignment
    addMusicTxt_layout->addStretch();

    connect(this, SIGNAL(finishedPopulating()), this, SLOT(updateMusicMsg()));
}

BabeTable::~BabeTable()
{
}

void BabeTable::setAddMusicMsg(const QString &msg)
{
    addMusicMsg += msg;
    addMusicTxt->setText(addMusicMsg);
}

void BabeTable::updateMusicMsg()
{
    addMusicTxt->setVisible(rowCount() > 0 ? false : true);
}

void BabeTable::moodTrack(int color)
{
    moodIt_action(colors.at(color));
}

void BabeTable::addToPlaylist(QAction *action)
{
    QString playlist(action->text().replace("&", ""));
    QString location(getRowData(rRow)[LOCATION]);
    if (playlist.contains("Create new..."))
        emit createPlaylist_clicked();
    else
        populatePlaylist({location}, playlist);
}

void BabeTable::populatePlaylist(const QStringList &urls, const QString &playlist)
{
    QString list;
    QVariantMap map;
    for (auto location : urls) {
        qDebug() << "Song to add: " << location << " to: " << playlist;
        foreach (const QVariant &track, m_collectionDB.loadTracks(QVariantMap({{"location", location}}))) {
            map = track.toMap();
            list = map.value("playlist").toString();
        }
        list += " " + playlist;
        m_collectionDB.updateTrack("playlist", location, QVariant(list));
    }
    nof.notifyUrgent("Tracks added to playlist: ", playlist);
}

void BabeTable::passPlaylists()
{
}

void BabeTable::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
}

void BabeTable::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
}

void BabeTable::passStyle(const QString &style)
{
    setStyleSheet(style);
}

int BabeTable::getIndex()
{
    return currentIndex().row();
}

void BabeTable::addRow(const QMap<int, QString> &map, bool descriptiveTooltip, bool coloring)
{
    insertRow(rowCount());
    setItem(rowCount()-1, TRACK, new QTableWidgetItem(map[TRACK]));
    setItem(rowCount()-1, TITLE, new QTableWidgetItem(map[TITLE]));
    setItem(rowCount()-1, ARTIST, new QTableWidgetItem(map[ARTIST]));
    setItem(rowCount()-1, ALBUM, new QTableWidgetItem(map[ALBUM]));
    setItem(rowCount()-1, GENRE, new QTableWidgetItem(map[GENRE]));
    setItem(rowCount()-1, LOCATION, new QTableWidgetItem(map[LOCATION]));
    setItem(rowCount()-1, STARS, new QTableWidgetItem(map[STARS]));
    setItem(rowCount()-1, BABE, new QTableWidgetItem(map[BABE]));
    setItem(rowCount()-1, ART, new QTableWidgetItem(map[ART]));
    setItem(rowCount()-1, PLAYED, new QTableWidgetItem(map[PLAYED]));
    setItem(rowCount()-1, PLAYLIST, new QTableWidgetItem(map[PLAYLIST]));

    if (coloring && !map[ART].isEmpty())
        colorizeRow(rowCount()-1, map[ART]);
    if (descriptiveTooltip)
        item(rowCount()-1, TITLE)->setToolTip("by " + map[ARTIST]);
    updateMusicMsg();
}

void BabeTable::addRowAt(int row, const QMap<int, QString> &map, bool descriptiveTooltip, bool coloring)
{
    insertRow(row);
    setItem(row, TRACK, new QTableWidgetItem(map[TRACK]));
    setItem(row, TITLE, new QTableWidgetItem(map[TITLE]));
    setItem(row, ARTIST, new QTableWidgetItem(map[ARTIST]));
    setItem(row, ALBUM, new QTableWidgetItem(map[ALBUM]));
    setItem(row, GENRE, new QTableWidgetItem(map[GENRE]));
    setItem(row, LOCATION, new QTableWidgetItem(map[LOCATION]));
    setItem(row, STARS, new QTableWidgetItem(map[STARS]));
    setItem(row, BABE, new QTableWidgetItem(map[BABE]));
    setItem(row, ART, new QTableWidgetItem(map[ART]));
    setItem(row, PLAYED, new QTableWidgetItem(map[PLAYED]));

    if (coloring)
        colorizeRow(row, "#000");
    if (descriptiveTooltip)
        item(row, TITLE)->setToolTip("by " + map[ARTIST]);
}

void BabeTable::removeMissing(const QStringList &missingFiles)
{
    if (!missingFiles.size())
        return;
    nof.notifyUrgent("Removing missing files...", missingFiles.join("\n"));
    QString parentDir;
    for (auto file_r : missingFiles) {
        parentDir = QFileInfo(file_r).dir().path();
        m_collectionDB.removeTrack(BaeUtils::fileExists(parentDir) ? parentDir : file_r);
    }
    m_collectionDB.setCollectionLists();
    m_collectionDB.cleanCollectionLists();
}

void BabeTable::populateTableView(const QList<QMap<int, QString>> &mapList, bool descriptiveTitle, bool coloring)
{
    addMusicTxt->setVisible(!mapList.size());
    QString location;
    QStringList missingFiles;
    setSortingEnabled(false);
    if (!mapList.isEmpty()) {
        for (auto trackMap : mapList) {
            location = trackMap[LOCATION];
            if (!BaeUtils::fileExists(location))
                missingFiles << location;
            else
                addRow(trackMap, descriptiveTitle, coloring);
        }
        removeMissing(missingFiles);
        setSortingEnabled(true);
        emit finishedPopulating();
    }
}

void BabeTable::populateTableView(const QVariantList &contentList, bool descriptiveTitle, bool coloring)
{
    addMusicTxt->setVisible(!contentList.size());
    setSortingEnabled(false);
    if (contentList.size()) {
        int _babe = 0;
        QString bb, art, location, played, playlist, rating, track, title, artist, album, genre;
        QVariantMap map;
        QStringList missingFiles;
        bool missingDialog = false;
        foreach (const QVariant &item, contentList) {
            map = item.toMap();
            location = map.value("location").toString();
            if (location.isEmpty())
                continue;
            if (!BaeUtils::fileExists(location)) {
                qDebug() << "That file doesn't exists anymore: " << location;
                missingFiles << location;
                missingDialog = true;
            } else {
                track = map.value("track").toString();
                title = map.value("title").toString();
                artist = map.value("artist").toString();
                album = map.value("album").toString();
                genre = map.value("genre").toString();
                int stars = map.value("stars").toInt();
                switch (stars) {
                case 0:
                    rating = " ";
                    break;
                case 1:
                    rating = "\xe2\x98\x86 ";
                    break;
                case 2:
                    rating = "\xe2\x98\x86 \xe2\x98\x86 ";
                    break;
                case 3:
                    rating = "\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 ";
                    break;
                case 4:
                    rating = "\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 ";
                    break;
                case 5:
                    rating = "\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 ";
                    break;
                }
                _babe = map.value("babe").toInt();
                if (_babe == 1)
                    rating = "\xe2\x99\xa1 ";
                switch (_babe) {
                case 0:
                    bb = " ";
                    break;
                case 1:
                    bb = "\xe2\x99\xa1 ";
                    break;
                }
                art = map.value("art").toString();
                played = map.value("played").toString();
                playlist = map.value("playlist").toString();
                const QMap<int, QString> map{
                    {TRACK, track},
                    {TITLE, title},
                    {ARTIST, artist},
                    {ALBUM, album},
                    {GENRE, genre},
                    {LOCATION, location},
                    {STARS, QString(stars)},
                    {BABE, bb},
                    {ART, art},
                    {PLAYED, played},
                    {PLAYLIST, playlist}
                };
                addRow(map, descriptiveTitle, coloring);
            }
        }
        if (missingDialog)
            removeMissing(missingFiles);
        setSortingEnabled(true);
        emit finishedPopulating();
    }
}

void BabeTable::setRating(int rate)
{
    switch (rate) {
    case 0:
        fav1->setIcon(QIcon::fromTheme("rating-unrated"));
        fav2->setIcon(QIcon::fromTheme("rating-unrated"));
        fav3->setIcon(QIcon::fromTheme("rating-unrated"));
        fav4->setIcon(QIcon::fromTheme("rating-unrated"));
        fav5->setIcon(QIcon::fromTheme("rating-unrated"));
        break;
    case 1:
        fav1->setIcon(QIcon::fromTheme("rating"));
        fav2->setIcon(QIcon::fromTheme("rating-unrated"));
        fav3->setIcon(QIcon::fromTheme("rating-unrated"));
        fav4->setIcon(QIcon::fromTheme("rating-unrated"));
        fav5->setIcon(QIcon::fromTheme("rating-unrated"));
        break;
    case 2:
        fav1->setIcon(QIcon::fromTheme("rating"));
        fav2->setIcon(QIcon::fromTheme("rating"));
        fav3->setIcon(QIcon::fromTheme("rating-unrated"));
        fav4->setIcon(QIcon::fromTheme("rating-unrated"));
        fav5->setIcon(QIcon::fromTheme("rating-unrated"));
        break;
    case 3:
        fav1->setIcon(QIcon::fromTheme("rating"));
        fav2->setIcon(QIcon::fromTheme("rating"));
        fav3->setIcon(QIcon::fromTheme("rating"));
        fav4->setIcon(QIcon::fromTheme("rating-unrated"));
        fav5->setIcon(QIcon::fromTheme("rating-unrated"));
        break;
    case 4:
        fav1->setIcon(QIcon::fromTheme("rating"));
        fav2->setIcon(QIcon::fromTheme("rating"));
        fav3->setIcon(QIcon::fromTheme("rating"));
        fav4->setIcon(QIcon::fromTheme("rating"));
        fav5->setIcon(QIcon::fromTheme("rating-unrated"));
        break;
    case 5:
        fav1->setIcon(QIcon::fromTheme("rating"));
        fav2->setIcon(QIcon::fromTheme("rating"));
        fav3->setIcon(QIcon::fromTheme("rating"));
        fav4->setIcon(QIcon::fromTheme("rating"));
        fav5->setIcon(QIcon::fromTheme("rating"));
        break;
    }
}

void BabeTable::setTableOrder(int column, int order)
{
    sortByColumn(column, order == ASCENDING ? Qt::AscendingOrder : Qt::DescendingOrder);
}

void BabeTable::setVisibleColumn(int column)
{
    showColumn(column);
}

QMap<QString, QString> BabeTable::getKdeConnectDevices()
{
    qDebug() << "getting the kdeconnect devices avaliable";
    QMap<QString, QString> _devices;
    QString line;
    QProcess process;
    QStringList items;
    process.start("kdeconnect-cli -a");
    process.waitForFinished();
    process.setReadChannel(QProcess::StandardOutput);
    while (process.canReadLine()) {
        line = QString::fromLocal8Bit(process.readLine());
        qDebug() << "line: " << line;
        if (line.contains("(paired and reachable)")) {
            items = line.split(" ");
            auto key = QString(items.at(2));
            auto name = QString(items.at(1)).replace(":", "");
            qDebug() << "Founded devices: " << key << ":" << name;
            _devices.insert(key, name);
        }
    }
    devices = _devices;
    return devices;
}

void BabeTable::setUpContextMenu(const int row, const int column)
{    
    qDebug() << "setUpContextMenu()...";
    int rate = 0;
    int babe = 0;
    rRow = row;
    rColumn = column;
    playlistsMenu->clear();

    for (auto playlist : m_collectionDB.getPlaylists())
        playlistsMenu->addAction(playlist);
    sendToMenu->clear();

    QMapIterator<QString, QString> i(getKdeConnectDevices());
    while (i.hasNext()) {
        i.next();
        sendToMenu->addAction(i.value());
    }

    QVariantMap map;
    QVariantList _tracks(m_collectionDB.loadTracks(QVariantMap({{"location", getRowData(rRow)[LOCATION]}})));
    foreach (const QVariant &entry, _tracks) {
        map = entry.toMap();
        rate = map.value("stars").toInt();
        babe = map.value("babe").toInt();
    }

    setRating(rate);
    contextMenu->actions().at(0)->setText(babe ? "Un-Babe it" : "Babe it");
    contextMenu->exec(QCursor::pos());
}

QStringList BabeTable::getPlaylistMenus()
{
    playlistsMenus.clear();
    for (auto playlist : m_collectionDB.getPlaylists())
        playlistsMenus << playlist;
    return playlistsMenus;
}

void BabeTable::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Return: {
        QList<QMap<int, QString>> list;
        list << getRowData(getIndex());
        emit tableWidget_doubleClicked(list);
        break;
    }
    case Qt::Key_Up: {
        QModelIndex index = currentIndex();
        int row = index.row() - 1;
        int column = 1;
        QModelIndex newIndex = model()->index(row, column);
        selectionModel()->select(newIndex, QItemSelectionModel::Select);
        setCurrentIndex(newIndex);
        setFocus();
        break;
    }
    case Qt::Key_Down: {
        QModelIndex index = currentIndex();
        int row = index.row() + 1;
        int column = 1;
        QModelIndex newIndex = model()->index(row, column);
        selectionModel()->select(newIndex, QItemSelectionModel::Select);
        setCurrentIndex(newIndex);
        setFocus();
        break;
    }
    default: {
        QTableWidget::keyPressEvent(event);
        break;
    }
    }
}

void BabeTable::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::RightButton) {
        qDebug() << "table right clicked at row|column: ";
        evt->accept();
        int row = indexAt(evt->pos()).row();
        int column = indexAt(evt->pos()).column();
        qDebug() << row << column;
        if (row != -1)
            emit rightClicked(row, column);
    }
    QTableWidget::mousePressEvent(evt);
}

void BabeTable::rateGroup(int id)
{
    qDebug() << "rated with: " << id;
    QString location(rowInfo(LOCATION));
    if (!m_collectionDB.trackExists(QVariantMap({{"location", location}}))) {
        m_collectionDB.addTrack({location});
        qDebug() << "track '" << location << "' not exists! Will be inserted now!";
    }

    m_collectionDB.updateTrack("stars", location, QVariant(id));
    setRating(id);

    QString stars;
    for (int i = 0; i < id; i++)
        stars += "\xe2\x98\x86 ";

    item(rRow, STARS)->setText(stars);

    if (id > 0) {
        qDebug() << "rated and trying to add to favs";
        emit songRated({rowInfo(TITLE), rowInfo(ARTIST), rowInfo(ALBUM), rowInfo(LOCATION), rowInfo(STARS), rowInfo(BABE)});
    } else {
        qDebug() << "rated and trying to add to favs failed";
    }
}

QMap<int, QString> BabeTable::getRowData(int row)
{
    QMap<int, QString> map;
    QString location(model()->data(model()->index(row, LOCATION)).toString());
    if (!location.isEmpty()) {
        QList<QMap<int, QString>> mapList = m_collectionDB.getTrackData(QVariantMap({{"location", location}}));
        if (!mapList.isEmpty())
            return mapList.first();
    }
    return map;
}

void BabeTable::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    QList<QMap<int, QString>> list;
    auto track = getRowData(getIndex());
    list << track;
    emit tableWidget_doubleClicked(list);
}

void BabeTable::babeIt_action()
{    
    emit babeIt_clicked(getRowData(rRow));
}

void BabeTable::sendIt_action(QAction *device)
{
    auto track = getRowData(rRow);
    QString url(track[LOCATION]);
    QString title(track[TITLE]);
    QString artist(track[ARTIST]);
    QString deviceName(device->text().replace("&", ""));
    QString deviceKey(devices.key(deviceName));
    qDebug() << "trying to send '" << url << "' to: " << deviceName;
    auto process = new QProcess(this);
    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << "process finished with status " << exitCode << exitStatus;
        nof.notifyUrgent("Song sent to " + deviceName, title + (artist.isEmpty() ? "" : (" by " + artist)));
    });
    qDebug() << "kdeconnect-cli -d " + deviceKey + " --share " + url;
    process->start("kdeconnect-cli -d " + deviceKey + " --share " + "\"" + url + "\"");
}

void BabeTable::editIt_action()
{
    auto infoForm = new metadataForm(getRowData(rRow), this);
    connect(infoForm, SIGNAL(infoModified(QMap<int, QString>)), this, SLOT(itemEdited(QMap<int, QString>)));
    infoForm->show();
}

void BabeTable::itemEdited(const QMap<int, QString> &map)
{
    qDebug() << "item changed: " << map[TITLE];
    item(rRow, TRACK)->setText(map[TRACK]);
    item(rRow, TITLE)->setText(map[TITLE]);
    item(rRow, ARTIST)->setText(map[ARTIST]);
    item(rRow, ALBUM)->setText(map[ALBUM]);
    item(rRow, GENRE)->setText(map[GENRE]);
}

void BabeTable::infoIt_action()
{
    emit infoIt_clicked(rowInfo(TITLE), rowInfo(ARTIST), rowInfo(ALBUM));
}

void BabeTable::removeIt_action()
{
    qDebug() << "removeIt/right clicked!";
    qDebug() << "LOCATION: " << rowInfo(LOCATION);
    removeRow(rRow);
    emit removeIt_clicked(rRow);
}

void BabeTable::moodIt_action(const QString &color)
{
    if (!color.isEmpty()) {
        qDebug() << "try to save art with color " << color << " into DB...";
        int result = m_collectionDB.updateTrack("art", rowInfo(LOCATION), QVariant(color));
        qDebug() << "result: " << result;
        contextMenu->close();
        moodIt_clicked(rRow, color);
    }
}

void BabeTable::colorizeRow(const int &row, const QString &color)
{
    QColor coloring(color);
    coloring.setAlpha(40);
    item(row, TITLE)->setBackgroundColor(coloring);
}

void BabeTable::queueIt_action()
{
    qDebug() << "queueIt clicked!";
    QString url(rowInfo(LOCATION));
    qDebug() << "url: " << url;
    emit queueIt_clicked(getRowData(rRow));
}

void BabeTable::flushTable()
{
    clearContents();
    setRowCount(0);
}

QStringList BabeTable::getTableColumnContent(const columns &column)
{
    QStringList result;
    int rowSize = rowCount();
    for (int i = 0; i < rowSize; i++)
        result << rowInfo(column, i);
    return result;
}

QList<QMap<int, QString>> BabeTable::getAllTableContent()
{
    QList<QMap<int, QString>> mapList;
    int rowSize = rowCount();
    for (int i = 0; i < rowSize; i++)
        mapList << getRowData(i);
    return mapList;
}

QString BabeTable::rowInfo(int column, int rowNumber)
{
    if (rowNumber == -1)
        rowNumber = rRow;
    return model()->data(model()->index(rowNumber, column)).toString();
}

void BabeTable::removeRepeated() //tofix
{
    auto list = getTableColumnContent(BabeTable::LOCATION);
    list.removeDuplicates();
    flushTable();
    populateTableView(m_collectionDB.getTrackData(list), true, true);
}
