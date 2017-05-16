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

#include "babetable.h"

#include <database/tracksdb.h>

BabeTable::BabeTable(QWidget *parent) : QTableWidget(parent)
{
    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_tableWidget_doubleClicked(QModelIndex)));
    this->setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setColumnCount(columnsCOUNT-1);
    this->setHorizontalHeaderLabels({"Track", "Title", "Artist", "Album", "Genre",
                                     "Location", "Stars", "Babe", "Art", "Played",
                                     "Playlist"});
    this->horizontalHeader()->setDefaultSectionSize(150);
    this->setMinimumSize(0, 0);
    this->verticalHeader()->setVisible(false);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setAlternatingRowColors(true);
    this->setSortingEnabled(true);
    this->horizontalHeader()->setHighlightSections(false);
    this->horizontalHeader()->setStretchLastSection(true);
    this->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

    this->setShowGrid(false);
    this->setColumnWidth(TRACK, 20);
    this->setColumnWidth(PLAYED, 20);
    this->setColumnWidth(STARS, 80);
    this->hideColumn(LOCATION);
    this->hideColumn(STARS);
    this->hideColumn(BABE);
    this->hideColumn(GENRE);
    this->hideColumn(TRACK);
    this->hideColumn(PLAYED);
    this->hideColumn(ART);
    this->hideColumn(PLAYLIST);

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
    connect(queueIt, SIGNAL(triggered()), this, SLOT(queueIt_action()));
    connect(infoIt, SIGNAL(triggered()), this, SLOT(infoIt_action()));
    connect(editIt, SIGNAL(triggered()), this, SLOT(editIt_action()));
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
        colorTag->setStyleSheet(QString("QToolButton { background-color: %1;}").arg(colors.at(i)));
        moodGroup->addButton(colorTag, i);
        moodsLayout->addWidget(colorTag);
    }

    moods->setLayout(moodsLayout);
    QWidgetAction *moodsAction = new QWidgetAction(contextMenu);
    moodsAction->setDefaultWidget(moods);
    contextMenu->addAction(moodsAction);
    QFont helvetica("Helvetica", 10);
    addMusicTxt = new QLabel();
    addMusicTxt->setStyleSheet("QLabel{background-color:transparent;}");
    addMusicTxt->setText(addMusicMsg);
    addMusicTxt->setFont(helvetica);
    addMusicTxt->setWordWrap(true);
    addMusicTxt->setAlignment(Qt::AlignCenter);
    addMusicTxt->setVisible(false);

    auto effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.5);
    addMusicTxt->setGraphicsEffect(effect);
    addMusicTxt->setAutoFillBackground(true);

    auto addMusicTxt_layout = new QHBoxLayout(this);
    addMusicTxt_layout->addStretch();
    addMusicTxt_layout->addWidget(addMusicTxt); // center alignment
    addMusicTxt_layout->addStretch();

    connect(updater, SIGNAL(timeout()), this, SLOT(update()));
    updater->start(1000);
}

BabeTable::~BabeTable()
{
}

void BabeTable::setAddMusicMsg(const QString &msg)
{
    addMusicMsg += msg;
    addMusicTxt->setText(addMusicMsg);
}

void BabeTable::update()
{
    if (this->rowCount() != 0) {
        if (addMusicTxt->isVisible())
            addMusicTxt->setVisible(false);
    } else {
        addMusicTxt->setVisible(true);
    }
}

void BabeTable::moodTrack(int color)
{
    moodIt_action(colors.at(color));
}

void BabeTable::addToPlaylist(QAction *action)
{
    QString playlist = action->text().replace("&", "");
    QString location = getRowData(rRow)[LOCATION];

    if (playlist.contains("Create new..."))
        emit createPlaylist_clicked();
    else
        populatePlaylist({location}, playlist);
}

void BabeTable::populatePlaylist(const QStringList &urls, const QString &playlist)  //this needs to get fixed
{
    for (auto location : urls) {
        qDebug() << "Song to add: " << location << " to: " << playlist;
        QSqlQuery query = connection->getQuery("SELECT * FROM tracks WHERE location = \"" + location + "\"");
        QString list;
        while (query.next())
            list = query.value(PLAYLIST).toString();
        list += " " + playlist;
        if (connection->insertInto("tracks", "playlist", location, list))
            qDebug() << list;
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
    this->setStyleSheet(style);
}

int BabeTable::getIndex()
{
    return this->currentIndex().row();
}

void BabeTable::addRow(const QMap<int, QString> &map, bool descriptiveTooltip, bool coloring)
{
    this->insertRow(this->rowCount());
    this->setItem(this->rowCount() - 1, TRACK, new QTableWidgetItem(map[TRACK]));
    this->setItem(this->rowCount() - 1, TITLE, new QTableWidgetItem(map[TITLE]));
    this->setItem(this->rowCount() - 1, ARTIST, new QTableWidgetItem(map[ARTIST]));
    this->setItem(this->rowCount() - 1, ALBUM, new QTableWidgetItem(map[ALBUM]));
    this->setItem(this->rowCount() - 1, GENRE, new QTableWidgetItem(map[GENRE]));
    this->setItem(this->rowCount() - 1, LOCATION, new QTableWidgetItem(map[LOCATION]));
    this->setItem(this->rowCount() - 1, STARS, new QTableWidgetItem(map[STARS]));
    this->setItem(this->rowCount() - 1, BABE, new QTableWidgetItem(map[BABE]));
    this->setItem(this->rowCount() - 1, ART, new QTableWidgetItem(map[ART]));
    this->setItem(this->rowCount() - 1, PLAYED, new QTableWidgetItem(map[PLAYED]));
    this->setItem(this->rowCount() - 1, PLAYLIST, new QTableWidgetItem(map[PLAYLIST]));

    if (coloring && !map[ART].isEmpty())
        this->colorizeRow(this->rowCount()-1, map[ART]);
    if (descriptiveTooltip)
        this->item(this->rowCount()-1, TITLE)->setToolTip("by " + map[ARTIST]);
}

void BabeTable::addRowAt(int row, const QMap<int, QString> &map, bool descriptiveTooltip, bool coloring)
{
    this->insertRow(row);
    this->setItem(row, TRACK, new QTableWidgetItem(map[TRACK]));
    this->setItem(row, TITLE, new QTableWidgetItem(map[TITLE]));
    this->setItem(row, ARTIST, new QTableWidgetItem(map[ARTIST]));
    this->setItem(row, ALBUM, new QTableWidgetItem(map[ALBUM]));
    this->setItem(row, GENRE, new QTableWidgetItem(map[GENRE]));
    this->setItem(row, LOCATION, new QTableWidgetItem(map[LOCATION]));
    this->setItem(row, STARS, new QTableWidgetItem(map[STARS]));
    this->setItem(row, BABE, new QTableWidgetItem(map[BABE]));
    this->setItem(row, ART, new QTableWidgetItem(map[ART]));
    this->setItem(row, PLAYED, new QTableWidgetItem(map[PLAYED]));

    if (coloring)
        this->colorizeRow(row, "#000");
    if (descriptiveTooltip)
        this->item(row,TITLE)->setToolTip("by " + map[ARTIST]);
}

void BabeTable::populateTableView(const QList<QMap<int,QString>> &mapList, bool descriptiveTitle, bool coloring)
{
    qDebug() << "ON POPULATE by mapList";

    this->setSortingEnabled(false);
    bool missing = false;
    QStringList missingFiles;
    if (!mapList.isEmpty()) {
        for (auto trackMap : mapList) {
            QString location = trackMap[LOCATION];
            if (!BaeUtils::fileExists(location)) {
                qDebug() << "That file doesn't exists anymore: " << location;
                missingFiles << location;
                missing = true;
            } else {
                addRow(trackMap,descriptiveTitle,coloring);
            }
        }
        if (missing && missingFiles.size())
            removeMissing(missingFiles);
        this->setSortingEnabled(true);
        emit finishedPopulating();
    } else {
        qDebug() << "Error: the mapList is empty";
    }
}

void BabeTable::removeMissing(const QStringList &missingFiles)
{
    nof.notifyUrgent("Removing missing files...", missingFiles.join("\n"));
    for (auto file_r : missingFiles) {
        QString parentDir = QFileInfo(QFileInfo(file_r)).dir().path();
        if (!BaeUtils::fileExists(parentDir))
            connection->removePath(parentDir);
        else
            connection->removePath(file_r);
    }
    connection->setCollectionLists();
    connection->cleanCollectionLists();
}

void BabeTable::populateTableView(const QString &indication, bool descriptiveTitle, bool coloring)
{
    qDebug() << "ON POPULATE:" << indication;
    this->setSortingEnabled(false);
    bool missingDialog = false;
    QStringList missingFiles;
    QSqlQuery query = connection->getQuery(indication);
    if (query.exec()) {
        while (query.next()) {
            QString location =query.value(LOCATION).toString();
            if (!BaeUtils::fileExists(location)) {
                qDebug() << "That file doesn't exists anymore: " << location;
                missingFiles << location;
                missingDialog = true;
            } else {
                QString track = query.value(TRACK).toString();
                QString title = query.value(TITLE).toString();
                QString artist = query.value(ARTIST).toString();
                QString album = query.value(ALBUM).toString();
                QString genre = query.value(GENRE).toString();
                QString rating;
                switch (query.value((STARS)).toInt()) {
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
                if (query.value(BABE).toInt() == 1)
                    rating = "\xe2\x99\xa1 ";
                QString stars = rating;
                QString bb;
                switch (query.value((BABE)).toInt()) {
                case 0:
                    bb = " ";
                    break;
                case 1:
                    bb = "\xe2\x99\xa1 ";
                    break;
                }
                QString babe = bb;
                QString art = query.value(ART).toString();
                QString played = query.value(PLAYED).toString();
                QString playlist = query.value(PLAYLIST).toString();
                const QMap<int, QString> map{{TRACK, track}, {TITLE, title}, {ARTIST, artist}, {ALBUM, album}, {GENRE, genre}, {LOCATION, location}, {STARS, stars}, {BABE, babe}, {ART, art}, {PLAYED, played}, {PLAYLIST, playlist}};
                addRow(map, descriptiveTitle, coloring);
            }
        }
        if (missingDialog)
            removeMissing(missingFiles);
        this->setSortingEnabled(true);
        emit finishedPopulating();
    } else {
        qDebug() << "Error: the query didn't pass" << indication;
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
    if (order == DESCENDING)
        this->sortByColumn(column, Qt::DescendingOrder);
    else if (order == ASCENDING)
        this->sortByColumn(column, Qt::AscendingOrder);
}

void BabeTable::setVisibleColumn(int column)
{
    if (column == LOCATION)
        this->showColumn(LOCATION);
    else if (column == STARS)
        this->showColumn(STARS);
    else if (column == BABE)
        this->showColumn(BABE);
    else if (column == ALBUM)
        this->showColumn(ALBUM);
}

QMap<QString, QString> BabeTable::getKdeConnectDevices()
{
    qDebug() << "getting the kdeconnect devices avaliable";
    QMap<QString,QString> _devices;
    QProcess process;
    process.start("kdeconnect-cli -a");
    process.waitForFinished();
    process.setReadChannel(QProcess::StandardOutput);
    while (process.canReadLine()) {
        QString line = QString::fromLocal8Bit(process.readLine());
        qDebug() << "line:" << line;
        if (line.contains("(paired and reachable)")) {
            QStringList items = line.split(" ");
            auto key = QString(items.at(2));
            auto name = QString(items.at(1)).replace(":", "");
            qDebug() << "Founded devices: " << key << ":" << name;
            _devices.insert(key,name);
        }
    }
    this->devices = _devices;
    return devices;
}

void BabeTable::setUpContextMenu(const int row, const int column)
{    
    qDebug() << "setUpContextMenu";
    int rate = 0;
    bool babe = false;
    this->rRow = row;
    this->rColumn = column;
    playlistsMenu->clear();
    for (auto playlist : connection->getPlaylists())
        playlistsMenu->addAction(playlist);
    sendToMenu->clear();

    QMapIterator<QString, QString> i(getKdeConnectDevices());
    while (i.hasNext()) {
        i.next();
        qDebug() << i.key();
        sendToMenu->addAction(i.value());
    }

    QString url = this->getRowData(rRow)[LOCATION];
    QSqlQuery query = connection->getQuery("SELECT * FROM tracks WHERE location = \"" + url + "\"");
    while (query.next()) {
        rate = query.value(STARS).toInt();
        babe = query.value(BABE).toInt() == 1 ? true : false;
    }
    setRating(rate);
    if (babe)
        contextMenu->actions().at(0)->setText("Un-Babe it");
    else
        contextMenu->actions().at(0)->setText("Babe it");
    contextMenu->exec(QCursor::pos());
}

QStringList BabeTable::getPlaylistMenus()
{
    playlistsMenus.clear();
    for (auto playlist : connection->getPlaylists())
        playlistsMenus << playlist;
    return playlistsMenus;
}

void BabeTable::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Return: {
            QList<QMap<int, QString>> list;
            list<<getRowData(this->getIndex());
            emit tableWidget_doubleClicked(list);
            break;
        }
        case Qt::Key_Up: {
            QModelIndex index = this->currentIndex();
            int row = index.row() - 1;
            int column = 1;
            QModelIndex newIndex = this->model()->index(row, column);
            this->selectionModel()->select(newIndex, QItemSelectionModel::Select);
            this->setCurrentIndex(newIndex);
            this->setFocus();
            qDebug() << this->model()->data(this->model()->index(row, LOCATION)).toString();
            break;
        }
        case Qt::Key_Down: {
            QModelIndex index = this->currentIndex();
            int row = index.row() + 1;
            int column = 1;
            QModelIndex newIndex = this->model()->index(row, column);
            this->selectionModel()->select(newIndex, QItemSelectionModel::Select);
            this->setCurrentIndex(newIndex);
            this->setFocus();
            qDebug() << this->model()->data(this->model()->index(row, LOCATION)).toString();
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
        qDebug() << "table right clicked";
        evt->accept();
        int row = this->indexAt(evt->pos()).row();
        int column = this->indexAt(evt->pos()).column();
        qDebug() << row << column;
        if (row != -1)
            emit rightClicked(row, column);
    }
    QTableWidget::mousePressEvent(evt);
}

void BabeTable::rateGroup(int id)
{
    qDebug() << "rated with: " << id;
    QString location = this->getRowData(rRow)[LOCATION];
    QSqlQuery query = connection->getQuery("SELECT * FROM tracks WHERE location = \"" + location + "\"");
    int rate = 0;
    while (query.next())
        rate = query.value(STARS).toInt();
    if (connection->check_existance("tracks", "location", location)) {
        if (connection->insertInto("tracks", "stars", location, id))
            setRating(id);
        QString stars;
        for (int i = 0; i < id; i++)
            stars += "\xe2\x98\x86 ";
        this->item(rRow, STARS)->setText(stars);
        if (id > 0 && rate < 5) {
            QString title = this->model()->data(this->model()->index(rRow, TITLE)).toString();
            QString artist = this->model()->data(this->model()->index(rRow, ARTIST)).toString();
            QString album = this->model()->data(this->model()->index(rRow, ALBUM)).toString();
            QString star = this->model()->data(this->model()->index(rRow, STARS)).toString();
            QString babe = this->model()->data(this->model()->index(rRow, BABE)).toString();
            qDebug() << "rated and trying to add to favs";
            emit songRated({title, artist, album, location, star, babe});
        } else {
            qDebug() << "rated and trying to add to favs failed";
        }
    }
}

QMap<int, QString> BabeTable::getRowData(int row)
{  
    QMap<int, QString> map;
    QList<QMap<int, QString>> mapList;
    QString location = this->model()->data(this->model()->index(row, LOCATION)).toString();
    qDebug() << "location: " << location;
    if (!location.isEmpty()) {
        mapList = connection->getTrackData("SELECT * FROM tracks WHERE location = \"" + location + "\"");
        if (!mapList.isEmpty())
            return mapList.first();
    }
    return map;
}

void BabeTable::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    QList<QMap<int, QString>> list;
    auto track = getRowData(this->getIndex());
    list << track;
    qDebug() << "BabeTable doubleClicked item<<" << track[LOCATION];
    emit tableWidget_doubleClicked(list);
}

void BabeTable::babeIt_action()
{    
    emit babeIt_clicked(this->getRowData(rRow));
}

void BabeTable::sendIt_action(QAction *device)
{
    auto track = this->getRowData(rRow);
    QString url = track[LOCATION];
    QString title = track[TITLE];
    QString artist = track[ARTIST];
    QString deviceName = device->text().replace("&", "");
    QString deviceKey = devices.key(deviceName);
    qDebug() << "trying to send "<< url << "to : "<< deviceName;
    auto process = new QProcess(this);
    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << "processFinished_totally" << exitCode << exitStatus;
        nof.notifyUrgent("Song sent to " + deviceName, title + " by " + artist);
    });
    qDebug() << "kdeconnect-cli -d " + deviceKey + " --share " + url;
    process->start("kdeconnect-cli -d " + deviceKey + " --share " + "\"" + url + "\"");
}

void BabeTable::editIt_action()
{
    auto infoForm = new metadataForm(getRowData(rRow),this);
    connect(infoForm, SIGNAL(infoModified(QMap<int, QString>)), this, SLOT(itemEdited(QMap<int, QString>)));
    infoForm->show();
}

void BabeTable::itemEdited(const QMap<int, QString> &map)
{
    qDebug() << "item changed: " << map[TITLE];
    this->item(rRow, TRACK)->setText(map[TRACK]);
    this->item(rRow, TITLE)->setText(map[TITLE]);
    this->item(rRow, ARTIST)->setText(map[ARTIST]);
    this->item(rRow, ALBUM)->setText(map[ALBUM]);
    this->item(rRow, GENRE)->setText(map[GENRE]);
}

void BabeTable::infoIt_action()
{
    QString artist = this->model()->data(this->model()->index(rRow, ARTIST)).toString();
    QString album = this->model()->data(this->model()->index(rRow, ALBUM)).toString();
    QString title = this->model()->data(this->model()->index(rRow, TITLE)).toString();
    emit infoIt_clicked(title, artist, album);
}

void BabeTable::removeIt_action()
{
    qDebug() << "removeIt/right clicked!";
    qDebug() << this->model()->data(this->model()->index(rRow, LOCATION)).toString();
    this->removeRow(rRow);
    emit removeIt_clicked(rRow);
}

void BabeTable::moodIt_action(const QString &color)
{
    if (!color.isEmpty()) {
        QSqlQuery query;
        query.prepare("UPDATE tracks SET art = (:art) WHERE location = (:location)" );
        query.bindValue(":art", color);
        query.bindValue(":location", this->getRowData(rRow)[LOCATION]);
        if (query.exec()) {
            qDebug() << "Art[color] inserted into DB" << color;
            contextMenu->close();
            moodIt_clicked(rRow, color);
        } else {
            qDebug() << "COULDN'T insert art[color] into DB";
        }
    }
}

void BabeTable::colorizeRow(const int &row, const QString &color)
{
    QColor coloring;
    coloring.setNamedColor(color);
    coloring.setAlpha(40);
    this->item(row, TITLE)->setBackgroundColor(coloring);
}

void BabeTable::queueIt_action()
{
    qDebug() << "queueIt clicked!";
    QString url = this->model()->data(this->model()->index(rRow, LOCATION)).toString();
    qDebug() << url;
    emit queueIt_clicked(getRowData(rRow));
}

void BabeTable::flushTable()
{
    this->clearContents();
    this->setRowCount(0);
}

QStringList BabeTable::getTableColumnContent(const columns &column)
{
    QStringList result;
    for (int i = 0; i < this->rowCount(); i++)
        result << this->model()->data(this->model()->index(i, column)).toString();
    return result;
}

QList<QMap<int, QString>> BabeTable::getAllTableContent()
{
    QList<QMap<int, QString>> mapList;
    for (int i = 0; i < this->rowCount(); i++)
        mapList << getRowData(i);
    return mapList;
}

void BabeTable::removeRepeated() //tofix
{
    auto list = this->getTableColumnContent(BabeTable::LOCATION);
    list.removeDuplicates();
    this->flushTable();
    this->populateTableView(connection->getTrackData(list), true, true);
}
