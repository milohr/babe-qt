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
#include <QCheckBox>
#include "babealbum.h"
#include "../kde/notify.h"
#include "../dialogs/metadataform.h"
#include "../utils/trackloader.h"
#include "../db/collectionDB.h"
#include "../views/babewindow.h"
#include "../utils/trackloader.h"

using namespace BABETABLE;


BabeTable::BabeTable(QWidget *parent) : QTableWidget(parent)
{
    this->connection = new CollectionDB(this);
    this->trackLoader = new TrackLoader;

    this->preview = new QMediaPlayer(this);
    this->preview->setVolume(100);
    connect(trackLoader, &TrackLoader::finished, [this]()
    {
        this->setSortingEnabled(true);
        emit this->finishedPopulating();
    });

    connect(trackLoader, &TrackLoader::trackReady, [this](BAE::DB track)
    {
        this->insertTrack(track);
    });

    connect(this, &QTableWidget::doubleClicked, this, &BabeTable::on_tableWidget_doubleClicked);
    connect(this, &QTableWidget::itemSelectionChanged, [this](){this->stopPreview();});

    this->setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setColumnCount(BAE::TracksColsMap.count());

    this->setUpHeaders();

    this->verticalHeader()->setVisible(false);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setAlternatingRowColors(true);
    this->setSortingEnabled(true);
    this->setShowGrid(false);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

    this->setColumnWidth(static_cast<int>(BAE::KEY::TRACK), 20);
    this->setColumnWidth(static_cast<int>(BAE::KEY::PLAYED), 20);
    this->setColumnWidth(static_cast<int>(BAE::KEY::STARS), 80);

    this->hideColumn(static_cast<int>(BAE::KEY::URL));
    this->hideColumn(static_cast<int>(BAE::KEY::STARS));
    this->hideColumn(static_cast<int>(BAE::KEY::BABE));
    this->hideColumn(static_cast<int>(BAE::KEY::GENRE));
    this->hideColumn(static_cast<int>(BAE::KEY::TRACK));
    this->hideColumn(static_cast<int>(BAE::KEY::PLAYED));
    this->hideColumn(static_cast<int>(BAE::KEY::ART));
    this->hideColumn(static_cast<int>(BAE::KEY::RELEASE_DATE));
    this->hideColumn(static_cast<int>(BAE::KEY::ADD_DATE));
    this->hideColumn(static_cast<int>(BAE::KEY::SOURCES_URL));
    this->hideColumn(static_cast<int>(BAE::KEY::LYRICS));

    this->contextMenu = new QMenu(this);

    auto babeIt = new QAction("Babe it \xe2\x99\xa1", this->contextMenu);
    babeIt->setData(ACTION::BABEIT);
    contextMenu->addAction(babeIt);

    auto queueIt = new QAction(tr("Queue"), this->contextMenu);
    queueIt->setData(ACTION::QUEUEIT);
    contextMenu->addAction(queueIt);

    QAction *sendEntry = this->contextMenu->addAction(tr("Send to phone..."));
    sendEntry->setData(ACTION::SENDTO);
    sendToMenu = new QMenu(this->contextMenu);
    sendEntry->setMenu(sendToMenu);

    auto infoIt = new QAction(tr("Info"), this->contextMenu);
    infoIt->setData(ACTION::INFOIT);
    contextMenu->addAction(infoIt);

    auto editIt = new QAction(tr("Edit"), this->contextMenu);
    editIt->setData(ACTION::EDITIT);
    contextMenu->addAction(editIt);

    auto removeIt = new QAction(tr("Remove"), this->contextMenu);
    removeIt->setData(ACTION::REMOVEIT);
    contextMenu->addAction(removeIt);

    auto addTo = new QAction(tr("Add to..."), this->contextMenu);
    addTo->setData(ACTION::ADDTO);
    contextMenu->addAction(addTo);

    connect(this, &BabeTable::rightClicked, this, &BabeTable::setUpContextMenu);

    connect(sendToMenu, &QMenu::triggered, this, &BabeTable::sendIt_action);
    connect(babeIt, &QAction::triggered, this, &BabeTable::babeIt_action);
    connect(queueIt, &QAction::triggered, this, &BabeTable::queueIt_action);
    connect(addTo, &QAction::triggered, this, &BabeTable::addToPlaylist);
    connect(infoIt, &QAction::triggered, this, &BabeTable::infoIt_action);
    connect(editIt, &QAction::triggered, this, &BabeTable::editIt_action);
    connect(removeIt, &QAction::triggered, this, &BabeTable::removeIt_action);

    auto starsWidget = new QWidget(this->contextMenu);
    auto starsWidget_layout = new QHBoxLayout;
    starsWidget->setLayout(starsWidget_layout);

    this->stars = new QButtonGroup(contextMenu);
    connect(stars, SIGNAL(buttonClicked(int)), this, SLOT(rateGroup(int)));

    for(auto i = 0; i<5; i++)
    {
        auto star = new QToolButton(starsWidget);
        star->setAutoRaise(true);
        star->setMaximumSize(16, 16);
        star->setIcon(QIcon::fromTheme("rating-unrated"));
        stars->addButton(star, i+1);
        starsWidget_layout->addWidget(star);
    }

    QWidgetAction *starsAction = new QWidgetAction(contextMenu);
    starsAction->setData(ACTION::RATEIT);
    starsAction->setDefaultWidget(starsWidget);
    contextMenu->addAction(starsAction);

    auto moods = new QWidget(contextMenu);
    auto moodsLayout = new QHBoxLayout;

    auto *moodGroup = new QButtonGroup(this->contextMenu);
    connect(moodGroup, SIGNAL(buttonClicked(int)), this, SLOT(moodTrack(int)));

    for(auto i =  0; i < BAE::MoodColors.size(); i++)
    {
        auto colorTag = new QToolButton(moods);
        //colorTag->setIconSize(QSize(10,10));
        colorTag->setFixedSize(15,15);
        // colorTag->setAutoRaise(true);
        colorTag->setStyleSheet(QString("QToolButton { background-color: %1;}").arg(BAE::MoodColors[i]));
        moodGroup->addButton(colorTag,i);
        moodsLayout->addWidget(colorTag);
    }
    moods->setLayout(moodsLayout);

    auto *moodsAction = new QWidgetAction(this->contextMenu);
    moodsAction->setData(ACTION::MOODIT);
    moodsAction->setDefaultWidget(moods);
    this->contextMenu->addAction(moodsAction);

    QFont helvetica("Helvetica", 10);

    this->addMusicMsgWidget = new QWidget(this);
    this->addMusicMsgWidget->setVisible(false);
    auto addMusicMsg_layout = new QVBoxLayout;
    this->addMusicMsgWidget->setLayout(addMusicMsg_layout);

    this->addMusicTxt = new QLabel(this->addMusicMsgWidget);
    this->addMusicTxt->setObjectName("addMusicTxt");
    this->addMusicTxt->setStyleSheet("QLabel{background-color:transparent;}");
    this->addMusicTxt->setText(this->addMusicMsg);
    this->addMusicTxt->setFont(helvetica);
    this->addMusicTxt->setWordWrap(true);
    this->addMusicTxt->setAlignment(Qt::AlignCenter);

    auto addMusicTxt_effect = new QGraphicsOpacityEffect(this->addMusicTxt);
    addMusicTxt_effect->setOpacity(0.5);
    this->addMusicTxt->setGraphicsEffect(addMusicTxt_effect);
    this->addMusicTxt->setAutoFillBackground(true);

    this->addMusicImg = new QLabel(this->addMusicMsgWidget);
    this->addMusicImg->setAlignment(Qt::AlignCenter);
    this->addMusicImg->setPixmap(QIcon::fromTheme(this->addMusicIcon).pixmap(48));
    this->addMusicImg->setEnabled(false);

    auto addMusicImg_effect= new QGraphicsOpacityEffect(this->addMusicImg);
    addMusicImg_effect->setOpacity(0.5);
    this->addMusicImg->setGraphicsEffect(addMusicImg_effect);

    addMusicMsg_layout->addStretch();
    addMusicMsg_layout->addWidget(this->addMusicImg);
    addMusicMsg_layout->addWidget(this->addMusicTxt);
    addMusicMsg_layout->addStretch();

    auto addMusicTxt_layout = new QHBoxLayout(this);
    addMusicTxt_layout->addStretch();
    addMusicTxt_layout->addWidget(addMusicMsgWidget,Qt::AlignCenter); // center alignment
    addMusicTxt_layout->addStretch();

    auto updater = new QTimer(this);
    connect(updater, SIGNAL(timeout()), this, SLOT(update()));
    updater->start(200);
}

BabeTable::~BabeTable()
{
    qDebug()<<"DELETING BABETABLE";
    delete this->trackLoader;
}

void BabeTable::setUpHeaders()
{
    this->horizontalHeader()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(this->horizontalHeader(), &QHeaderView::customContextMenuRequested, [this](QPoint pos)
    {
        Q_UNUSED(pos);
        for(auto action : this->headerMenu->actions())
            action->setChecked(this->horizontalHeader()->isSectionHidden(action->data().toInt()) ? false : true);

        this->headerMenu->exec(QCursor::pos());
    });

    this->headerMenu = new QMenu(this);
    connect(this->headerMenu, &QMenu::triggered, [this](QAction *action)
    {
        auto key = action->data().toInt();
        if(action->isChecked())
        {
            this->showColumn(key);
        }else this->hideColumn(key);
    });

    QStringList cols;
    for(auto key : BAE::TracksColsMap.keys())
    {
        auto col = BAE::TracksColsMap[key];
        auto colTitle = QString(col.left(1).toUpper()+col.mid(1));
        cols << colTitle;
        auto action = new QAction(colTitle);
        action->setData(static_cast<int>(key));
        action->setCheckable(true);
        this->headerMenu->addAction(action);
    }

    this->setHorizontalHeaderLabels(cols);
    this->horizontalHeader()->setSectionsMovable(true);
    this->horizontalHeader()->setDefaultSectionSize(150);

    this->horizontalHeader()->setHighlightSections(false);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    this->horizontalHeader()->setStretchLastSection(true);
}

void BabeTable::dropEvent(QDropEvent *event)
{
    if(event->source() == this && !event->isAccepted() && rowDragging)
    {
        auto newRow = this->indexAt(event->pos()).row();
        auto insertionRow = newRow;

        qDebug()<<"new row position"<< newRow;
        auto list = this->getSelectedRows(false);

        BAE::DB_LIST tracks;
        QList<int> newList;

        for(auto track : list)
        {
            if(this->getItem(track,BAE::KEY::TITLE)->icon().name() != "media-playback-start")
            {
                tracks<<this->getRowData(track);
                newList << track;
            }
        }

        if(!newList.isEmpty())
        {
            auto i = 0;
            auto j = 0;

            std::sort(newList.begin(), newList.end());

            for(auto track:newList)
            {
                if(track >= newRow)
                {
                    this->removeRow(track-i-j);
                    i++;

                }else
                {
                    this->removeRow(track-j);
                    j++;
                    newRow--;
                }
            }

            i = 0;

            for(auto track : tracks)
            {
                this->addRowAt(newRow+i+1, track);
                qDebug()<<"indexes moved "<< newList.at(i) << insertionRow<<newRow;
                emit this->indexesMoved(newList.at(i), insertionRow);
                i++;
            }
        }
    }
}

void BabeTable::setAddMusicMsg(const QString &msg,const QString &icon)
{
    this->addMusicMsg = msg;
    this->addMusicTxt->setText(this->addMusicMsg);
    this->addMusicIcon = icon;
    this->addMusicImg->setPixmap(QIcon::fromTheme(this->addMusicIcon).pixmap(48));
}

void BabeTable::update()
{
    if(this->rowCount()!=0)
    {
        if(this->addMusicMsgWidget->isVisible())
            this->addMusicMsgWidget->setVisible(false);

    }else this->addMusicMsgWidget->setVisible(true);

    if(this->preview->state() == QMediaPlayer::StoppedState && this->previewRow != -1)
    {
        this->getItem(this->previewRow, BAE::KEY::TITLE)->setIcon(QIcon::fromTheme(""));
        emit this->previewFinished();
        this->previewRow = -1;
    }
}

void BabeTable::moodTrack(int color) { this->moodIt_action(BAE::MoodColors[color]); }

void BabeTable::addToPlaylist()
{    
    BAE::DB_LIST mapList;
    for(auto row : this->getSelectedRows())
        mapList<< this->getRowData(row);

    emit this->saveToPlaylist(mapList);
}


void BabeTable::enterEvent(QEvent *event)
{
    emit this->enterTable();
    event->accept();
    //    QTableWidget::enterEvent(event);
}

void BabeTable::leaveEvent(QEvent *event)
{
    this->stopPreview();
    emit this->leaveTable();
    event->accept();
    //    QTableWidget::leaveEvent(event);
}

void BabeTable::passStyle(const QString &style) { this->setStyleSheet(style); }

int BabeTable::getIndex() { return this->currentIndex().row(); }

QTableWidgetItem *BabeTable::getItem(const int &row, const BAE::KEY &column)
{
    return this->item(row,static_cast<int>(column));
}

void BabeTable::putItem(const int &row, const BAE::KEY &col, QTableWidgetItem *item)
{
    this->setItem(row, static_cast<int>(col), item);
}

void BabeTable::enableRowColoring(const bool &state)
{
    this->rowColoring=state;
}

void BabeTable::enableRowDragging(const bool &state)
{
    this->rowDragging = state;

    if(rowDragging)
    {
        this->setDragEnabled(true);
        this->setAcceptDrops(true);
        this->viewport()->setAcceptDrops(true);
        this->setDragDropOverwriteMode(true);
        this->setDropIndicatorShown(true);
        this->setDragDropMode(QAbstractItemView::InternalMove);
    }
}

void BabeTable::addMenuItem(QAction *item)
{
    item->setParent(this->contextMenu);
    this->contextMenu->addAction(item);
}

void BabeTable::addRow(const BAE::DB &map)
{
    this->insertRow(this->rowCount());
    this->putItem(this->rowCount() - 1,BAE::KEY::TRACK, new QTableWidgetItem(map[BAE::KEY::TRACK]));
    this->putItem(this->rowCount() - 1,BAE::KEY::TITLE, new QTableWidgetItem(map[BAE::KEY::TITLE]));
    this->putItem(this->rowCount() - 1,BAE::KEY::ARTIST, new QTableWidgetItem(map[BAE::KEY::ARTIST]));
    this->putItem(this->rowCount() - 1,BAE::KEY::ALBUM, new QTableWidgetItem(map[BAE::KEY::ALBUM]));
    this->putItem(this->rowCount() - 1,BAE::KEY::DURATION, new QTableWidgetItem(BAE::transformTime(map[BAE::KEY::DURATION].toInt())));
    this->putItem(this->rowCount() - 1,BAE::KEY::GENRE, new QTableWidgetItem(map[BAE::KEY::GENRE]));
    this->putItem(this->rowCount() - 1,BAE::KEY::URL, new QTableWidgetItem(map[BAE::KEY::URL]));
    this->putItem(this->rowCount() - 1,BAE::KEY::STARS, new QTableWidgetItem(this->getStars(map[BAE::KEY::STARS].toInt())));
    this->putItem(this->rowCount() - 1,BAE::KEY::BABE, new QTableWidgetItem(this->getHearts(map[BAE::KEY::BABE].toInt())));
    //    this->setItem(this->rowCount() - 1, BAE::KEY::ART, new QTableWidgetItem(map[BAE::KEY::ART]));
    this->putItem(this->rowCount() - 1,BAE::KEY::PLAYED, new QTableWidgetItem(map[BAE::KEY::PLAYED]));
    this->putItem(this->rowCount() - 1,BAE::KEY::RELEASE_DATE, new QTableWidgetItem(map[BAE::KEY::RELEASE_DATE]));
    this->putItem(this->rowCount() - 1,BAE::KEY::ADD_DATE, new QTableWidgetItem(map[BAE::KEY::ADD_DATE]));

    if(this->rowColoring && !map[BAE::KEY::ART].isEmpty())
        this->colorizeRow({this->rowCount()-1},map[BAE::KEY::ART]);

    //    if(descriptiveTooltip)
    //        this->item(this->rowCount()-1,BAE::KEY::TITLE)->setToolTip( "by "+map[BAE::KEY::ARTIST]);
}

void BabeTable::addRowAt(const int &row, const BAE::DB &map)
{
    this->insertRow(row);
    this->putItem(row,BAE::KEY::URL, new QTableWidgetItem(map[BAE::KEY::URL]));
    this->putItem(row ,BAE::KEY::TITLE, new QTableWidgetItem(map[BAE::KEY::TITLE]));

    this->putItem(row,BAE::KEY::TRACK, new QTableWidgetItem(map[BAE::KEY::TRACK]));
    this->putItem(row,BAE::KEY::ARTIST, new QTableWidgetItem(map[BAE::KEY::ARTIST]));
    this->putItem(row ,BAE::KEY::ALBUM, new QTableWidgetItem(map[BAE::KEY::ALBUM]));

    this->putItem(row,BAE::KEY::DURATION, new QTableWidgetItem(BAE::transformTime(map[BAE::KEY::DURATION].toInt())));
    this->putItem(row,BAE::KEY::GENRE, new QTableWidgetItem(map[BAE::KEY::GENRE]));
    this->putItem(row,BAE::KEY::STARS, new QTableWidgetItem(this->getStars(map[BAE::KEY::STARS].toInt())));

    this->putItem(row ,BAE::KEY::BABE, new QTableWidgetItem(this->getHearts(map[BAE::KEY::BABE].toInt())));
    //    this->setItem(row , BAE::KEY::ART, new QTableWidgetItem(map[BAE::KEY::ART]));
    this->putItem(row,BAE::KEY::PLAYED, new QTableWidgetItem(map[BAE::KEY::PLAYED]));
    this->putItem(row,BAE::KEY::RELEASE_DATE, new QTableWidgetItem(map[BAE::KEY::RELEASE_DATE]));
    this->putItem(row,BAE::KEY::ADD_DATE, new QTableWidgetItem(map[BAE::KEY::ADD_DATE]));
    this->putItem(row,BAE::KEY::SOURCES_URL, new QTableWidgetItem(map[BAE::KEY::SOURCES_URL]));
    this->putItem(row,BAE::KEY::LYRICS, new QTableWidgetItem(map[BAE::KEY::LYRICS]));

    if(this->rowColoring && !map[BAE::KEY::ART].isEmpty())
        this->colorizeRow({row},map[BAE::KEY::ART]);


    //    if(descriptiveTooltip)
    //        this->item(row,BAE::KEY::TITLE)->setToolTip( "by "+map[BAE::KEY::ARTIST]);
}

void BabeTable::removeMissing(const QString &url)
{
    BabeWindow::BabeWindow::nof->notify("Removing missing file...",url);

    auto parentDir = QFileInfo(QFileInfo(url)).dir().path();

    if (!BAE::fileExists(parentDir)) this->connection->removeSource(parentDir);
    else this->connection->removeTrack(url);

}


void BabeTable::insertTrack(const BAE::DB &track)
{
    auto location =track[BAE::KEY::URL];

    if (!BAE::fileExists(location))  removeMissing(location);
    else addRow(track);
}

void BabeTable::populateTableView(const BAE::DB_LIST &mapList)
{
    this->setSortingEnabled(false);
    if(!mapList.isEmpty())
    {
        for(auto trackMap : mapList) insertTrack(trackMap);
        this->setSortingEnabled(true);
        emit finishedPopulating();
    }
}

void BabeTable::populateTableView(const QString &query)
{
    qDebug() << "ON POPULATE TABLEVIEW";
    this->setSortingEnabled(false);
    this->trackLoader->requestTracks(query);
}


QString BabeTable::getStars(const int &value)
{
    switch (value) {
        case 0:
            return  " ";

        case 1:
            return  "\xe2\x98\x86 ";

        case 2:
            return "\xe2\x98\x86 \xe2\x98\x86 ";

        case 3:
            return  "\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 ";

        case 4:
            return  "\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 ";

        case 5:
            return "\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 ";

        default: return "error";
    }

}

void BabeTable::setRating(const int &rate)
{
    for(int i =1; i<=rate; i++)
        this->stars->button(i)->setIcon(QIcon::fromTheme("rating-unrated"));

    for(int i =1; i<=rate; i++)
        this->stars->button(i)->setIcon(QIcon::fromTheme("rating"));

}

void BabeTable::setTableOrder(int column, BAE::W order)
{
    if (order == BAE::W::DESC) {
        this->sortByColumn(column, Qt::DescendingOrder);
    } else if (order == BAE::W ::ASC) {
        this->sortByColumn(column, Qt::AscendingOrder);
    }
}

void BabeTable::setVisibleColumn(const BAE::KEY &column)
{
    if (column == BAE::KEY::URL)
        this->showColumn(static_cast<int>(BAE::KEY::URL));
    else if (column == BAE::KEY::STARS)
        this->showColumn(static_cast<int>(BAE::KEY::STARS));
    else if (column == BAE::KEY::BABE)
        this->showColumn(static_cast<int>(BAE::KEY::BABE));
    else if (column == BAE::KEY::ALBUM)
        this->showColumn(static_cast<int>(BAE::KEY::ALBUM));
}


QMap<QString,QString> BabeTable::getKdeConnectDevices()
{
    qDebug()<<"getting the kdeconnect devices avaliable";
    QMap<QString,QString> _devices;
    QProcess process;
    process.start("kdeconnect-cli -a");
    process.waitForFinished();
    // auto output = process->readAllStandardOutput();

    process.setReadChannel(QProcess::StandardOutput);

    while (process.canReadLine())
    {
        QString line = QString::fromLocal8Bit(process.readLine());
        qDebug()<<"line:"<<line;
        if(line.contains("(paired and reachable)"))
        {
            QStringList items = line.split(" ");
            auto key=QString(items.at(2));
            auto name=QString(items.at(1)).replace(":","");

            qDebug()<<"Founded devices: "<<key<<":"<<name;
            _devices.insert(key,name);
        }
    }

    this->devices=_devices;
    return devices;

}

void BabeTable::setUpContextMenu(const int row, const int column)
{
    qDebug() << "setUpContextMenu";
    //contextMenu->exec(QCursor::pos());

    this->rRow = row;
    this->rColumn= column;

    this->sendToMenu->clear();
    QMapIterator<QString, QString> i(getKdeConnectDevices());
    while (i.hasNext())
    {
        i.next();
        this->sendToMenu->addAction(i.value());
    }

    auto rowData = this->getRowData(rRow);
    if(!rowData.isEmpty())
    {
        auto url = rowData[BAE::KEY::URL];
        this->setRating(this->connection->getTrackStars(url));

        if (this->connection->getTrackBabe(url))
            this->contextMenu->actions().at(ACTION::BABEIT)->setText("Un-Babe it");
        else
            this->contextMenu->actions().at(ACTION::BABEIT)->setText("Babe it");

        this->contextMenu->exec(QCursor::pos());
    }
}

void BabeTable::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Return:
        {
            BAE::DB_LIST list;
            for(auto row : this->getSelectedRows(false))
            {
                list<<getRowData(row);

                qDebug()<<row;
            }

            this->stopPreview();
            emit tableWidget_doubleClicked(list);
            break;
        }

        case Qt::Key_Up:
        {
            QModelIndex index = this->currentIndex();
            int row = index.row() - 1;
            int column = 1;
            QModelIndex newIndex = this->model()->index(row, column);
            this->selectionModel()->select(newIndex, QItemSelectionModel::Select);
            this->setCurrentIndex(newIndex);
            this->setFocus();

            break;
        }

        case Qt::Key_Down:
        {
            QModelIndex index = this->currentIndex();
            int row = index.row() + 1;
            int column = 1;
            QModelIndex newIndex = this->model()->index(row, column);
            this->selectionModel()->select(newIndex, QItemSelectionModel::Select);
            this->setCurrentIndex(newIndex);
            this->setFocus();

            break;
        }

        case Qt::Key_Space:
        {
            if(this->rowPreview)
            {
                if(preview->state()==QMediaPlayer::PlayingState) this->stopPreview();
                else
                {
                    this->previewRow= this->getIndex();
                    this->startPreview(this->getRowData(previewRow)[BAE::KEY::URL]);
                }
            }
            break;
        }

        case Qt::Key_Delete	:
        {
            auto rows = this->getSelectedRows(false);
            auto i = 0;
            for(auto row:rows)
            {
                this->removeRow(row-i);
                emit indexRemoved(row-i);
                i++;
            }
            break;
        }

        case Qt::Key_Right:
        {
            if(preview->state()==QMediaPlayer::PlayingState)
            {
                auto newPos = preview->position()+1000;
                if(newPos>=preview->duration())
                    this->stopPreview();
                else
                    preview->setPosition(newPos);
            }
            break;
        }

        case Qt::Key_Q :
        {
            BAE::DB_LIST mapList;
            for(auto row : this->getSelectedRows(false))
                mapList<< this->getRowData(row);
            stopPreview();

            emit queueIt_clicked(mapList);
            break;
        }

        case Qt::Key_P :
        {
            BAE::DB_LIST mapList;
            for(auto row : this->getSelectedRows(false))
                mapList<< this->getRowData(row);

            stopPreview();

            emit playItNow(mapList);
            break;
        }

        case Qt::Key_L :
        {
            BAE::DB_LIST mapList;
            for(auto row : this->getSelectedRows(false))
                mapList<< this->getRowData(row);

            stopPreview();
            emit appendIt(mapList);
            break;
        }

        case Qt::Key_S :
        {
            BAE::DB_LIST mapList;
            for(auto row : this->getSelectedRows(false))
                mapList<< this->getRowData(row);

            stopPreview();
            emit saveToPlaylist(mapList);
            break;
        }

        case Qt::Key_I:
        {
            auto url =  this->getRowData(this->getIndex())[BAE::KEY::URL];
            emit infoIt_clicked(this->connection->getDBData(QStringList(url)).first());
            break;
        }

        case Qt::Key_0:
        {
            this->rateGroup(0,false);
            break;
        }
        case Qt::Key_1:
        {
            this->rateGroup(1,false);
            break;
        }
        case Qt::Key_2:
        {
            this->rateGroup(2,false);
            break;
        }
        case Qt::Key_3:
        {
            this->rateGroup(3,false);
            break;
        }
        case Qt::Key_4:
        {
            this->rateGroup(4,false);
            break;
        }
        case Qt::Key_5:
        {
            this->rateGroup(5,false);
            break;
        }
        default:
        {
            QTableWidget::keyPressEvent(event);
            break;
        }

    }
}

void BabeTable::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::RightButton)
    {
        evt->accept();
        int row = this->indexAt(evt->pos()).row();
        int column= this->indexAt(evt->pos()).column();
        if(row != -1) emit rightClicked(row, column);
    }

    QTableWidget::mousePressEvent(evt);
}

void BabeTable::enablePreview(const bool state)
{
    this->rowPreview = state;
}

void BabeTable::startPreview(const QString &url)
{
    preview->setMedia(QUrl::fromLocalFile(url));
    preview->play();
    this->getItem(this->getIndex(),BAE::KEY::TITLE)->setIcon(QIcon::fromTheme("quickview"));
    emit previewStarted();
}

void BabeTable::stopPreview()
{
    if(preview->state()==QMediaPlayer::PlayingState)
    {
        preview->stop();
        this->getItem(this->previewRow,BAE::KEY::TITLE)->setIcon(QIcon::fromTheme(""));
        emit previewFinished();
        previewRow=-1;
    }
}

QString BabeTable::getHearts(const int &value)
{
    QString babe;
    switch (value) {
        case 0:
            babe = " ";
            break;
        case 1:
            babe = "\xe2\x99\xa1 ";
            break;
    }

    return babe;
}


void BabeTable::rateGroup(const int &id, const bool &rightClick)
{
    qDebug() << "rated with: " << id;

    for(auto row : this->getSelectedRows(rightClick))
    {
        QString location = this->getRowData(row)[BAE::KEY::URL];

        if (this->connection->rateTrack(location,id))
        {
            setRating(id);

            QString stars;
            for (int i = 0; i < id; i++)
                stars += "\xe2\x98\x86 ";

            this->getItem(row,BAE::KEY::STARS)->setText(stars);

        } else qDebug() << "rating failed for"<< location;
    }
}

BAE::DB BabeTable::getRowData(const int &row)
{
    QString url = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::URL))).toString();

    if(this->connection->check_existance(BAE::TABLEMAP[BAE::TABLE::TRACKS],BAE::TracksColsMap[BAE::KEY::URL],url))
    {
        auto data = this->connection->getDBData(QStringList(url));
        return data.size() > 0 ? data.first() : BAE::DB();
    }else
    {
        QString track = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::TRACK))).toString();
        QString title = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::TITLE))).toString();
        QString artist = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::ARTIST))).toString();
        QString album = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::ALBUM))).toString();
        QString duration = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::DURATION))).toString();
        QString genre = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::GENRE))).toString();
        QString location = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::URL))).toString();
        QString stars = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::STARS))).toString();
        QString babe = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::BABE))).toString();
        QString releaseDate = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::RELEASE_DATE))).toString();
        QString addDate = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::ADD_DATE))).toString();
        QString played = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::PLAYED))).toString();
        QString art = this->model()->data(this->model()->index(row,static_cast<int>(BAE::KEY::ART))).toString();

        return BAE::DB
        {
            {BAE::KEY::TRACK,track},
            {BAE::KEY::TITLE,title},
            {BAE::KEY::ARTIST,artist},
            {BAE::KEY::ALBUM,album},
            {BAE::KEY::DURATION,duration},
            {BAE::KEY::GENRE,genre},
            {BAE::KEY::URL,location},
            {BAE::KEY::STARS,stars},
            {BAE::KEY::BABE,babe},
            {BAE::KEY::ART,art},
            {BAE::KEY::PLAYED,played},
            {BAE::KEY::RELEASE_DATE,releaseDate},
            {BAE::KEY::ADD_DATE,addDate}
        };

    }
}


void BabeTable::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    BAE::DB_LIST list;
    auto track = getRowData(this->getIndex());
    list<<track;
    emit tableWidget_doubleClicked(list);
}

void BabeTable::babeIt_action()
{
    BAE::DB_LIST mapList;
    for(auto row: this->getSelectedRows())
        mapList<< this->getRowData(row);

    emit babeIt_clicked(mapList);
}

void BabeTable::sendIt_action(QAction *device)
{
    for(auto row: this->getSelectedRows())
    {

        auto track = this->getRowData(row);

        QString url =track[BAE::KEY::URL];
        QString title = track[BAE::KEY::TITLE];
        QString artist = track[BAE::KEY::ARTIST];

        QString deviceName = device->text().replace("&","");
        QString deviceKey = devices.key(deviceName);

        qDebug()<<"trying to send "<< url << "to : "<< deviceName;
        auto process = new QProcess(this);
        connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            qDebug()<<"processFinished_totally"<<exitCode<<exitStatus;
            BabeWindow::nof->notify("Song sent to " + deviceName,title +" by "+ artist);

        });

        qDebug()<<"kdeconnect-cli -d "  +deviceKey+ " --share " + url;
        process->start("kdeconnect-cli -d " +deviceKey+ " --share " +"\""+ url+"\"");
    }
}

void BabeTable::editIt_action()
{
    auto infoForm = new metadataForm(getRowData(rRow), this);
    connect(infoForm, &metadataForm::infoModified, [=](BAE::DB track)
    {
        this->itemEdited(track);
        infoForm->deleteLater();
    });

    infoForm->show();
}

void BabeTable::itemEdited(const BAE::DB &map)
{
    qDebug()<<"item changed: " << map[BAE::KEY::TITLE];
    this->getItem(rRow,BAE::KEY::TRACK)->setText(map[BAE::KEY::TRACK]);
    this->getItem(rRow,BAE::KEY::TITLE)->setText(map[BAE::KEY::TITLE]);
    this->getItem(rRow,BAE::KEY::ARTIST)->setText(map[BAE::KEY::ARTIST]);
    this->getItem(rRow,BAE::KEY::ALBUM)->setText(map[BAE::KEY::ALBUM]);
    this->getItem(rRow,BAE::KEY::GENRE)->setText(map[BAE::KEY::GENRE]);

    auto updatedTrack = map;
    updatedTrack.remove(KEY::LYRICS);
    updatedTrack.remove(KEY::WIKI);
    this->connection->updateTrack(updatedTrack);
}

void BabeTable::infoIt_action()
{
    emit infoIt_clicked(this->getRowData(rRow));
}

void BabeTable::removeIt_action()
{
    auto i = 0;
    for(auto row : this->getSelectedRows(true))
    {
        this->removeRow(row-i);
        emit this->indexRemoved(row-i);
        i++;
    }
}

void BabeTable::moodIt_action(const QString &color)
{
    if(!color.isEmpty())
    {
        for(auto row : this->getSelectedRows())
        {
            auto url =this->getRowData(row)[BAE::KEY::URL];
            if(this->connection->artTrack(url,color)) qDebug()<< "moodIt was sucessful";
            else qDebug()<<"could not mood track: "<< url;
        }
        contextMenu->close();
        emit moodIt_clicked(this->getSelectedRows(),color,false);    }
}

void BabeTable::colorizeRow(const QList<int> &rows, const QString &color, const bool &dark)
{
    for(auto row : rows)
    {
        QColor coloring;
        coloring.setNamedColor(color);
        if(!dark) coloring.setAlpha(60);
        this->getItem(row,BAE::KEY::TITLE)->setBackgroundColor(coloring);
        if(dark)
        {
            QBrush brush;
            brush.setColor("#fff");
            this->getItem(row,BAE::KEY::TITLE)->setForeground(brush);
        }
        //this->item(row,TITLE)->setTextAlignment(Qt::AlignCenter);
    }
}

void BabeTable::queueIt_action()
{

    BAE::DB_LIST mapList;
    for(auto row : this->getSelectedRows())
        mapList<< this->getRowData(row);

    emit queueIt_clicked(mapList);
}

QList<int> BabeTable::getSelectedRows(const bool &onRightClick)
{
    QList<int> selectedRows;
    auto selection = this->selectionModel()->selectedRows(static_cast<int>(BAE::KEY::TITLE));
    QList<QMap<int, QString>> list;

    for(auto model : selection) selectedRows<<model.row();

    if(!selectedRows.contains(rRow) && onRightClick) return {rRow};
    else return selectedRows;

}

void BabeTable::flushTable()
{
    qDebug()<<"ORDER TO CLEAN TABLE";
    this->clearContents();
    this->setRowCount(0);
}

QStringList BabeTable::getTableColumnContent(const BAE::KEY &column)
{
    QStringList result;

    if(static_cast<int>(column) <= this->columnCount())
    {
        for (int i = 0; i < this->rowCount(); i++)
            result << this->getRowData(i)[column];
    }
    return result;
}


BAE::DB_LIST BabeTable::getAllTableContent()
{
    BAE::DB_LIST mapList;

    for (int i = 0; i<this->rowCount(); i++)
        mapList<<getRowData(i);

    return mapList;
}

void BabeTable::removeRepeated()//tofix
{
    QStringList index;
    //    int rows =;
    for(auto row=0;row<this->rowCount();row++)
    {
        auto track = this->getRowData(row);
        auto trackInfo = track[BAE::KEY::TITLE]+"/&/"+track[BAE::KEY::ARTIST]+"/&/"+track[BAE::KEY::ALBUM];

        if(index.contains(trackInfo) && this->getItem(row,BAE::KEY::TITLE)->icon().name().isEmpty())
        {
            qDebug()<<"index contains:"<<track[BAE::KEY::TITLE]<<row;;
            this->removeRow(row);
            emit indexRemoved(row);
            row--;
        }else
        {
            qDebug()<<"adding to index:"<<track[BAE::KEY::TITLE]<<row;
            index<<trackInfo;
        }
    }
}
