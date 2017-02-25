#include "babetable.h"
#include <QButtonGroup>
#include <QEvent>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidgetAction>
#include <settings.h>
#include <QColorDialog>
#include <notify.h>

BabeTable::BabeTable(QWidget *parent) : QTableWidget(parent) {

    /* connection = new CollectionDB();
  connection->openCollection("../player/collection.db");*/

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this,
            SLOT(on_tableWidget_doubleClicked(QModelIndex)));
    this->setFrameShape(QFrame::NoFrame);
    this->setColumnCount(10);
    this->setHorizontalHeaderLabels({"Track", "Tile", "Artist", "Album", "Genre",
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
    // this->setGridStyle(Qt::PenStyle);
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
    // this->horizontalHeaderItem(0)->setResizeMode(1, QHeaderView::Interactive);
    // this->horizontalHeader()->setHighlightSections(true);
    contextMenu = new QMenu(this);
    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    auto babeIt = new QAction("Babe it \xe2\x99\xa1", contextMenu);
    this->addAction(babeIt);

    auto queueIt = new QAction("Queue", contextMenu);
    this->addAction(queueIt);

    auto infoIt = new QAction("Info + ", contextMenu);
    this->addAction(infoIt);

    auto editIt = new QAction("Edit", contextMenu);
    this->addAction(editIt);

    auto removeIt = new QAction("Remove", contextMenu);
    this->addAction(removeIt);

    QAction *addEntry = contextMenu->addAction("Add to...");
    this->addAction(addEntry);
    playlistsMenu = new QMenu("...");
    addEntry->setMenu(playlistsMenu);

    // auto moodIt = new QAction("Mood..", contextMenu);
    // this->addAction(moodIt);

    /*QAction *moodEntry = contextMenu->addAction("Mood...");
  this->addAction(moodEntry);
  moodMenu = new QMenu("...");
  moodEntry->setMenu(moodMenu);*/
    // passPlaylists();
    // playlistsMenu->addAction("hello rold");

    connect(playlistsMenu, SIGNAL(triggered(QAction *)), this,
            SLOT(addToPlaylist(QAction *)));
    connect(this, SIGNAL(rightClicked(QPoint)), this,
            SLOT(setUpContextMenu(QPoint)));


    connect(babeIt, SIGNAL(triggered()), this, SLOT(babeIt_action()));
    connect(removeIt, SIGNAL(triggered()), this, SLOT(removeIt_action()));
    // connect(moodIt, SIGNAL(triggered()), this, SLOT(moodIt_action()));
    connect(queueIt, SIGNAL(triggered()), this, SLOT(queueIt_action()));



    QButtonGroup *bg = new QButtonGroup(contextMenu);
    bg->addButton(fav1, 1);
    bg->addButton(fav2, 2);
    bg->addButton(fav3, 3);
    bg->addButton(fav4, 4);
    bg->addButton(fav5, 5);
    // connect(fav1,SIGNAL(enterEvent(QEvent)),this,hoverEvent());
    connect(bg, SIGNAL(buttonClicked(int)), this, SLOT(rateGroup(int)));
    auto gr = new QWidget();
    auto ty = new QHBoxLayout();
    ty->addWidget(fav1);
    ty->addWidget(fav2);
    ty->addWidget(fav3);
    ty->addWidget(fav4);
    ty->addWidget(fav5);

    gr->setLayout(ty);

    QWidgetAction *chkBoxAction = new QWidgetAction(contextMenu);
    chkBoxAction->setDefaultWidget(gr);

    this->addAction(chkBoxAction);

    auto moods = new QWidget();
    auto moodsLayout = new QHBoxLayout();
    QButtonGroup *moodGroup = new QButtonGroup(contextMenu);
    connect(moodGroup, SIGNAL(buttonClicked(int)), this, SLOT(moodTrack(int)));
    for(int i=0; i<5; i++)
    {
        auto  *colorTag = new QToolButton();
        colorTag->setIconSize(QSize(10,10));
        colorTag->setFixedSize(16,16);
        // colorTag->setAutoRaise(true);
        colorTag->setStyleSheet(QString("QToolButton { background-color: %1;}").arg(colors.at(i)));
        moodGroup->addButton(colorTag,i);
        moodsLayout->addWidget(colorTag);
    }
    moods->setLayout(moodsLayout);

    QWidgetAction *moodsAction = new QWidgetAction(contextMenu);
    moodsAction->setDefaultWidget(moods);

    this->addAction(moodsAction);
}


void BabeTable::moodTrack(int color)
{

    moodIt_action(colors.at(color));
}

void BabeTable::addToPlaylist(QAction *action) {

    QString playlist = action->text().replace("&", "");
    QString location =
            this->model()->data(this->model()->index(row, LOCATION)).toString();

    if (playlist.contains("Create new...")) {
        qDebug() << "trying to create a new playlistsssss" << playlist;

        emit createPlaylist_clicked();
    } else {
        populatePlaylist({location}, playlist);
    }
}

void BabeTable::populatePlaylist(QStringList urls, QString playlist) {

    for (auto location : urls) {
        if (connection->checkQuery("SELECT * FROM tracks WHERE location = \"" +
                                   location + "\"")) {
            // ui->fav_btn->setIcon(QIcon::fromTheme("face-in-love"));
            qDebug() << "Song to add: " << location << " to: " << playlist;

            QSqlQuery query = connection->getQuery(
                        "SELECT * FROM tracks WHERE location = \"" + location + "\"");

            QString list;
            while (query.next())
                list = query.value(PLAYLIST).toString();
            list += " " + playlist;
            // qDebug()<<played;

            if (connection->insertInto("tracks", "playlist", location, list)) {
                // ui->fav_btn->setIcon(QIcon(":Data/data/love-amarok.svg"));
                qDebug() << list;
            }
        }
    }
}

BabeTable::~BabeTable() { delete this; }

void BabeTable::passPlaylists() {}

void BabeTable::enterEvent(QEvent *event) {
    // qDebug()<<"entered the playlist";
    Q_UNUSED(event);
    // emit enteredTable();
}

void BabeTable::leaveEvent(QEvent *event) {
    // qDebug()<<"left the playlist";
    Q_UNUSED(event);
    //  emit leftTable();
}

void BabeTable::passStyle(QString style) { this->setStyleSheet(style); }

void BabeTable::addRow(QStringList list) {
    QString track,title,artist,album,genre,location,stars,babe,art,played,playlist;

    track= list.at(TRACK);
    title=list.at(TITLE);
    artist=list.at(ARTIST);
    album=list.at(ALBUM);
    genre=list.at(GENRE);
    location=list.at(LOCATION);
    stars=list.at(STARS);
    babe=list.at(BABE);
    art=list.at(ART);
    played=list.at(PLAYED);
    playlist=list.at(PLAYLIST);

    this->insertRow(this->rowCount());

    this->setItem(this->rowCount() - 1, TRACK, new QTableWidgetItem(track));
    this->setItem(this->rowCount() - 1, TITLE, new QTableWidgetItem(title));
    this->setItem(this->rowCount() - 1, ARTIST, new QTableWidgetItem(artist));
    this->setItem(this->rowCount() - 1, ALBUM, new QTableWidgetItem(album));
    this->setItem(this->rowCount() - 1, GENRE, new QTableWidgetItem(genre));
    this->setItem(this->rowCount() - 1, LOCATION, new QTableWidgetItem(location));
    this->setItem(this->rowCount() - 1, STARS, new QTableWidgetItem(stars));
    this->setItem(this->rowCount() - 1, BABE, new QTableWidgetItem(babe));
    this->setItem(this->rowCount() - 1, ART, new QTableWidgetItem(art));
    this->setItem(this->rowCount() - 1, PLAYED, new QTableWidgetItem(played));
    this->setItem(this->rowCount() - 1, PLAYLIST, new QTableWidgetItem(playlist));
}

void BabeTable::populateTableView(QString indication) {
    // this->clearContents();
    this->setSortingEnabled(false);
    QSqlQuery query = connection->getQuery(indication);
    bool missingDialog = false;
    QStringList missingFiles;
    qDebug() << "ON POPULATE:"<<indication;

    if(connection->checkQuery(indication))
    {

        while (query.next()) {

            if (!QFileInfo(query.value(LOCATION).toString()).exists()) {
                qDebug() << "That file doesn't exists anymore: "
                         << query.value(LOCATION).toString();
                missingFiles << query.value(LOCATION).toString();
                missingDialog = true;
            } else {
                this->insertRow(this->rowCount());

                auto *track = new QTableWidgetItem(query.value(TRACK).toString());
                this->setItem(this->rowCount() - 1, TRACK, track);

                auto *title = new QTableWidgetItem(query.value(TITLE).toString());
                // title->setFlags(title->flags() & ~Qt::ItemIsEditable);
                this->setItem(this->rowCount() - 1, TITLE, title);

                auto *artist = new QTableWidgetItem(query.value(ARTIST).toString());
                this->setItem(this->rowCount() - 1, ARTIST, artist);

                // qDebug()<<query.value(2).toString();
                auto *album = new QTableWidgetItem(query.value(ALBUM).toString());
                this->setItem(this->rowCount() - 1, ALBUM, album);

                auto *genre = new QTableWidgetItem(query.value(GENRE).toString());
                this->setItem(this->rowCount() - 1, GENRE, genre);

                auto *location = new QTableWidgetItem(query.value(LOCATION).toString());
                this->setItem(this->rowCount() - 1, LOCATION, location);

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
                    rating =
                            "\xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 \xe2\x98\x86 ";
                    break;
                }

                if (query.value(BABE).toInt() == 1)
                    rating = "\xe2\x99\xa1 ";

                auto *stars = new QTableWidgetItem(rating);
                this->setItem(this->rowCount() - 1, STARS, stars);

                QString bb;
                switch (query.value((BABE)).toInt()) {
                case 0:
                    bb = " ";
                    break;
                case 1:
                    bb = "\xe2\x99\xa1 ";
                    break;
                }

                auto *babe = new QTableWidgetItem(bb);
                this->setItem(this->rowCount() - 1, BABE, babe);

                auto *art = new QTableWidgetItem(query.value(ART).toString());
                this->setItem(this->rowCount() - 1, ART, art);

                auto *played = new QTableWidgetItem(query.value(PLAYED).toString());
                this->setItem(this->rowCount() - 1, PLAYED, played);
            }
        }
        // this->sortByColumn(1);

        if (missingDialog) {
            QString parentDir;
            // QMessageBox::about(this,"Removing missing
            // files",missingFiles.join("\n"));

            auto *nof = new Notify();
            nof->notifyUrgent("Removing missing files...",missingFiles.join("\n"));

            for (auto file_r : missingFiles)
            {
                parentDir=QFileInfo(QFileInfo(file_r)).dir().path();
                if (!QFileInfo(parentDir).exists())
                {
                    connection->removePath(parentDir);
                    qDebug()<<"the parent file doesn't exists"<<parentDir;
                }else
                {

                    connection->removePath(file_r);

                    qDebug() << "deleted from db: " << file_r;
                }



            }
            connection->setCollectionLists();
            connection->cleanCollectionLists();


        }

        this->setSortingEnabled(true);
        emit finishedPopulating();
    }else
    {
        qDebug()<<"Error: the query didn't pass"<<indication;
    }
    //
    // this->sortByColumn(1,Qt::AscendingOrder);
    /*for (Track track : collection.getTracks() )
  {
   this->insertRow(this->rowCount());
   auto *title= new QTableWidgetItem( QString::fromStdString(track.getTitle()));
   //title->setFlags(title->flags() & ~Qt::ItemIsEditable);

   this->setItem(this->rowCount()-1, TITLE, title);

   auto *artist= new QTableWidgetItem(
  QString::fromStdString(track.getArtist()));
   this->setItem(this->rowCount()-1, ARTIST, artist);

   auto *album= new QTableWidgetItem( QString::fromStdString(track.getAlbum()));
   this->setItem(this->rowCount()-1, ALBUM, album);

   auto *location= new QTableWidgetItem(
  QString::fromStdString(track.getLocation()));
   this->setItem(this->rowCount()-1, LOCATION, location);

  }*/
}

void BabeTable::setRating(int rate) {
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

void BabeTable::setTableOrder(int column, int order) {
    if (order == DESCENDING) {
        this->sortByColumn(column, Qt::DescendingOrder);
    } else if (order == ASCENDING) {
        this->sortByColumn(column, Qt::AscendingOrder);
    }
}

void BabeTable::setVisibleColumn(int column) {
    if (column == LOCATION) {
        this->showColumn(LOCATION);
    } else if (column == STARS) {
        this->showColumn(STARS);
    } else if (column == BABE) {
        this->showColumn(BABE);
    } else if (column == ALBUM) {
        this->showColumn(ALBUM);
    }
}

void BabeTable::setUpContextMenu(QPoint pos)

{
    qDebug() << "setUpContextMenu";
    playlistsMenu->clear();

    for (auto playlist : connection->getPlaylists()) {
        playlistsMenu->addAction(playlist);
    }
    // playlistsMenu->addAction("Create new...");
    int rate = 0;
    bool babe = false;
    row = this->indexAt(pos).row();

    // row= this->currentIndex().row(), rate;

    QString url =
            this->model()->data(this->model()->index(row, LOCATION)).toString();
    //
    QSqlQuery query = connection->getQuery(
                "SELECT * FROM tracks WHERE location = \"" + url + "\"");

    while (query.next()) {
        rate = query.value(STARS).toInt();
        babe = query.value(BABE).toInt() == 1 ? true : false;

        qDebug() << "se llamó a menu contextual con url: " << url
                 << "rated: " << rate;
    }

    setRating(rate);
    if (babe)
        this->actions().at(0)->setText("Un-Babe it");
    else
        this->actions().at(0)->setText("Babe it");
}

QStringList BabeTable::getPlaylistMenus() {

    playlistsMenus.clear();
    for (auto playlist : connection->getPlaylists()) {
        playlistsMenus << playlist;
    }
    return playlistsMenus;
}

void BabeTable::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Return: {

    QList<QStringList> list;



        list<<getRowData(this->currentIndex().row());
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

        qDebug()
                << this->model()->data(this->model()->index(row, LOCATION)).toString();

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

        qDebug()
                << this->model()->data(this->model()->index(row, LOCATION)).toString();
        break;
    }
    default: {
        QTableWidget::keyPressEvent(event);
        break;
    }
    }
}

void BabeTable::mousePressEvent(QMouseEvent *evt) {
    // QTableView::mouseReleaseEvent( event );

    if (evt->button() == Qt::RightButton) {
        qDebug() << "table right clicked";
        emit rightClicked(evt->pos());
    } else {
        QTableWidget::mousePressEvent(evt);
    }
}

void BabeTable::rateGroup(int id) {
    qDebug() << "rated with: " << id;
    // int row= this->currentIndex().row();
    QString location =
            this->model()->data(this->model()->index(row, LOCATION)).toString();

    QSqlQuery query = connection->getQuery(
                "SELECT * FROM tracks WHERE location = \"" + location + "\"");

    int rate = 0;

    while (query.next())
        rate = query.value(STARS).toInt();

    if (connection->check_existance("tracks", "location", location)) {
        if (connection->insertInto("tracks", "stars", location, id)) {
            setRating(id);

            // this->model()->data(this->model()->index(row,1)).
        }
        //qDebug() << "rating the song of rowffff: " << row;

        QString stars;
        for (int i = 0; i < id; i++) {
            stars += "\xe2\x98\x86 ";
        }
        this->item(row, STARS)->setText(stars);

        if (id > 0 && rate < 5) {
            QString title =
                    this->model()->data(this->model()->index(row, TITLE)).toString();
            QString artist =
                    this->model()->data(this->model()->index(row, ARTIST)).toString();
            QString album =
                    this->model()->data(this->model()->index(row, ALBUM)).toString();
            QString star =
                    this->model()->data(this->model()->index(row, STARS)).toString();
            QString babe =
                    this->model()->data(this->model()->index(row, BABE)).toString();

            qDebug() << "rated and trying to add to favs";
            emit songRated({title, artist, album, location, star, babe});
        } else {
            qDebug() << "rated and trying to add to favs failed";
        }
        // this->update();
    } else {
    }
}

QStringList BabeTable::getRowData(int row)
{
    QStringList file;

    file<< this->model()
            ->data(
                this->model()->index(row, TRACK)).toString();
    file<< this->model()
            ->data(
                this->model()->index(row, TITLE)).toString();
    file<< this->model()
            ->data(
                this->model()->index(row, ARTIST)).toString();
    file<< this->model()
            ->data(
                this->model()->index(row, ALBUM)).toString();
    file<< this->model()
            ->data(
                this->model()->index(row, GENRE)).toString();
    file<< this->model()
             ->data(
                 this->model()->index(row, LOCATION))
             .toString();
    file<< this->model()
            ->data(
                this->model()->index(row, STARS)).toString();
    file<< this->model()
            ->data(
                this->model()->index(row, BABE)).toString();
    file<< this->model()
            ->data(
                this->model()->index(row, ART)).toString();
    file<< this->model()
            ->data(
                this->model()->index(row, PLAYED)).toString();
    file<< this->model()
            ->data(
                this->model()->index(row, PLAYLIST)).toString();

    qDebug() << this->model()
                ->data(this->model()->index(this->currentIndex().row(),
                                            LOCATION))
                .toString();

    return file;
}
void BabeTable::allowDrag()
{

}

void BabeTable::on_tableWidget_doubleClicked(const QModelIndex &index) {

    QList<QStringList> list;
    qDebug()
            << "BabeTable doubleClicked item<<"
            << index.sibling(this->currentIndex().row(), LOCATION).data().toString();

    list<<getRowData(this->currentIndex().row());

    emit tableWidget_doubleClicked(list);
    //emit tableWidget_doubleClicked(index);

    /* playlist.add(files);
  updateList();

  if(shuffle) shufflePlaylist();*/
}

void BabeTable::babeIt_action() {
    qDebug() << "right clicked!";
    // int row= this->currentIndex().row();
    qDebug()
            << this->model()->data(this->model()->index(row, LOCATION)).toString();
    QList<QStringList> list;
    list<<getRowData(row);
    emit babeIt_clicked(list);
}

void BabeTable::removeIt_action()
{
    qDebug() << "removeIt/right clicked!";
  // int row= this->currentIndex().row();
    qDebug()
            << this->model()->data(this->model()->index(row, LOCATION)).toString();
    this->removeRow(row);
   emit removeIt_clicked(row);
}

void BabeTable::moodIt_action(QString color) {
    qDebug() << "right clicked!";
    // int row= this->currentIndex().row();
    qDebug()
            << this->model()->data(this->model()->index(row, LOCATION)).toString();

    // QColor color = QColorDialog::getColor(Qt::black, this, "Pick a Mood",  QColorDialog::DontUseNativeDialog);
    qDebug()<< color;

    if(!color.isEmpty())
    {
        QSqlQuery query;
        query.prepare("UPDATE tracks SET art = (:art) WHERE location = (:location)" );
        //query.prepare("SELECT * FROM "+tableName+" WHERE "+searchId+" = (:search)");
        query.bindValue(":art",  color);
        query.bindValue(":location", this->model()->data(this->model()->index(row, LOCATION)).toString());

        if(query.exec())
        {
            qDebug()<<"Art[color] inserted into DB"<< color;
            emit moodIt_clicked(color);

        }else
        {
            qDebug()<<"COULDN'T insert art[color] into DB";
        }
    }


    //emit moodIt_clicked(this->model()->data(this->model()->index(row, LOCATION)).toString(),color.name());
    /*emit babeIt_clicked(
      {this->model()->data(this->model()->index(row, LOCATION)).toString()});*/
}


void BabeTable::queueIt_action()
{
    qDebug() << "queueIt clicked!";
    // int row= this->currentIndex().row();
    QString url = this->model()->data(this->model()->index(row, LOCATION)).toString();
    qDebug()<<url;
    emit queueIt_clicked(url);

}

void BabeTable::flushTable() {
    this->clearContents();
    this->setRowCount(0);
}

void BabeTable::passCollectionConnection(CollectionDB *con) {
    connection = con;
}

QStringList BabeTable::getTableContent(int column) {
    QStringList result;
    for (int i = 0; i < this->rowCount(); i++) {
        result << this->model()->data(this->model()->index(i, column)).toString();
    }

    return result;
}


QList<QStringList> BabeTable::getAllTableContent() {
    QList<QStringList> result;


    for (int i = 0; i < this->rowCount(); i++) {

        result<<getRowData(i);
    }

    return result;
}
