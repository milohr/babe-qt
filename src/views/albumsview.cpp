#include "albumsview.h"

#include "../widget_models/babealbum.h"
#include "../widget_models/babetable.h"
#include "../widget_models/babegrid.h"
#include "../views/babewindow.h"
#include "../db/collectionDB.h"


AlbumsView::AlbumsView(const bool &extraList, QWidget *parent) :
    QWidget(parent), extraList(extraList)
{

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto layout = new QGridLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    //    layout->setContentsMargins(0,6,0,6);
//    this->BabeWindow::connection->openDB();

    this->setAcceptDrops(false);
    this->grid = new BabeGrid(BAE::MEDIUM_ALBUM_FACTOR, BAE::AlbumSizeHint::MEDIUM_ALBUM,4,this);
    connect(grid, &BabeGrid::albumReady, [this](){albumLoader.next();});
    connect(&albumLoader, &AlbumLoader::albumReady, this, &AlbumsView::addAlbum);
    connect(&albumLoader, &AlbumLoader::finished, [this]()
    {
        this->grid->sortItems(Qt::AscendingOrder);
    });

    connect(grid, &BabeGrid::albumClicked, this, &AlbumsView::showAlbumInfo);

    connect(grid, &BabeGrid::albumDoubleClicked, [this](const BAE::DB &albumMap)
    {
        emit this->albumDoubleClicked(albumMap);
        hideAlbumFrame();
    });

    connect(grid, &BabeGrid::playAlbum, [this](const BAE::DB &albumMap)
    {
        emit this->playAlbum(albumMap);
    });

    connect(grid ,&BabeGrid::babeAlbum, [this](const BAE::DB &albumMap)
    {
        emit this->babeAlbum(albumMap);
    });

    connect(grid, &BabeGrid::dragAlbum, this, &AlbumsView::hideAlbumFrame);


    this->albumTable = new BabeTable(this);
    this->albumTable->setFrameShape(QFrame::NoFrame);
    this->albumTable->showColumn(static_cast<int>(BAE::KEY::TRACK));
    this->albumTable->showColumn(static_cast<int>(BAE::KEY::STARS));
    this->albumTable->hideColumn(static_cast<int>(BAE::KEY::ARTIST));
    this->albumTable->hideColumn(static_cast<int>(BAE::KEY::ALBUM));

    auto albumBox = new QGridLayout;
    albumBox->setContentsMargins(0,0,0,0);
    albumBox->setSpacing(0);

    this->albumBox_frame = new QFrame(this);
    albumBox_frame->setFrameShape(QFrame::StyledPanel);
    albumBox_frame->setFrameShadow(QFrame::Sunken);
    this->albumBox_frame->setLayout(albumBox);


    this->cover = new BabeAlbum({{BAE::KEY::ARTWORK, ":Data/data/cover.svg"}}, BAE::AlbumSizeHint::MEDIUM_ALBUM, 0, true, this);
    connect(this->cover,&BabeAlbum::playAlbum,[this] (const BAE::DB &info) { emit this->playAlbum(info); });
    connect(this->cover, &BabeAlbum::babeAlbum, this, &AlbumsView::babeAlbum);
    this->cover->showTitle(false);

    this->closeBtn = new QToolButton(cover);
    connect(this->closeBtn, &QToolButton::clicked, this, &AlbumsView::hideAlbumFrame);
    this->closeBtn->setGeometry(2,2,16,16);
    this->closeBtn->setIcon(QIcon::fromTheme("tab-close"));
    this->closeBtn->setAutoRaise(true);
    this->closeBtn->setToolTip("Close");

    this->expandBtn = new QToolButton(cover);
    connect(expandBtn,&QToolButton::clicked, this, &AlbumsView::expandList);
    this->expandBtn->setGeometry(static_cast<int>(cover->getSize())-18,2,16,16);
    this->expandBtn->setIcon(QIcon(":/Data/data/icons/artists_selected.svg"));
    this->expandBtn->setAutoRaise(true);

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMaximumWidth(1);

    if(this->extraList)
    {
        this->artistList=new QListWidget(this);
        connect(this->artistList,SIGNAL(clicked(QModelIndex)),this,SLOT(filterAlbum(QModelIndex)));
        this->artistList->setFrameShape(QFrame::NoFrame);
        this->artistList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->artistList->setMaximumWidth(static_cast<int>(cover->getSize()));
        this-> artistList->setAlternatingRowColors(true);
        // artistList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
        // artistList->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);

        albumBox->addWidget(this->cover,0,0,Qt::AlignTop);
        albumBox->addWidget(this->artistList,1,0,Qt::AlignLeft);
        albumBox->addWidget(line,0,1,2,1, Qt::AlignLeft);
        albumBox->addWidget(this->albumTable,0,2,2,1);

    }else
    {
        albumBox->addWidget(this->cover,0,0,Qt::AlignTop);
        albumBox->addWidget(line,0,1,Qt::AlignLeft);
        albumBox->addWidget(this->albumTable,0,2);

    }

    //    auto spacer = new QWidget(this);
    //    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    //    spacer->setFixedHeight(15);
    //    spacer->setAutoFillBackground(false);
    //    spacer->setPalette(this->grid->palette());
    //spacer->setStyleSheet("QWidget{background-color:transparent;");

    auto *splitter = new QSplitter(parent);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Vertical);
    splitter->setHandleWidth(6);
    splitter->addWidget(this->grid);
    splitter->addWidget(this->albumBox_frame);

    layout->addWidget(splitter);

    albumBox_frame->hide();

    splitter->setSizes({0,0});
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    this->setLayout(layout);
}

AlbumsView::~AlbumsView()
{
    qDebug()<<"DELETING ALBUMSVIEW";
}


void AlbumsView::expandList()
{
    auto album = cover->getAlbum();
    auto artist = cover->getArtist();
    emit expandTo(artist);
}


void AlbumsView::hideAlbumFrame()
{
    albumTable->flushTable();
    albumBox_frame->hide();
}

void AlbumsView::filterAlbum(QModelIndex index)
{
    QString album = index.data().toString();
    qDebug()<<album;

    albumTable->flushTable();
    albumTable->populateTableView(BabeWindow::connection->getAlbumTracks(album, cover->getArtist()));
    cover->setTitle(cover->getArtist(), album);

    cover->putPixmap(BabeWindow::connection->getAlbumArt(album, cover->getArtist()));
}


void AlbumsView::albumHover() { }

void  AlbumsView::flushView()
{
    this->grid->flushGrid();
    this->hideAlbumFrame();
}

void AlbumsView::populate(const QString &query)
{
    qDebug()<<"POPULATING ALBUMS WAS CALLED";
    albumLoader.requestAlbums(query);
}

void AlbumsView::populate(const DB_LIST &albums)
{
    qDebug()<<"POPULATING ALBUMS WAS CALLED";
    albumLoader.requestAlbums(albums);
}
void AlbumsView::addAlbum(const BAE::DB &albumMap)
{
    this->grid->addAlbum(albumMap);
}

void AlbumsView::filter(const BAE::DB_LIST &filter, const BAE::KEY &type)
{
    hide_all(true);
    this->hideAlbumFrame();

    QList<QListWidgetItem*> matches;

    for(auto result : filter)
        switch(type)
        {
        case BAE::KEY::ALBUM:
            matches<<grid->findItems(result[BAE::KEY::ALBUM]+" "+result[BAE::KEY::ARTIST], Qt::MatchFlag::MatchContains);
            break;
        case BAE::KEY::ARTIST:
            matches<<grid->findItems(result[BAE::KEY::ARTIST], Qt::MatchFlag::MatchContains);
            break;
        default: break;
        }

    for(QListWidgetItem* item : matches)
        item->setHidden(false);
}

void AlbumsView::hide_all(bool state)
{
    for(int row = 0; row < grid->count(); row++ )
        grid->item(row)->setHidden(state);
}

void AlbumsView::populateExtraList(const QStringList &albums)
{
    artistList->clear();

    qDebug()<<"ON POPULATE EXTRA LIST:";

    for(auto album : albums)
    {
        auto item = new QListWidgetItem;
        item->setText(album);
        item->setTextAlignment(Qt::AlignCenter);
        artistList->addItem(item);
    }
}

void AlbumsView::showAlbumInfo(const BAE::DB &albumMap)
{
    albumBox_frame->setVisible(true);
    albumTable->flushTable();

    auto type = BAE::albumType(albumMap);

    if(type== BAE::TABLE::ALBUMS)
    {
        auto artist = albumMap[BAE::KEY::ARTIST];
        auto album = albumMap[BAE::KEY::ALBUM];

        cover->setTitle(artist,album);
        expandBtn->setToolTip("View "+ cover->getArtist());

        albumTable->populateTableView(BabeWindow::connection->getAlbumTracks(album, artist));

        auto art = BabeWindow::connection->getAlbumArt(album,artist);
        art = art.isEmpty()? BabeWindow::connection->getArtistArt(artist) : art;

        cover->putPixmap(art);

        if(!art.isEmpty()) cover->putPixmap(art);
        else cover->putDefaultPixmap();

    }else if(type== BAE::TABLE::ARTISTS)
    {

        auto artist =albumMap[BAE::KEY::ARTIST];
        cover->setTitle(artist);

        albumTable->populateTableView(BabeWindow::connection->getArtistTracks(artist));
        auto art = BabeWindow::connection->getArtistArt(artist);
        if(!art.isEmpty()) cover->putPixmap(art);
        else cover->putDefaultPixmap();

        if(extraList)
            populateExtraList(BabeWindow::connection->getArtistAlbums(artist));

    }

}
