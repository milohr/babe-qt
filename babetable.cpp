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


BabeTable::BabeTable(QWidget *parent) : QTableWidget(parent) {


    preview = new QMediaPlayer(this);
    preview->setVolume(100);
    connect(this,&QTableWidget::doubleClicked, this, &BabeTable::on_tableWidget_doubleClicked);
    connect(this,&QTableWidget::itemSelectionChanged,[this](){this->stopPreview();});
    //    connect(this->selectionModel(),&QItemSelectionModel::selectionChanged,[this](){    this->setSelectionMode(QAbstractItemView::SingleSelection);
    //qDebug()<<"a row got selected;";});
    //connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),SLOT(setUpContextMenu(const QPoint&)));
    //connect(this->model(),SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),this,SLOT(itemEdited(const QModelIndex&, const QModelIndex&)));
    //this->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    this->setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setColumnCount(BaeUtils::TracksCols::columnsCOUNT-1);
    this->setHorizontalHeaderLabels({"Location", "Source", "#","Title","Artist", "Album","Duration", "Played",
                                     "Babe", "Stars", "Release", "Date","Lyrics", "Genre","Art"});
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

    this->setColumnWidth(BaeUtils::TracksCols::TRACK, 20);
    this->setColumnWidth(BaeUtils::TracksCols::PLAYED, 20);
    this->setColumnWidth(BaeUtils::TracksCols::STARS, 80);
    this->hideColumn(BaeUtils::TracksCols::URL);
    this->hideColumn(BaeUtils::TracksCols::STARS);
    this->hideColumn(BaeUtils::TracksCols::BABE);
    this->hideColumn(BaeUtils::TracksCols::GENRE);
    this->hideColumn(BaeUtils::TracksCols::TRACK);
    this->hideColumn(BaeUtils::TracksCols::PLAYED);
    this->hideColumn(BaeUtils::TracksCols::ART);
    this->hideColumn(BaeUtils::TracksCols::RELEASE_DATE);
    this->hideColumn(BaeUtils::TracksCols::ADD_DATE);
    this->hideColumn(BaeUtils::TracksCols::SOURCES_URL);
    this->hideColumn(BaeUtils::TracksCols::LYRICS);
    this->hideColumn(BaeUtils::TracksCols::ADD_DATE);

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

    // this->horizontalHeaderItem(0);
    // this->horizontalHeader()->setHighlightSections(true);



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
    // connect(fav1,SIGNAL(enterEvent(QEvent)),this,hoverEvent());
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
    for(int i=0; i<5; i++)
    {
        auto  *colorTag = new QToolButton();
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
    addMusicTxt = new QLabel();
    addMusicTxt->setObjectName("addMusicTxt");
    addMusicTxt->setStyleSheet("QLabel{background-color:transparent;}");
    addMusicTxt->setText(addMusicMsg);
    addMusicTxt->setFont(helvetica);
    addMusicTxt->setWordWrap(true);
    addMusicTxt->setAlignment(Qt::AlignCenter);

    auto effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.5);
    addMusicTxt->setGraphicsEffect(effect);
    addMusicTxt->setAutoFillBackground(true);

    auto effect2= new QGraphicsOpacityEffect();
    effect2->setOpacity(0.5);


    addMusicImg = new QLabel();
    addMusicImg->setAlignment(Qt::AlignCenter);
    addMusicImg->setPixmap(QIcon::fromTheme(addMusicIcon).pixmap(48));
    addMusicImg->setEnabled(false);
    addMusicImg->setGraphicsEffect(effect2);

    auto addMusicMsg_layout = new QVBoxLayout();
    addMusicMsgWidget = new QWidget(this);
    addMusicMsgWidget->setVisible(false);
    addMusicMsgWidget->setLayout(addMusicMsg_layout);
    addMusicMsg_layout->addStretch();
    addMusicMsg_layout->addWidget(addMusicImg);
    addMusicMsg_layout->addWidget(addMusicTxt);
    addMusicMsg_layout->addStretch();

    auto addMusicTxt_layout = new QHBoxLayout(this);
    addMusicTxt_layout->addStretch();
    addMusicTxt_layout->addWidget(addMusicMsgWidget); // center alignment
    addMusicTxt_layout->addStretch();

    connect(updater, SIGNAL(timeout()), this, SLOT(update()));
    updater->start(100);
}


BabeTable::~BabeTable() {  }

void BabeTable::dropEvent(QDropEvent *event)
{


    if(event->source() == this && !event->isAccepted() && rowDragging )
    {
        int newRow = this->indexAt(event->pos()).row();
        auto insertionRow = newRow;

        if(this->item(insertionRow,BaeUtils::TracksCols::TITLE)->icon().name()!="clock")
        {
            qDebug()<<"new row position"<< newRow;
            auto list = this->getSelectedRows(false);
            QList<QMap<int,QString>> tracks;
            QList<int> newList;
            for(auto track : list)
            {
                if(this->item(track,BaeUtils::TracksCols::TITLE)->icon().name().isEmpty())
                {
                    tracks<<this->getRowData(track);
                    newList<<track;
                }

            }

            if(!newList.isEmpty())
            {
                int i =0;
                int j =0;

                std::sort(newList.begin(),newList.end());

                for(auto track:newList)
                {
                    if(track>=newRow)
                    {
                        this->removeRow(track-i-j);
                        i++;
                    }

                    else
                    {
                        this->removeRow(track-j);
                        j++;
                        newRow--;
                    }



                }


                i =0;
                for(auto track : tracks)
                {
                    this->addRowAt(newRow+1+i,track,true);
                    qDebug()<<"indexes moved "<< newList.at(i)<<insertionRow;

                    emit indexesMoved(newList.at(i),insertionRow);
                    i++;

                }

            }
        }
    }
}




void BabeTable::setAddMusicMsg(const QString &msg,const QString &icon)
{
    addMusicMsg=msg;
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

        this->item(this->previewRow,BaeUtils::TracksCols::TITLE)->setIcon(QIcon::fromTheme(""));
        emit previewFinished();
        previewRow=-1;
    }

}

void BabeTable::moodTrack(int color) { moodIt_action(colors.at(color)); }

void BabeTable::addToPlaylist()
{
    QList<QMap<int,QString>> mapList;
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


void BabeTable::addRow(const QMap<int, QString> &map, const  bool &descriptiveTooltip)
{
    this->insertRow(this->rowCount());

    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::TRACK, new QTableWidgetItem(map[BaeUtils::TracksCols::TRACK]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::TITLE, new QTableWidgetItem(map[BaeUtils::TracksCols::TITLE]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::ARTIST, new QTableWidgetItem(map[BaeUtils::TracksCols::ARTIST]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::ALBUM, new QTableWidgetItem(map[BaeUtils::TracksCols::ALBUM]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::DURATION, new QTableWidgetItem(BaeUtils::transformTime(map[BaeUtils::TracksCols::DURATION].toInt())));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::GENRE, new QTableWidgetItem(map[BaeUtils::TracksCols::GENRE]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::URL, new QTableWidgetItem(map[BaeUtils::TracksCols::URL]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::STARS, new QTableWidgetItem(this->getStars(map[BaeUtils::TracksCols::STARS].toInt())));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::BABE, new QTableWidgetItem(map[BaeUtils::TracksCols::BABE]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::ART, new QTableWidgetItem(map[BaeUtils::TracksCols::ART]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::PLAYED, new QTableWidgetItem(map[BaeUtils::TracksCols::PLAYED]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::RELEASE_DATE, new QTableWidgetItem(map[BaeUtils::TracksCols::RELEASE_DATE]));
    this->setItem(this->rowCount() - 1, BaeUtils::TracksCols::ADD_DATE, new QTableWidgetItem(map[BaeUtils::TracksCols::ADD_DATE]));

    if(this->rowColoring && !map[BaeUtils::TracksCols::ART].isEmpty())
        this->colorizeRow({this->rowCount()-1},map[BaeUtils::TracksCols::ART]);

    if(descriptiveTooltip)
        this->item(this->rowCount()-1,BaeUtils::TracksCols::TITLE)->setToolTip( "by "+map[BaeUtils::TracksCols::ARTIST]);
}

void BabeTable::addRowAt(const int &row,const QMap<int, QString> &map,const  bool &descriptiveTooltip)
{
    this->insertRow(row);

    this->setItem(row , BaeUtils::TracksCols::TRACK, new QTableWidgetItem(map[BaeUtils::TracksCols::TRACK]));
    this->setItem(row , BaeUtils::TracksCols::TITLE, new QTableWidgetItem(map[BaeUtils::TracksCols::TITLE]));
    this->setItem(row , BaeUtils::TracksCols::ARTIST, new QTableWidgetItem(map[BaeUtils::TracksCols::ARTIST]));
    this->setItem(row , BaeUtils::TracksCols::ALBUM, new QTableWidgetItem(map[BaeUtils::TracksCols::ALBUM]));
    this->setItem(row , BaeUtils::TracksCols::DURATION, new QTableWidgetItem(BaeUtils::transformTime(map[BaeUtils::TracksCols::DURATION].toInt())));
    this->setItem(row , BaeUtils::TracksCols::GENRE, new QTableWidgetItem(map[BaeUtils::TracksCols::GENRE]));
    this->setItem(row , BaeUtils::TracksCols::URL, new QTableWidgetItem(map[BaeUtils::TracksCols::URL]));
    this->setItem(row , BaeUtils::TracksCols::STARS, new QTableWidgetItem(this->getStars(map[BaeUtils::TracksCols::STARS].toInt())));
    this->setItem(row , BaeUtils::TracksCols::BABE, new QTableWidgetItem(map[BaeUtils::TracksCols::BABE]));
    this->setItem(row , BaeUtils::TracksCols::ART, new QTableWidgetItem(map[BaeUtils::TracksCols::ART]));
    this->setItem(row , BaeUtils::TracksCols::PLAYED, new QTableWidgetItem(map[BaeUtils::TracksCols::PLAYED]));
    this->setItem(row , BaeUtils::TracksCols::RELEASE_DATE, new QTableWidgetItem(map[BaeUtils::TracksCols::RELEASE_DATE]));
    this->setItem(row , BaeUtils::TracksCols::ADD_DATE, new QTableWidgetItem(map[BaeUtils::TracksCols::ADD_DATE]));

    if(this->rowColoring && !map[BaeUtils::TracksCols::ART].isEmpty())
        this->colorizeRow({row},map[BaeUtils::TracksCols::ART]);


    if(descriptiveTooltip)
        this->item(row,BaeUtils::TracksCols::TITLE)->setToolTip( "by "+map[BaeUtils::TracksCols::ARTIST]);
}

void BabeTable::populateTableView(const QList<QMap<int,QString>>& mapList, const bool &descriptiveTitle)
{
    qDebug() << "ON POPULATE by mapList";

    this->setSortingEnabled(false);
    bool missing = false;
    QStringList missingFiles;

    if(!mapList.isEmpty())
    {
        for(auto trackMap : mapList)
        {
            QString location = trackMap[BaeUtils::TracksCols::URL];

            if (!BaeUtils::fileExists(location))
            {
                qDebug() << "That file doesn't exists anymore: "
                         << location;
                missingFiles << location;
                missing = true;

            } else addRow(trackMap,descriptiveTitle);

        }

        if (missing) removeMissing(missingFiles);

        this->setSortingEnabled(true);
        emit finishedPopulating();

    }else qDebug()<<"Error: the mapList was empty";

}

void BabeTable::removeMissing(QStringList missingFiles)
{
    nof.notifyUrgent("Removing missing files...",missingFiles.join("\n"));

    for (auto file_r : missingFiles)
    {
        QString parentDir = QFileInfo(QFileInfo(file_r)).dir().path();

        if (!BaeUtils::fileExists(parentDir)) connection.removePath(parentDir);
        else connection.removePath(file_r);
    }

    connection.setCollectionLists();
    connection.cleanCollectionLists();
}

void BabeTable::populateTableView(const QString &indication, const bool &descriptiveTitle)
{
    qDebug() << "ON POPULATE:"<<indication;

    this->setSortingEnabled(false);
    bool missingDialog = false;
    QStringList missingFiles;
    QSqlQuery query = connection.getQuery(indication);

    if(query.exec())
    {
        while (query.next())
        {

            QString location =query.value(BaeUtils::TracksCols::URL).toString();

            if (!BaeUtils::fileExists(location))
            {
                qDebug() << "That file doesn't exists anymore: "
                         << location;
                missingFiles << location;
                missingDialog = true;

            } else
            {

                QString track = query.value(BaeUtils::TracksCols::TRACK).toString();
                QString title = query.value(BaeUtils::TracksCols::TITLE).toString();
                QString artist = query.value(BaeUtils::TracksCols::ARTIST).toString();
                QString album = query.value(BaeUtils::TracksCols::ALBUM).toString();
                QString genre = query.value(BaeUtils::TracksCols::GENRE).toString();
                QString stars = query.value(BaeUtils::TracksCols::STARS).toString();


                QString babe;
                switch (query.value(BaeUtils::TracksCols::BABE).toInt()) {
                case 0:
                    babe = " ";
                    break;
                case 1:
                    babe = "\xe2\x99\xa1 ";
                    break;
                }

                QString art = query.value(BaeUtils::TracksCols::ART).toString();
                QString played =query.value(BaeUtils::TracksCols::PLAYED).toString();
                QString duration =query.value(BaeUtils::TracksCols::DURATION).toString();
                QString releaseDate = query.value(BaeUtils::TracksCols::RELEASE_DATE).toString();
                QString addDate = query.value(BaeUtils::TracksCols::ADD_DATE).toString();

                const QMap<int, QString> map{{BaeUtils::TracksCols::TRACK,track}, {BaeUtils::TracksCols::TITLE,title},{BaeUtils::TracksCols::ARTIST,artist},{BaeUtils::TracksCols::ALBUM,album},{BaeUtils::TracksCols::DURATION,duration},{BaeUtils::TracksCols::GENRE,genre},{BaeUtils::TracksCols::URL,location},{BaeUtils::TracksCols::STARS,stars},{BaeUtils::TracksCols::BABE,babe},{BaeUtils::TracksCols::ART,art},{BaeUtils::TracksCols::PLAYED,played},{BaeUtils::TracksCols::RELEASE_DATE,releaseDate},{BaeUtils::TracksCols::ADD_DATE,addDate}};

                addRow(map,descriptiveTitle);
            }
        }


        if (missingDialog) removeMissing(missingFiles);

        this->setSortingEnabled(true);
        emit finishedPopulating();
    }else qDebug()<<"Error: the query didn't pass"<<indication;


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

void BabeTable::setRating(int rate)
{
    switch (rate)
    {

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

void BabeTable::setTableOrder(int column, int order) {
    if (order == DESCENDING) {
        this->sortByColumn(column, Qt::DescendingOrder);
    } else if (order == ASCENDING) {
        this->sortByColumn(column, Qt::AscendingOrder);
    }
}

void BabeTable::setVisibleColumn(int column) {
    if (column == BaeUtils::TracksCols::URL) {
        this->showColumn(BaeUtils::TracksCols::URL);
    } else if (column == BaeUtils::TracksCols::STARS) {
        this->showColumn(BaeUtils::TracksCols::STARS);
    } else if (column == BaeUtils::TracksCols::BABE) {
        this->showColumn(BaeUtils::TracksCols::BABE);
    } else if (column == BaeUtils::TracksCols::ALBUM) {
        this->showColumn(BaeUtils::TracksCols::ALBUM);
    }
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

    while (process.canReadLine()) {
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

    int rate = 0;
    bool babe = false;
    this->rRow = row;
    this->rColumn= column;

    sendToMenu->clear();
    QMapIterator<QString, QString> i(getKdeConnectDevices());
    while (i.hasNext())
    {
        i.next();
        qDebug()<<i.key();
        sendToMenu->addAction(i.value());
    }

    auto rowData = this->getRowData(rRow);
    if(!rowData.isEmpty())
    {
        QString url =rowData[BaeUtils::TracksCols::URL];

        QSqlQuery query = connection.getQuery(
                    "SELECT * FROM tracks WHERE url = \"" + url + "\"");

        while (query.next())
        {
            rate = query.value(BaeUtils::TracksCols::STARS).toInt();
            babe = query.value(BaeUtils::TracksCols::BABE).toInt() == 1 ? true : false;
        }

        setRating(rate);

        if (babe)
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


        QList<QMap<int, QString>> list;
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
                this->startPreview(this->getRowData(previewRow)[BaeUtils::TracksCols::URL]);
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
        QList<QMap<int,QString>> mapList;
        for(auto row : this->getSelectedRows(false))
            mapList<< this->getRowData(row);
        stopPreview();

        emit queueIt_clicked(mapList);
        break;
    }

    case Qt::Key_P :
    {
        QList<QMap<int,QString>> mapList;
        for(auto row : this->getSelectedRows(false))
            mapList<< this->getRowData(row);

        stopPreview();

        emit playItNow(mapList);
        break;
    }

    case Qt::Key_L :
    {
        QList<QMap<int,QString>> mapList;
        for(auto row : this->getSelectedRows(false))
            mapList<< this->getRowData(row);

        stopPreview();
        emit appendIt(mapList);
        break;
    }


    case Qt::Key_S :
    {
        QList<QMap<int,QString>> mapList;
        for(auto row : this->getSelectedRows(false))
            mapList<< this->getRowData(row);

        stopPreview();
        emit saveToPlaylist(mapList);
        break;
    }

    case Qt::Key_I:

    {
        QString artist = this->model()->data(this->model()->index(this->getIndex(), BaeUtils::TracksCols::ARTIST)).toString();
        QString album = this->model()->data(this->model()->index(this->getIndex(), BaeUtils::TracksCols::ALBUM)).toString();
        QString title = this->model()->data(this->model()->index(this->getIndex(), BaeUtils::TracksCols::TITLE)).toString();
        emit infoIt_clicked(title, artist, album);
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
    this->item(this->getIndex(),BaeUtils::TracksCols::TITLE)->setIcon(QIcon::fromTheme("quickview"));
    emit previewStarted();
}

void BabeTable::stopPreview()
{
    if(preview->state()==QMediaPlayer::PlayingState)
    {
        preview->stop();
        this->item(this->previewRow,BaeUtils::TracksCols::TITLE)->setIcon(QIcon::fromTheme(""));
        emit previewFinished();
        previewRow=-1;
    }
}


void BabeTable::rateGroup(const int &id, const bool &rightClick)
{
    qDebug() << "rated with: " << id;

    for(auto row : this->getSelectedRows(rightClick))
    {
        QString location = this->getRowData(row)[BaeUtils::TracksCols::URL];

        if (connection.rateTrack(location,id))
        {
            setRating(id);

            QString stars;
            for (int i = 0; i < id; i++)
                stars += "\xe2\x98\x86 ";

            this->item(row, BaeUtils::TracksCols::STARS)->setText(stars);

        } else qDebug() << "rating failed for"<< location;
    }
}

QMap<int, QString> BabeTable::getRowData(const int &row)
{
    QString url = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::URL)).toString();

    if(connection.check_existance(BaeUtils::DBTablesMap[BaeUtils::DBTables::TRACKS],BaeUtils::TracksColsMap[BaeUtils::TracksCols::URL],url))
        return connection.getTrackData(QStringList (url)).first();
    else
    {
        QString track = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::TRACK)).toString();
        QString title = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::TITLE)).toString();
        QString artist = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::ARTIST)).toString();
        QString album = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::ALBUM)).toString();
        QString duration = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::DURATION)).toString();
        QString genre = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::GENRE)).toString();
        QString location = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::URL)).toString();
        QString stars = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::STARS)).toString();
        QString babe = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::BABE)).toString();
        QString releaseDate = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::RELEASE_DATE)).toString();
        QString addDate = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::ADD_DATE)).toString();
        QString played = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::PLAYED)).toString();
        QString art = this->model()->data(this->model()->index(row, BaeUtils::TracksCols::ART)).toString();

        QMap<int, QString> map{{BaeUtils::TracksCols::TRACK,track}, {BaeUtils::TracksCols::TITLE,title}, {BaeUtils::TracksCols::ARTIST,artist},{BaeUtils::TracksCols::ALBUM,album},{BaeUtils::TracksCols::DURATION,duration},{BaeUtils::TracksCols::GENRE,genre},{BaeUtils::TracksCols::URL,location},{BaeUtils::TracksCols::STARS,stars},{BaeUtils::TracksCols::BABE,babe},{BaeUtils::TracksCols::ART,art},{BaeUtils::TracksCols::PLAYED,played},{BaeUtils::TracksCols::RELEASE_DATE,releaseDate},{BaeUtils::TracksCols::ADD_DATE,addDate}};
        return map;
    }
}


void BabeTable::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    QList<QMap<int, QString>> list;
    auto track = getRowData(this->getIndex());
    list<<track;
    qDebug()
            << "BabeTable doubleClicked item<<"
            << track[BaeUtils::TracksCols::URL];

    emit tableWidget_doubleClicked(list);
}

void BabeTable::babeIt_action()
{    QList<QMap<int,QString>> mapList;
     for(auto row: this->getSelectedRows())
         mapList<< this->getRowData(row);

      emit babeIt_clicked(mapList);
}

void BabeTable::sendIt_action(QAction *device)
{
    for(auto row: this->getSelectedRows())
    {

        auto track = this->getRowData(row);

        QString url =track[BaeUtils::TracksCols::URL];
        QString title = track[BaeUtils::TracksCols::TITLE];
        QString artist = track[BaeUtils::TracksCols::ARTIST];

        QString deviceName = device->text().replace("&","");
        QString deviceKey = devices.key(deviceName);

        qDebug()<<"trying to send "<< url << "to : "<< deviceName;
        auto process = new QProcess(this);
        connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            qDebug()<<"processFinished_totally"<<exitCode<<exitStatus;
            nof.notifyUrgent("Song sent to " + deviceName,title +" by "+ artist);

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
    connect(infoForm,SIGNAL(infoModified(QMap<int, QString>)),this,SLOT(itemEdited(QMap<int, QString>)));
    infoForm->show();

}

void BabeTable::itemEdited(QMap<int, QString> map)
{
    qDebug()<<"item changed: " << map[BaeUtils::TracksCols::TITLE];
    this->item(rRow,BaeUtils::TracksCols::TRACK)->setText(map[BaeUtils::TracksCols::TRACK]);
    this->item(rRow,BaeUtils::TracksCols::TITLE)->setText(map[BaeUtils::TracksCols::TITLE]);
    this->item(rRow,BaeUtils::TracksCols::ARTIST)->setText(map[BaeUtils::TracksCols::ARTIST]);
    this->item(rRow,BaeUtils::TracksCols::ALBUM)->setText(map[BaeUtils::TracksCols::ALBUM]);
    this->item(rRow,BaeUtils::TracksCols::GENRE)->setText(map[BaeUtils::TracksCols::GENRE]);

    //connection.insertInto("tracks",column,this->model()->index(newIndex.row(),LOCATION).data().toString(),newIndex.data().toString());

}

void BabeTable::infoIt_action()
{
    QString artist = this->model()->data(this->model()->index(rRow, BaeUtils::TracksCols::ARTIST)).toString();
    QString album = this->model()->data(this->model()->index(rRow, BaeUtils::TracksCols::ALBUM)).toString();
    QString title = this->model()->data(this->model()->index(rRow, BaeUtils::TracksCols::TITLE)).toString();
    emit infoIt_clicked(title, artist, album);
}

void BabeTable::removeIt_action()
{
    qDebug() << "removeIt/right clicked!";
    // int row= this->currentIndex().row();
    qDebug()
            << this->model()->data(this->model()->index(rRow, BaeUtils::TracksCols::URL)).toString();
    //    this->removeRow(rRow);
    emit removeIt_clicked(rRow);
}

void BabeTable::moodIt_action(const QString &color)
{
    if(!color.isEmpty())
    {
        for(auto row : this->getSelectedRows())
        {
            auto url =this->getRowData(row)[BaeUtils::TracksCols::URL];
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
        this->item(row,BaeUtils::TracksCols::TITLE)->setBackgroundColor(coloring);
        if(dark)
        {
            QBrush brush;
            brush.setColor("#fff");
            this->item(row,BaeUtils::TracksCols::TITLE)->setForeground(brush);
        }
        //this->item(row,TITLE)->setTextAlignment(Qt::AlignCenter);
    }
}

void BabeTable::queueIt_action()
{
    QList<QMap<int,QString>> mapList;
    for(auto row : this->getSelectedRows())
        mapList<< this->getRowData(row);

    emit queueIt_clicked(mapList);
}

QList<int> BabeTable::getSelectedRows(const bool &onRightClick)
{
    QList<int> selectedRows;
    QItemSelectionModel *select = this->selectionModel();

    auto selection = select->selectedRows(BaeUtils::TracksCols::TITLE);
    QList<QMap<int, QString>> list;
    for(auto model : selection) selectedRows<<model.row();

    if(!selectedRows.contains(rRow) && onRightClick) return {rRow};
    else return selectedRows;

}

void BabeTable::flushTable()
{
    this->clearContents();
    this->setRowCount(0);
}

QStringList BabeTable::getTableColumnContent(const BaeUtils::TracksCols &column)
{
    QStringList result;
    for (int i = 0; i < this->rowCount(); i++)
        result << this->model()->data(this->model()->index(i, column)).toString();

    return result;
}


QList<QMap<int, QString>> BabeTable::getAllTableContent()
{
    QList<QMap<int,QString>> mapList;

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
        auto trackInfo = track[BaeUtils::TracksCols::TITLE]+"/&/"+track[BaeUtils::TracksCols::ARTIST]+"/&/"+track[BaeUtils::TracksCols::ALBUM];

        if(index.contains(trackInfo) && this->item(row,BaeUtils::TracksCols::TITLE)->icon().name().isEmpty())
        {
            qDebug()<<"index contains:"<<track[BaeUtils::TracksCols::TITLE]<<row;;
            this->removeRow(row);
            emit indexRemoved(row);
            row--;

        }
        else
        {
            qDebug()<<"adding to index:"<<track[BaeUtils::TracksCols::TITLE]<<row;

            index<<trackInfo;
        }


    }

}
