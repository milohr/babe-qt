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


BabeTable::BabeTable(QWidget *parent) : QTableWidget(parent)
{
    preview = new QMediaPlayer(this);
    preview->setVolume(100);
    this->nof = new Notify(this);
    connect(&trackLoader,&TrackLoader::finished,[this]()
    {
        this->setSortingEnabled(true);
        emit finishedPopulating();
    });
    connect(&trackLoader,&TrackLoader::trackReady,[this](Bae::DB track)
    {
        this->insertTrack(track);
    });

    connect(this,&QTableWidget::doubleClicked, this, &BabeTable::on_tableWidget_doubleClicked);
    connect(this,&QTableWidget::itemSelectionChanged,[this](){this->stopPreview();});
    //    connect(this->selectionModel(),&QItemSelectionModel::selectionChanged,[this](){    this->setSelectionMode(QAbstractItemView::SingleSelection);
    //qDebug()<<"a row got selected;";});
    //connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),SLOT(setUpContextMenu(const QPoint&)));
    //connect(this->model(),SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),this,SLOT(itemEdited(const QModelIndex&, const QModelIndex&)));
    //this->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    this->setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setColumnCount(Bae::TracksColsMap.count());
    QStringList cols;
    for(auto col:Bae::TracksColsMap.values())  cols<< col.left(1).toUpper()+col.mid(1);;
    this->setHorizontalHeaderLabels(cols);
    this->horizontalHeader()->setSectionsMovable(true);

    this->horizontalHeader()->setDefaultSectionSize(150);
    //    this->setMinimumSize(0, 0);
    this->verticalHeader()->setVisible(false);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setAlternatingRowColors(true);
    this->setSortingEnabled(true);

    this->horizontalHeader()->setHighlightSections(false);

    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    this->horizontalHeader()->setStretchLastSection(true);

    this->setShowGrid(false);


    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

    this->setColumnWidth(static_cast<int>(Bae::DBCols::TRACK), 20);
    this->setColumnWidth(static_cast<int>(Bae::DBCols::PLAYED), 20);
    this->setColumnWidth(static_cast<int>(Bae::DBCols::STARS), 80);
    this->hideColumn(static_cast<int>(Bae::DBCols::URL));
    this->hideColumn(static_cast<int>(Bae::DBCols::STARS));
    this->hideColumn(static_cast<int>(Bae::DBCols::BABE));
    this->hideColumn(static_cast<int>(Bae::DBCols::GENRE));
    this->hideColumn(static_cast<int>(Bae::DBCols::TRACK));
    this->hideColumn(static_cast<int>(Bae::DBCols::PLAYED));
    this->hideColumn(static_cast<int>(Bae::DBCols::ART));
    this->hideColumn(static_cast<int>(Bae::DBCols::RELEASE_DATE));
    this->hideColumn(static_cast<int>(Bae::DBCols::ADD_DATE));
    this->hideColumn(static_cast<int>(Bae::DBCols::SOURCES_URL));
    this->hideColumn(static_cast<int>(Bae::DBCols::LYRICS));


    contextMenu = new QMenu(this);

    // this->horizontalHeaderItem(0);
    // this->horizontalHeader()->setHighlightSections(true);

    auto babeIt = new QAction("Babe it \xe2\x99\xa1", contextMenu);
    contextMenu->addAction(babeIt);

    auto queueIt = new QAction("Queue", contextMenu);
    contextMenu->addAction(queueIt);

    QAction *sendEntry = contextMenu->addAction("Send to phone...");
    sendToMenu = new QMenu(contextMenu);
    sendEntry->setMenu(sendToMenu);

    auto infoIt = new QAction("Info + ", contextMenu);
    contextMenu->addAction(infoIt);

    auto editIt = new QAction("Edit", contextMenu);
    contextMenu->addAction(editIt);

    auto saveIt = new QAction("Save to... ", contextMenu);
    contextMenu->addAction(saveIt);

    auto removeIt = new QAction("Remove", contextMenu);
    contextMenu->addAction(removeIt);

    auto addTo = new QAction("Add to...", contextMenu);
    contextMenu->addAction(addTo);


    connect(this, &BabeTable::rightClicked, this,&BabeTable::setUpContextMenu);
    connect(sendToMenu,&QMenu::triggered, this,&BabeTable::sendIt_action);

    connect(babeIt,&QAction::triggered, this, &BabeTable::babeIt_action);
    connect(queueIt,&QAction::triggered, this,&BabeTable::queueIt_action);
    connect(addTo,&QAction::triggered, this,&BabeTable::addToPlaylist);
    connect(infoIt, &QAction::triggered, this, &BabeTable::infoIt_action);
    connect(editIt,&QAction::triggered, this, &BabeTable::editIt_action);
    connect(removeIt,&QAction::triggered, this, &BabeTable::removeIt_action);


    auto starsWidget = new QWidget(contextMenu);
    auto starsWidget_layout = new QHBoxLayout;
    starsWidget->setLayout(starsWidget_layout);

    this->stars = new QButtonGroup(contextMenu);
    connect(stars, SIGNAL(buttonClicked(int)), this, SLOT(rateGroup(int)));

    for(int i = 0; i<5; i++)
    {
        auto star = new QToolButton(starsWidget);
        star->setAutoRaise(true);
        star->setMaximumSize(16, 16);
        star->setIcon(QIcon::fromTheme("rating-unrated"));
        stars->addButton(star, i+1);
        starsWidget_layout->addWidget(star);
    }

    QWidgetAction *starsAction = new QWidgetAction(contextMenu);
    starsAction->setDefaultWidget(starsWidget);
    contextMenu->addAction(starsAction);

    auto moods = new QWidget(contextMenu);
    auto moodsLayout = new QHBoxLayout;
    QButtonGroup *moodGroup = new QButtonGroup(contextMenu);
    connect(moodGroup, SIGNAL(buttonClicked(int)), this, SLOT(moodTrack(int)));
    for(int i=0; i<5; i++)
    {
        auto colorTag = new QToolButton(moods);
        //colorTag->setIconSize(QSize(10,10));
        colorTag->setFixedSize(15,15);
        // colorTag->setAutoRaise(true);
        colorTag->setStyleSheet(QString("QToolButton { background-color: %1;}").arg(colors.at(i)));
        moodGroup->addButton(colorTag,i);
        moodsLayout->addWidget(colorTag);
    }
    moods->setLayout(moodsLayout);

    QWidgetAction *moodsAction = new QWidgetAction(contextMenu);
    moodsAction->setDefaultWidget(moods);
    contextMenu->addAction(moodsAction);

    QFont helvetica("Helvetica", 10);

    addMusicMsgWidget = new QWidget(this);
    addMusicMsgWidget->setVisible(false);
    auto addMusicMsg_layout = new QVBoxLayout;
    addMusicMsgWidget->setLayout(addMusicMsg_layout);

    addMusicTxt = new QLabel(addMusicMsgWidget);
    addMusicTxt->setObjectName("addMusicTxt");
    addMusicTxt->setStyleSheet("QLabel{background-color:transparent;}");
    addMusicTxt->setText(this->addMusicMsg);
    addMusicTxt->setFont(helvetica);
    addMusicTxt->setWordWrap(true);
    addMusicTxt->setAlignment(Qt::AlignCenter);

    auto addMusicTxt_effect = new QGraphicsOpacityEffect(addMusicTxt);
    addMusicTxt_effect->setOpacity(0.5);
    addMusicTxt->setGraphicsEffect(addMusicTxt_effect);
    addMusicTxt->setAutoFillBackground(true);

    addMusicImg = new QLabel(addMusicMsgWidget);
    addMusicImg->setAlignment(Qt::AlignCenter);
    addMusicImg->setPixmap(QIcon::fromTheme(addMusicIcon).pixmap(48));
    addMusicImg->setEnabled(false);

    auto addMusicImg_effect= new QGraphicsOpacityEffect(addMusicImg);
    addMusicImg_effect->setOpacity(0.5);
    addMusicImg->setGraphicsEffect(addMusicImg_effect);


    addMusicMsg_layout->addStretch();
    addMusicMsg_layout->addWidget(addMusicImg);
    addMusicMsg_layout->addWidget(addMusicTxt);
    addMusicMsg_layout->addStretch();

    auto addMusicTxt_layout = new QHBoxLayout(this);
    addMusicTxt_layout->addStretch();
    addMusicTxt_layout->addWidget(addMusicMsgWidget,Qt::AlignCenter); // center alignment
    addMusicTxt_layout->addStretch();


    connect(updater, SIGNAL(timeout()), this, SLOT(update()));
    updater->start(100);
}




void BabeTable::dropEvent(QDropEvent *event)
{
    if(event->source() == this && !event->isAccepted() && rowDragging)
    {
        int newRow = this->indexAt(event->pos()).row();
        auto insertionRow = newRow;

        if(this->getItem(insertionRow,Bae::DBCols::TITLE)->icon().name()!="clock")
        {
            qDebug()<<"new row position"<< newRow;
            auto list = this->getSelectedRows(false);

            Bae::DB_LIST tracks;
            QList<int> newList;

            for(auto track : list)
            {
                if(this->getItem(track,Bae::DBCols::TITLE)->icon().name().isEmpty())
                {
                    tracks<<this->getRowData(track);
                    newList<<track;
                }

            }

            if(!newList.isEmpty())
            {
                int i=0;
                int j=0;

                std::sort(newList.begin(),newList.end());

                for(auto track:newList)
                {
                    if(track>=newRow)
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

                i=0;

                for(auto track : tracks)
                {
                    this->addRowAt(newRow+i+1,track);
                    qDebug()<<"indexes moved "<< newList.at(i)<<insertionRow<<newRow;

                    emit indexesMoved(newList.at(i),insertionRow);
                    i++;
                }

            }
        }
    }
}




void BabeTable::setAddMusicMsg(const QString &msg,const QString &icon)
{
    this->addMusicMsg=msg;
    addMusicTxt->setText(addMusicMsg);
    addMusicIcon=icon;
    addMusicImg->setPixmap(QIcon::fromTheme(addMusicIcon).pixmap(48));

}

void BabeTable::update()
{
    if(this->rowCount()!=0)
    {
        if(addMusicMsgWidget->isVisible()) addMusicMsgWidget->setVisible(false);

    }else addMusicMsgWidget->setVisible(true);

    if(preview->state() == QMediaPlayer::StoppedState && previewRow!=-1)

    {

        this->getItem(this->previewRow,Bae::DBCols::TITLE)->setIcon(QIcon::fromTheme(""));
        emit previewFinished();
        previewRow=-1;
    }

}

void BabeTable::moodTrack(int color) { moodIt_action(colors.at(color)); }

void BabeTable::addToPlaylist()
{    
    Bae::DB_LIST mapList;
    for(auto row : this->getSelectedRows())
        mapList<< this->getRowData(row);

    emit saveToPlaylist(mapList);
}


void BabeTable::enterEvent(QEvent *event)
{
    emit enterTable();

    QTableWidget::enterEvent(event);

}

void BabeTable::leaveEvent(QEvent *event)
{

    this->stopPreview();
    emit leaveTable();
    QTableWidget::leaveEvent(event);


}

void BabeTable::passStyle(QString style) { this->setStyleSheet(style); }

int BabeTable::getIndex() { return this->currentIndex().row(); }

QTableWidgetItem *BabeTable::getItem(const int &row, const Bae::DBCols &column)
{
    return this->item(row,static_cast<int>(column));
}

void BabeTable::putItem(const int &row, const Bae::DBCols &col, QTableWidgetItem *item)
{
    this->setItem(row, static_cast<int>(col), item);
}


void BabeTable::enableRowColoring(const bool &state)
{
    this->rowColoring=state;
}

void BabeTable::enableRowDragging(const bool &state)
{
    this->rowDragging=state;
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
    item->setParent(contextMenu);
    contextMenu->addAction(item);
}



void BabeTable::addRow(const Bae::DB &map)
{
    this->insertRow(this->rowCount());

    this->putItem(this->rowCount() - 1,Bae::DBCols::TRACK, new QTableWidgetItem(map[Bae::DBCols::TRACK]));
    this->putItem(this->rowCount() - 1,Bae::DBCols::TITLE, new QTableWidgetItem(map[Bae::DBCols::TITLE]));
    this->putItem(this->rowCount() - 1,Bae::DBCols::ARTIST, new QTableWidgetItem(map[Bae::DBCols::ARTIST]));
    this->putItem(this->rowCount() - 1,Bae::DBCols::ALBUM, new QTableWidgetItem(map[Bae::DBCols::ALBUM]));
    this->putItem(this->rowCount() - 1,Bae::DBCols::DURATION, new QTableWidgetItem(Bae::transformTime(map[Bae::DBCols::DURATION].toInt())));
    this->putItem(this->rowCount() - 1,Bae::DBCols::GENRE, new QTableWidgetItem(map[Bae::DBCols::GENRE]));
    this->putItem(this->rowCount() - 1,Bae::DBCols::URL, new QTableWidgetItem(map[Bae::DBCols::URL]));
    this->putItem(this->rowCount() - 1,Bae::DBCols::STARS, new QTableWidgetItem(this->getStars(map[Bae::DBCols::STARS].toInt())));
    this->putItem(this->rowCount() - 1,Bae::DBCols::BABE, new QTableWidgetItem(this->getHearts(map[Bae::DBCols::BABE].toInt())));
    //    this->setItem(this->rowCount() - 1, Bae::DBCols::ART, new QTableWidgetItem(map[Bae::DBCols::ART]));
    this->putItem(this->rowCount() - 1,Bae::DBCols::PLAYED, new QTableWidgetItem(map[Bae::DBCols::PLAYED]));
    this->putItem(this->rowCount() - 1,Bae::DBCols::RELEASE_DATE, new QTableWidgetItem(map[Bae::DBCols::RELEASE_DATE]));
    this->putItem(this->rowCount() - 1,Bae::DBCols::ADD_DATE, new QTableWidgetItem(map[Bae::DBCols::ADD_DATE]));

    if(this->rowColoring && !map[Bae::DBCols::ART].isEmpty())
        this->colorizeRow({this->rowCount()-1},map[Bae::DBCols::ART]);

    //    if(descriptiveTooltip)
    //        this->item(this->rowCount()-1,Bae::DBCols::TITLE)->setToolTip( "by "+map[Bae::DBCols::ARTIST]);
}

void BabeTable::addRowAt(const int &row, const Bae::DB &map)
{
    this->insertRow(row);
    this->putItem(row,Bae::DBCols::URL, new QTableWidgetItem(map[Bae::DBCols::URL]));
    this->putItem(row ,Bae::DBCols::TITLE, new QTableWidgetItem(map[Bae::DBCols::TITLE]));

    this->putItem(row,Bae::DBCols::TRACK, new QTableWidgetItem(map[Bae::DBCols::TRACK]));
    this->putItem(row,Bae::DBCols::ARTIST, new QTableWidgetItem(map[Bae::DBCols::ARTIST]));
    this->putItem(row ,Bae::DBCols::ALBUM, new QTableWidgetItem(map[Bae::DBCols::ALBUM]));

    this->putItem(row,Bae::DBCols::DURATION, new QTableWidgetItem(Bae::transformTime(map[Bae::DBCols::DURATION].toInt())));
    this->putItem(row,Bae::DBCols::GENRE, new QTableWidgetItem(map[Bae::DBCols::GENRE]));
    this->putItem(row,Bae::DBCols::STARS, new QTableWidgetItem(this->getStars(map[Bae::DBCols::STARS].toInt())));

    this->putItem(row ,Bae::DBCols::BABE, new QTableWidgetItem(this->getHearts(map[Bae::DBCols::BABE].toInt())));
    //    this->setItem(row , Bae::DBCols::ART, new QTableWidgetItem(map[Bae::DBCols::ART]));
    this->putItem(row,Bae::DBCols::PLAYED, new QTableWidgetItem(map[Bae::DBCols::PLAYED]));
    this->putItem(row,Bae::DBCols::RELEASE_DATE, new QTableWidgetItem(map[Bae::DBCols::RELEASE_DATE]));
    this->putItem(row,Bae::DBCols::ADD_DATE, new QTableWidgetItem(map[Bae::DBCols::ADD_DATE]));
    this->putItem(row,Bae::DBCols::SOURCES_URL, new QTableWidgetItem(map[Bae::DBCols::SOURCES_URL]));
    this->putItem(row,Bae::DBCols::LYRICS, new QTableWidgetItem(map[Bae::DBCols::LYRICS]));

    if(this->rowColoring && !map[Bae::DBCols::ART].isEmpty())
        this->colorizeRow({row},map[Bae::DBCols::ART]);


    //    if(descriptiveTooltip)
    //        this->item(row,Bae::DBCols::TITLE)->setToolTip( "by "+map[Bae::DBCols::ARTIST]);
}

void BabeTable::removeMissing(const QString &url)
{
    nof->notifyUrgent("Removing missing file...",url);

    auto parentDir = QFileInfo(QFileInfo(url)).dir().path();

    if (!Bae::fileExists(parentDir)) connection.removeSource(parentDir);
    else connection.removeTrack(url);

}


void BabeTable::insertTrack(const Bae::DB &track)
{
    auto location =track[Bae::DBCols::URL];

    if (!Bae::fileExists(location))  removeMissing(location);
    else addRow(track);
}

void BabeTable::populateTableView(const Bae::DB_LIST &mapList)
{
    this->setSortingEnabled(false);
    if(!mapList.isEmpty())
    {
        for(auto trackMap : mapList) insertTrack(trackMap);
        this->setSortingEnabled(true);
        emit finishedPopulating();
    }

}

void BabeTable::populateTableView(QSqlQuery &query)
{
    qDebug() << "ON POPULATE TABLEVIEW";
    this->setSortingEnabled(false);
    trackLoader.requestTracks(query.lastQuery());
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

void BabeTable::setTableOrder(int column, Bae::Order order)
{
    if (order == Bae::Order::DESC) {
        this->sortByColumn(column, Qt::DescendingOrder);
    } else if (order == Bae::Order ::ASC) {
        this->sortByColumn(column, Qt::AscendingOrder);
    }
}

void BabeTable::setVisibleColumn(const Bae::DBCols &column)
{
    if (column == Bae::DBCols::URL)
        this->showColumn(static_cast<int>(Bae::DBCols::URL));
    else if (column == Bae::DBCols::STARS)
        this->showColumn(static_cast<int>(Bae::DBCols::STARS));
    else if (column == Bae::DBCols::BABE)
        this->showColumn(static_cast<int>(Bae::DBCols::BABE));
    else if (column == Bae::DBCols::ALBUM)
        this->showColumn(static_cast<int>(Bae::DBCols::ALBUM));
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

    sendToMenu->clear();
    QMapIterator<QString, QString> i(getKdeConnectDevices());
    while (i.hasNext())
    {
        i.next();
        sendToMenu->addAction(i.value());
    }

    auto rowData = this->getRowData(rRow);
    if(!rowData.isEmpty())
    {
        QString url =rowData[Bae::DBCols::URL];
        setRating(this->connection.getTrackStars(url));

        if (this->connection.getTrackBabe(url))
            contextMenu->actions().at(0)->setText("Un-Babe it");
        else
            contextMenu->actions().at(0)->setText("Babe it");

        contextMenu->exec(QCursor::pos());
    }

}


void BabeTable::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Return: {


        Bae::DB_LIST list;
        for(auto row : this->getSelectedRows(false))
        {
            list<<getRowData(row);

            qDebug()<<row;
        }

        stopPreview();

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

        break;
    }
    case Qt::Key_Space:
    {
        if(rowPreview)
        {
            if(preview->state()==QMediaPlayer::PlayingState) this->stopPreview();
            else
            {
                previewRow= this->getIndex();
                this->startPreview(this->getRowData(previewRow)[Bae::DBCols::URL]);
            }
        }

        break;
    }

    case Qt::Key_Delete	:
    {
        auto rows = this->getSelectedRows(false);
        int i=0;
        for(auto row:rows)
        {
            emit removeIt_clicked(row-i);
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
        Bae::DB_LIST mapList;
        for(auto row : this->getSelectedRows(false))
            mapList<< this->getRowData(row);
        stopPreview();

        emit queueIt_clicked(mapList);
        break;
    }

    case Qt::Key_P :
    {
        Bae::DB_LIST mapList;
        for(auto row : this->getSelectedRows(false))
            mapList<< this->getRowData(row);

        stopPreview();

        emit playItNow(mapList);
        break;
    }

    case Qt::Key_L :
    {
        Bae::DB_LIST mapList;
        for(auto row : this->getSelectedRows(false))
            mapList<< this->getRowData(row);

        stopPreview();
        emit appendIt(mapList);
        break;
    }


    case Qt::Key_S :
    {
        Bae::DB_LIST mapList;
        for(auto row : this->getSelectedRows(false))
            mapList<< this->getRowData(row);

        stopPreview();
        emit saveToPlaylist(mapList);
        break;
    }

    case Qt::Key_I:

    {
        auto url =  this->getRowData(this->getIndex())[Bae::DBCols::URL];
        emit infoIt_clicked(this->connection.getTrackData(QStringList(url)).first());
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


    default: {
        QTableWidget::keyPressEvent(event);
        break;
    }
    }
}

void BabeTable::mousePressEvent(QMouseEvent *evt)
{

    if (evt->button() == Qt::RightButton)
    {
        qDebug() << "table right clicked";
        evt->accept();
        int row = this->indexAt(evt->pos()).row();
        int column= this->indexAt(evt->pos()).column();
        qDebug()<<row << column;

        if(row != -1) emit rightClicked(row, column);
    }

    QTableWidget::mousePressEvent(evt);

}

void BabeTable::enablePreview(const bool state)
{
    this->rowPreview=state;
}

void BabeTable::startPreview(const QString &url)
{
    preview->setMedia(QUrl::fromLocalFile(url));
    preview->play();
    this->getItem(this->getIndex(),Bae::DBCols::TITLE)->setIcon(QIcon::fromTheme("quickview"));
    emit previewStarted();
}

void BabeTable::stopPreview()
{
    if(preview->state()==QMediaPlayer::PlayingState)
    {
        preview->stop();
        this->getItem(this->previewRow,Bae::DBCols::TITLE)->setIcon(QIcon::fromTheme(""));
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
        QString location = this->getRowData(row)[Bae::DBCols::URL];

        if (connection.rateTrack(location,id))
        {
            setRating(id);

            QString stars;
            for (int i = 0; i < id; i++)
                stars += "\xe2\x98\x86 ";

            this->getItem(row,Bae::DBCols::STARS)->setText(stars);

        } else qDebug() << "rating failed for"<< location;
    }
}

Bae::DB BabeTable::getRowData(const int &row)
{
    QString url = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::URL))).toString();

    if(connection.check_existance(Bae::DBTablesMap[Bae::DBTables::TRACKS],Bae::TracksColsMap[Bae::DBCols::URL],url))
        return connection.getTrackData(QStringList(url)).first();
    else
    {
        QString track = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::TRACK))).toString();
        QString title = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::TITLE))).toString();
        QString artist = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::ARTIST))).toString();
        QString album = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::ALBUM))).toString();
        QString duration = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::DURATION))).toString();
        QString genre = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::GENRE))).toString();
        QString location = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::URL))).toString();
        QString stars = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::STARS))).toString();
        QString babe = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::BABE))).toString();
        QString releaseDate = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::RELEASE_DATE))).toString();
        QString addDate = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::ADD_DATE))).toString();
        QString played = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::PLAYED))).toString();
        QString art = this->model()->data(this->model()->index(row,static_cast<int>(Bae::DBCols::ART))).toString();

        return Bae::DB
        {
            {Bae::DBCols::TRACK,track},
            {Bae::DBCols::TITLE,title},
            {Bae::DBCols::ARTIST,artist},
            {Bae::DBCols::ALBUM,album},
            {Bae::DBCols::DURATION,duration},
            {Bae::DBCols::GENRE,genre},
            {Bae::DBCols::URL,location},
            {Bae::DBCols::STARS,stars},
            {Bae::DBCols::BABE,babe},
            {Bae::DBCols::ART,art},
            {Bae::DBCols::PLAYED,played},
            {Bae::DBCols::RELEASE_DATE,releaseDate},
            {Bae::DBCols::ADD_DATE,addDate}
        };

    }
}


void BabeTable::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    Bae::DB_LIST list;
    auto track = getRowData(this->getIndex());
    list<<track;
    qDebug()
            << "BabeTable doubleClicked item<<"
            << track[Bae::DBCols::URL];

    emit tableWidget_doubleClicked(list);
}

void BabeTable::babeIt_action()
{
    Bae::DB_LIST mapList;
    for(auto row: this->getSelectedRows())
        mapList<< this->getRowData(row);

    emit babeIt_clicked(mapList);
}

void BabeTable::sendIt_action(QAction *device)
{
    for(auto row: this->getSelectedRows())
    {

        auto track = this->getRowData(row);

        QString url =track[Bae::DBCols::URL];
        QString title = track[Bae::DBCols::TITLE];
        QString artist = track[Bae::DBCols::ARTIST];

        QString deviceName = device->text().replace("&","");
        QString deviceKey = devices.key(deviceName);

        qDebug()<<"trying to send "<< url << "to : "<< deviceName;
        auto process = new QProcess(this);
        connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            qDebug()<<"processFinished_totally"<<exitCode<<exitStatus;
            nof->notifyUrgent("Song sent to " + deviceName,title +" by "+ artist);

        });

        qDebug()<<"kdeconnect-cli -d " +deviceKey+ " --share " + url;
        process->start("kdeconnect-cli -d " +deviceKey+ " --share " +"\""+ url+"\"");
    }
}

void BabeTable::editIt_action()
{
    //editing=true;
    // emit this->edit(this->model()->index(rRow,rColumn));

    auto infoForm = new metadataForm(getRowData(rRow),this);
    connect(infoForm, &metadataForm::infoModified,this,&BabeTable::itemEdited);
    infoForm->show();

}

void BabeTable::itemEdited(const Bae::DB &map)
{
    qDebug()<<"item changed: " << map[Bae::DBCols::TITLE];
    this->getItem(rRow,Bae::DBCols::TRACK)->setText(map[Bae::DBCols::TRACK]);
    this->getItem(rRow,Bae::DBCols::TITLE)->setText(map[Bae::DBCols::TITLE]);
    this->getItem(rRow,Bae::DBCols::ARTIST)->setText(map[Bae::DBCols::ARTIST]);
    this->getItem(rRow,Bae::DBCols::ALBUM)->setText(map[Bae::DBCols::ALBUM]);
    this->getItem(rRow,Bae::DBCols::GENRE)->setText(map[Bae::DBCols::GENRE]);

    //connection.insertInto("tracks",column,this->model()->index(newIndex.row(),LOCATION).data().toString(),newIndex.data().toString());

}

void BabeTable::infoIt_action()
{
    emit infoIt_clicked(this->getRowData(rRow));
}

void BabeTable::removeIt_action()
{
    emit removeIt_clicked(rRow);
}

void BabeTable::moodIt_action(const QString &color)
{
    if(!color.isEmpty())
    {
        for(auto row : this->getSelectedRows())
        {
            auto url =this->getRowData(row)[Bae::DBCols::URL];
            if(connection.artTrack(url,color)) qDebug()<< "moodIt was sucessful";
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
        this->getItem(row,Bae::DBCols::TITLE)->setBackgroundColor(coloring);
        if(dark)
        {
            QBrush brush;
            brush.setColor("#fff");
            this->getItem(row,Bae::DBCols::TITLE)->setForeground(brush);
        }
        //this->item(row,TITLE)->setTextAlignment(Qt::AlignCenter);
    }
}

void BabeTable::queueIt_action()
{

    Bae::DB_LIST mapList;
    for(auto row : this->getSelectedRows())
        mapList<< this->getRowData(row);

    emit queueIt_clicked(mapList);
}

QList<int> BabeTable::getSelectedRows(const bool &onRightClick)
{
    QList<int> selectedRows;
    auto selection = this->selectionModel()->selectedRows(static_cast<int>(Bae::DBCols::TITLE));
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

QStringList BabeTable::getTableColumnContent(const Bae::DBCols &column)
{
    QStringList result;

    if(static_cast<int>(column) <= this->columnCount())
    {
        for (int i = 0; i < this->rowCount(); i++)
            result << this->getRowData(i)[column];
    }
    return result;
}


Bae::DB_LIST BabeTable::getAllTableContent()
{
    Bae::DB_LIST mapList;

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
        auto trackInfo = track[Bae::DBCols::TITLE]+"/&/"+track[Bae::DBCols::ARTIST]+"/&/"+track[Bae::DBCols::ALBUM];

        if(index.contains(trackInfo) && this->getItem(row,Bae::DBCols::TITLE)->icon().name().isEmpty())
        {
            qDebug()<<"index contains:"<<track[Bae::DBCols::TITLE]<<row;;
            this->removeRow(row);
            emit indexRemoved(row);
            row--;

        }
        else
        {
            qDebug()<<"adding to index:"<<track[Bae::DBCols::TITLE]<<row;

            index<<trackInfo;
        }


    }

}
