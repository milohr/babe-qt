#include "albumsview.h"


AlbumsView::AlbumsView(const bool &extraList, QWidget *parent) :
    QWidget(parent), extraList(extraList)
{


    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto layout = new QGridLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    this->setAcceptDrops(false);
    this->grid = new GridView(Bae::MEDIUM_ALBUM_FACTOR,Bae::AlbumSizeHint::MEDIUM_ALBUM,this);
    connect(grid,&GridView::albumReady,[this](){albumLoader.next();});
    connect(&albumLoader, &AlbumLoader::albumReady,this, &AlbumsView::addAlbum);
    connect(&albumLoader, &AlbumLoader::finished,[this]()
    {
        this->grid->sortItems(Qt::AscendingOrder);
    });

    connect(grid,&GridView::albumClicked,this,&AlbumsView::showAlbumInfo);

    connect(grid,&GridView::albumDoubleClicked,[this](const Bae::DB &albumMap)
    {
         emit this->albumDoubleClicked(albumMap);
         hideAlbumFrame();
    });

    connect(grid,&GridView::playAlbum,[this](const Bae::DB &albumMap)
    {
         emit this->playAlbum(albumMap);
    });

    connect(grid,&GridView::babeAlbum,[this](const Bae::DB &albumMap)
    {
         emit this->babeAlbum(albumMap);
    });

    connect(grid,&GridView::dragAlbum,this,&AlbumsView::hideAlbumFrame);

    auto utilsLayout = new QHBoxLayout;
    utilsLayout->setContentsMargins(0,0,0,0);
    utilsLayout->setSpacing(0);

    this->utilsFrame = new QFrame(this);
    this->utilsFrame->setLayout(utilsLayout);
    this->utilsFrame->setFrameShape(QFrame::NoFrame);
    this->utilsFrame->setFrameShadow(QFrame::Plain);
    this->utilsFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    this->albumTable = new BabeTable(this);
    this->albumTable->setFrameShape(QFrame::NoFrame);
    this->albumTable->showColumn(static_cast<int>(Bae::DBCols::TRACK));
    this->albumTable->showColumn(static_cast<int>(Bae::DBCols::STARS));
    this->albumTable->hideColumn(static_cast<int>(Bae::DBCols::ARTIST));
    this->albumTable->hideColumn(static_cast<int>(Bae::DBCols::ALBUM));

    auto albumBox = new QGridLayout;
    albumBox->setContentsMargins(0,0,0,0);
    albumBox->setSpacing(0);

    this->albumBox_frame = new QWidget(this);
    this->albumBox_frame->setLayout(albumBox);

    this->line_h = new QFrame(this);
    this->line_h->setFrameShape(QFrame::HLine);
    this->line_h->setFrameShadow(QFrame::Plain);
    this->line_h->setMaximumHeight(1);

    this->cover = new Album(this);
    connect(this->cover,&Album::playAlbum,[this] (const Bae::DB &info) { emit this->playAlbum(info); });
    connect(this->cover,&Album::babeAlbum,this,&AlbumsView::babeAlbum);
    this->cover->createAlbum({{Bae::DBCols::ARTWORK,":Data/data/cover.svg"}},Bae::AlbumSizeHint::MEDIUM_ALBUM,0,true);
    this->cover->showTitle(false);

    this->closeBtn = new QToolButton(cover);
    connect(this->closeBtn,&QToolButton::clicked,this,&AlbumsView::hideAlbumFrame);
    this->closeBtn->setGeometry(2,2,16,16);
    this->closeBtn->setIcon(QIcon::fromTheme("tab-close"));
    this->closeBtn->setAutoRaise(true);
    this->closeBtn->setToolTip("Close");

    this->expandBtn = new QToolButton(cover);
    connect(expandBtn,&QToolButton::clicked,this,&AlbumsView::expandList);
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

//    splitter->addWidget(spacer);
    splitter->addWidget(this->grid);
    splitter->addWidget(this->line_h);
    splitter->addWidget(this->albumBox_frame);

    layout->addWidget(splitter);

    albumBox_frame->hide(); line_h->hide();

    splitter->setSizes({0,0,0});
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(2, 0);

    this->setLayout(layout);
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
    line_h->hide();
}

void AlbumsView::filterAlbum(QModelIndex index)
{
    QString album = index.data().toString();
    qDebug()<<album;

    albumTable->flushTable();
    albumTable->populateTableView(connection.getAlbumTracks(album,cover->getArtist()));
    cover->setTitle(cover->getArtist(),album);

    cover->putPixmap(connection.getAlbumArt(album,cover->getArtist()));

}


void AlbumsView::albumHover() { }

void  AlbumsView::flushView()
{
    this->grid->flushGrid();
    this->hideAlbumFrame();
}



void AlbumsView::populateAlbumsView(const Bae::DBTables &type,QSqlQuery &query)
{
    qDebug()<<"POPULATING ALBUMS WAS CALLED";
    albumLoader.requestAlbums(type, query.lastQuery());
}

void AlbumsView::addAlbum(const Bae::DB &albumMap)
{
    this->grid->addAlbum(albumMap);
}


void AlbumsView::filter(const Bae::DB_LIST &filter, const Bae::DBCols &type)
{

    hide_all(true);
    this->hideAlbumFrame();

    QList<QListWidgetItem*> matches;

    for(auto result : filter)
    {

        switch(type)
        {
        case Bae::DBCols::ALBUM:
            matches<<grid->findItems(result[Bae::DBCols::ALBUM]+" "+result[Bae::DBCols::ARTIST], Qt::MatchFlag::MatchContains);
            break;
        case Bae::DBCols::ARTIST:
            matches<<grid->findItems(result[Bae::DBCols::ARTIST], Qt::MatchFlag::MatchContains);
            break;
        default: break;
        }
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


void AlbumsView::showAlbumInfo(const Bae::DB &albumMap)
{
    albumBox_frame->setVisible(true);
    line_h->setVisible(true);
    albumTable->flushTable();

    auto type = Bae::albumType(albumMap);

    if(type== Bae::DBTables::ALBUMS)
    {
        auto artist = albumMap[Bae::DBCols::ARTIST];
        auto album = albumMap[Bae::DBCols::ALBUM];

        cover->setTitle(artist,album);
        expandBtn->setToolTip("View "+ cover->getArtist());

        albumTable->populateTableView(connection.getAlbumTracks(album, artist));

        auto art = connection.getAlbumArt(album,artist);
        art = art.isEmpty()? connection.getArtistArt(artist) : art;

        cover->putPixmap(art);

        if(!art.isEmpty()) cover->putPixmap(art);
        else cover->putDefaultPixmap();

    }else if(type== Bae::DBTables::ARTISTS)
    {

        auto artist =albumMap[Bae::DBCols::ARTIST];
        cover->setTitle(artist);

        albumTable->populateTableView(connection.getArtistTracks(artist));
        auto art = connection.getArtistArt(artist);
        if(!art.isEmpty()) cover->putPixmap(art);
        else cover->putDefaultPixmap();

        if(extraList)
            populateExtraList(connection.getArtistAlbums(artist));

    }

}
