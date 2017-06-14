#include "albumsview.h"



AlbumsView::AlbumsView(bool extraList, QWidget *parent) :
    QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto layout = new QGridLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    albumSize = BaeUtils::getWidgetSizeHint(BaeUtils::MEDIUM_ALBUM_FACTOR,BaeUtils::MEDIUM_ALBUM);
    this->setAcceptDrops(false);
    grid = new QListWidget(this);
    grid->installEventFilter(this);
    grid->setObjectName("grid");
    grid->setMinimumHeight(albumSize);
    grid->setViewMode(QListWidget::IconMode);
    grid->setResizeMode(QListWidget::Adjust);
    //    grid->setUniformItemSizes(true);
    grid->setWrapping(true);
    grid->setAcceptDrops(false);
    grid->setDragDropMode(QAbstractItemView::DragOnly);
    grid->setFrameShape(QFrame::NoFrame);
    grid->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );
    grid->setSizeAdjustPolicy(QListWidget::AdjustToContentsOnFirstShow);
    //grid->setStyleSheet("QListWidget {background:#2E2F30; border:1px solid black; border-radius: 2px; }");
    grid->setStyleSheet("QListWidget,QListWidget::item:selected,QListWidget::item:selected:active {background:transparent; color:transparent; }");
    grid->setGridSize(QSize(albumSize+25,albumSize+25));


    connect(this, &AlbumsView::createdAlbum,[this](Album *artwork)
    {
        albumsList.push_back(artwork);

        if(artwork->getAlbum().isEmpty()) connect(artwork,&Album::albumCoverClicked,this,&AlbumsView::getArtistInfo);
        else  connect(artwork,&Album::albumCoverClicked,this,&AlbumsView::getAlbumInfo);


        auto shadow = new QGraphicsDropShadowEffect();
        shadow->setColor(QColor(0, 0, 0, 140));
        shadow->setBlurRadius(9);
        shadow->setOffset(3,5);

        artwork->setGraphicsEffect(shadow);
        artwork->borderColor=false;
        artwork->setUpMenu();

        connect(artwork,&Album::albumCoverDoubleClicked, [this] (QMap<int, QString> info)
        {
            emit albumDoubleClicked(info);
            hideAlbumFrame();
        });
        connect(artwork,&Album::playAlbum, [this] (QMap<int,QString> info) { emit this->playAlbum(info); });
        connect(artwork,&Album::changedArt,this,&AlbumsView::changedArt_cover);
        connect(artwork,&Album::babeAlbum_clicked,this,&AlbumsView::babeAlbum);
        connect(artwork,&Album::albumDragStarted, this, &AlbumsView::hideAlbumFrame);
        connect(artwork,&Album::albumCoverEnter,[artwork,shadow,this]()
        {
            if(hiddenLabels) artwork->showTitle(true);
            shadow->setColor(QColor(0, 0, 0, 180));
            shadow->setOffset(2,3);
        });
        connect(artwork, &Album::albumCoverLeave,[artwork,shadow,this]()
        {
            if(hiddenLabels) artwork->showTitle(false);
            shadow->setColor(QColor(0, 0, 0, 140));
            shadow->setOffset(3,5);
        });


        auto item = new QListWidgetItem();


        itemsList.push_back(item);
        item->setSizeHint(QSize(artwork->getSize(),artwork->getSize()));

        if(artwork->getAlbum().isEmpty()) item->setText(artwork->getArtist());
        else item->setText(artwork->getAlbum()+" "+artwork->getArtist());

        item->setTextAlignment(Qt::AlignCenter);
        grid->addItem(item);
        grid->setItemWidget(item,artwork);

    });

    grid->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);



    order  = new QAction("Go Descending");
    connect(order, &QAction::triggered,this,&AlbumsView::orderChanged);
    grid->addAction(order);
    auto hideLabels = new QAction("Hide titles",this);
    grid->addAction(hideLabels);
    connect(hideLabels, &QAction::triggered,[hideLabels,this]()
    {
        if (hideLabels->text().contains("Hide titles"))
        {
            for(auto album: albumsList)
                album->showTitle(false);
            hideLabels->setText("Show titles");
            hiddenLabels = true;

        }
        else
        {
            for(auto album: albumsList)
                album->showTitle(true);
            hideLabels->setText("Hide titles");
            hiddenLabels = false;
        }

    });

    QAction *zoomIn = new QAction(this);
    zoomIn->setText("Zoom in");
    zoomIn->setShortcut(tr("CTRL++"));
    grid->addAction(zoomIn);
    connect(zoomIn, &QAction::triggered,[this]()
    {
        if(albumSize+5<=BaeUtils::MAX_MID_ALBUM_SIZE)
        {
            this->setAlbumsSize(albumSize+5);
            this->adjustGrid();

        }

    });

    QAction *zoomOut = new QAction(this);
    zoomOut->setText("Zoom out");
    zoomOut->setShortcut(tr("CTRL+-"));
    grid->addAction(zoomOut);
    connect(zoomOut, &QAction::triggered,[this]()
    {
        if(albumSize-5>=BaeUtils::MAX_MIN_ALBUM_SIZE)
        {
            this->setAlbumsSize(albumSize-5);
            this->adjustGrid();

        }
    });


    auto utilsLayout = new QHBoxLayout();
    utilsLayout->setContentsMargins(0,0,0,0);
    utilsLayout->setSpacing(0);

    utilsFrame = new QFrame();
    utilsFrame->setLayout(utilsLayout);
    utilsFrame->setFrameShape(QFrame::NoFrame);
    utilsFrame->setFrameShadow(QFrame::Plain);
    utilsFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    /*slider = new QSlider();
    connect(slider,SIGNAL(sliderMoved(int)),this,SLOT(setAlbumsSize(int)));
    slider->setMaximum(200);
    slider->setMinimum(80);
    slider->setValue(ALBUM_SIZE_MEDIUM);
    slider->setSliderPosition(ALBUM_SIZE_MEDIUM);
    slider->setOrientation(Qt::Orientation::Horizontal);*/





    // utilsLayout->addWidget(slider);

    albumTable = new BabeTable(this);
    albumTable->setFrameShape(QFrame::NoFrame);
    //    albumTable->horizontalHeader()->setVisible(false);
    albumTable->showColumn(BabeTable::TRACK);
    albumTable->showColumn(BabeTable::STARS);
    albumTable->hideColumn(BabeTable::ARTIST);
    albumTable->hideColumn(BabeTable::ALBUM);

    auto albumBox = new QGridLayout();
    albumBox->setContentsMargins(0,0,0,0);
    albumBox->setSpacing(0);

    albumBox_frame = new QWidget(this);
    albumBox_frame->setLayout(albumBox);
    //albumBox_frame->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

    line_h = new QFrame(this);
    line_h->setFrameShape(QFrame::HLine);
    line_h->setFrameShadow(QFrame::Plain);
    line_h->setMaximumHeight(1);

    cover = new Album(this);
    connect(cover,&Album::playAlbum,[this] (QMap<int,QString> info) { emit this->playAlbum(info); });
    connect(cover,&Album::changedArt,this,&AlbumsView::changedArt_cover);
    connect(cover,&Album::babeAlbum_clicked,this,&AlbumsView::babeAlbum);
    cover->createAlbum("","",":Data/data/cover.svg",BaeUtils::MEDIUM_ALBUM,0,true);
    cover->showTitle(false);

    closeBtn = new QToolButton(cover);
    connect(closeBtn,&QToolButton::clicked,this,&AlbumsView::hideAlbumFrame);
    closeBtn->setGeometry(2,2,16,16);
    closeBtn->setIcon(QIcon::fromTheme("tab-close"));
    closeBtn->setAutoRaise(true);
    closeBtn->setToolTip("Close");

    expandBtn = new QToolButton(cover);
    connect(expandBtn,&QToolButton::clicked,this,&AlbumsView::expandList);
    expandBtn->setGeometry(cover->getSize()-18,2,16,16);
    expandBtn->setIcon(QIcon(":/Data/data/icons/artists_selected.svg"));
    expandBtn->setAutoRaise(true);

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMaximumWidth(1);


    if(extraList)
    {
        this->extraList=true;

        artistList=new QListWidget(this);
        connect(artistList,SIGNAL(clicked(QModelIndex)),this,SLOT(filterAlbum(QModelIndex)));
        artistList->setFrameShape(QFrame::NoFrame);
        artistList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        artistList->setMaximumWidth(cover->getSize());
        artistList->setAlternatingRowColors(true);
        // artistList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
        // artistList->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);

        albumBox->addWidget(cover,0,0,Qt::AlignTop);
        albumBox->addWidget(artistList,1,0,Qt::AlignLeft);
        albumBox->addWidget(line,0,1,2,1, Qt::AlignLeft);
        albumBox->addWidget(albumTable,0,2,2,1);

    }else
    {
        albumBox->addWidget(cover,0,0,Qt::AlignTop);
        albumBox->addWidget(line,0,1,Qt::AlignLeft);
        albumBox->addWidget(albumTable,0,2);

    }

    QSplitter *splitter = new QSplitter(parent);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Vertical);

    splitter->addWidget(grid);
    splitter->addWidget(line_h);
    splitter->addWidget(albumBox_frame);

    layout->addWidget(splitter);

    albumBox_frame->hide(); line_h->hide();

    splitter->setSizes({0,0,0});
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    this->setLayout(layout);
}

AlbumsView::~AlbumsView(){}


void AlbumsView::expandList()
{
    auto album = cover->getAlbum();
    auto artist = cover->getArtist();

    emit expandTo(artist, album);
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
    albumTable->populateTableView("SELECT * FROM tracks WHERE album = \""+album+"\" AND artist =\""+cover->getArtist()+"\" ORDER by album asc, track asc ",false);
    cover->setTitle(cover->getArtist(),album);

    QSqlQuery queryCover = connection.getQuery("SELECT * FROM albums WHERE title = \""+album+"\" AND artist =\""+cover->getArtist()+"\"");
    while (queryCover.next())
        if(!queryCover.value(2).toString().isEmpty()&&queryCover.value(2).toString()!="NULL") cover->putPixmap( queryCover.value(2).toString());

}

void AlbumsView::setAlbumsSize(int value)
{
    albumSize=value;
    /*slider->setToolTip(QString::number(value));
                QToolTip::showText( slider->mapToGlobal( QPoint( 0, 0 ) ), QString::number(value) );*/
    for(auto album : albumsList)
    {
        album->setSize(albumSize);
        grid->setGridSize(QSize(albumSize+25,albumSize+25));
        grid->update();

    }

    for(auto item : itemsList) item->setSizeHint(QSize(albumSize, albumSize));

}

void AlbumsView::albumHover() { }

void  AlbumsView::flushGrid()
{
    this->hideAlbumFrame();
    albumsList.clear();
    albums.clear();
    artists.clear();
    grid->clear();
}

void AlbumsView::orderChanged()
{
    if(ascending)
    {
        grid->sortItems(Qt::DescendingOrder);
        order->setText("Go Ascending");
        ascending=!ascending;
    }else
    {
        grid->sortItems(Qt::AscendingOrder);
        order->setText("Go Descending");
        ascending=!ascending;
    }


}

void AlbumsView::populateTableView(QSqlQuery query)
{
    qDebug()<<"ON POPULATE ALBUM VIEW:";
    int i =0;
    while (query.next())
    {
        QString artist = query.value(ARTIST).toString();
        QString album = query.value(TITLE).toString();
        QString art=":Data/data/cover.svg";

        if(!albums.contains(album+" "+artist))
        {
            albums<<album+" "+artist;
            //qDebug()<<"creating a new album[cover] for<<"<<album+" "+artist;
            if(!query.value(ART).toString().isEmpty()&&query.value(ART).toString()!="NULL")
                art = query.value(ART).toString();
            else art = connection.getArtistArt(artist);

            auto artwork= new Album(this);
            connect(artwork,&Album::albumCreated,[this](Album *album){emit createdAlbum(album);});
            artwork->createAlbum(artist,album,art,BaeUtils::MEDIUM_ALBUM,4,true);

        }

        qDebug()<<"oi"<<i++;
    }

    //grid->adjustSize();
    //    qDebug()<<grid->width()<<grid->size().height();
    grid->sortItems(Qt::AscendingOrder);

}


void AlbumsView::filter(const QList<QMap<int,QString>> &filter, const BabeTable::columns &type)
{

    hide_all(true);
    this->hideAlbumFrame();

    QList<QListWidgetItem*> matches;

    for(auto result : filter)
    {

        switch(type)

        {
        case BabeTable::ALBUM:
            matches<<grid->findItems(result[BabeTable::ALBUM]+" "+result[BabeTable::ARTIST], Qt::MatchFlag::MatchContains);
            break;
        case BabeTable::ARTIST:
            matches<<grid->findItems(result[BabeTable::ARTIST], Qt::MatchFlag::MatchContains);
            break;
        }


    }
    for(QListWidgetItem* item : matches)
        item->setHidden(false);
}

void AlbumsView::hide_all(bool state)
{
    for(int row(0); row < grid->count(); row++ )
        grid->item(row)->setHidden(state);
}


void AlbumsView::babeAlbum(QMap<int,QString> info)
{
    emit babeAlbum_clicked(info);
}

void AlbumsView::populateTableViewHeads(QSqlQuery query)
{
    qDebug()<<"ON POPULATE HEADS VIEW:";
    while (query.next())
    {
        QString artist =query.value(TITLE).toString();
        QString art=":Data/data/cover.svg";


        if(!artists.contains(artist))
        {
            artists<<artist;

            if(!query.value(1).toString().isEmpty()&&query.value(1).toString()!="NULL")
                art=(query.value(1).toString());

            auto artwork= new Album(this);

            connect(artwork,&Album::albumCreated,[this](Album *album){emit createdAlbum(album);});
            artwork->createAlbum(artist,"",art,BaeUtils::MEDIUM_ALBUM,4,true);

        }
    }

    grid->sortItems(Qt::AscendingOrder);
}

void AlbumsView::populateExtraList(QSqlQuery query)
{
    artistList->clear();

    qDebug()<<"ON POPULATE EXTRA LIST:";
    while (query.next())
    {
        auto album = query.value(TITLE).toString();
        auto item = new QListWidgetItem();
        item->setText(album);
        item->setTextAlignment(Qt::AlignCenter);
        artistList->addItem(item);
    }
}


void AlbumsView::changedArt_cover(QMap<int,QString> info)
{
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];
    QString path = info[Album::ART];

    connection.execQuery(QString("UPDATE albums SET art = \"%1\" WHERE title = \"%2\" AND artist = \"%3\"").arg(path,album,artist));

}

void AlbumsView::changedArt_head(QMap<int,QString> info)
{
    QString artist =info[Album::ARTIST];
    QString path = info[Album::ART];
    connection.execQuery(QString("UPDATE artists SET art = \"%1\" WHERE title = \"%2\" ").arg(path,artist) );

}

void AlbumsView::getArtistInfo(QMap<int,QString> info)
{
    albumBox_frame->show();
    line_h->show();

    QString artist =info[Album::ARTIST];
    cover->setTitle(artist);

    albumTable->flushTable();

    albumTable->populateTableView("SELECT * FROM tracks WHERE artist = \""+artist+"\" ORDER by album asc, track asc",false);

    auto art = connection.getArtistArt(artist);
    if(!art.isEmpty()) cover->putPixmap(art);
    else cover->putDefaultPixmap();



    //    QListView w;
    //    QPalette p = w.palette();
    //    p.setBrush(QPalette::Base, QPixmap("/usr/share/wallpapers/Air/contents/images/1024x768.jpg"));
    //    w.setPalette(p);

    //    QGraphicsBlurEffect* effect	= new QGraphicsBlurEffect();
    //    effect->setBlurRadius(5);
    //    int extent =0;
    //    auto src = QPixmap(art).toImage();
    //    QGraphicsScene scene;
    //    QGraphicsPixmapItem item;
    //    item.setPixmap(QPixmap::fromImage(src));
    //    item.setGraphicsEffect(effect);
    //    scene.addItem(&item);
    //    QImage res(src.size()+QSize(extent*2, extent*2), QImage::Format_ARGB32);
    //    res.fill(Qt::transparent);
    //    QPainter ptr(&res);
    //    scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ) );


    //    QPalette palette = albumBox_frame->palette();
    //    palette.setBrush(QPalette::Background,res);
    //    albumBox_frame->setPalette(palette);
    //    albumBox_frame->setGraphicsEffect(effect);

    if(extraList) populateExtraList(connection.getQuery("SELECT * FROM albums WHERE artist = \""+artist+"\" ORDER by title asc"));

}

bool AlbumsView::eventFilter(QObject *obj, QEvent *event)
{
    if(obj==grid && event->type()==QEvent::Resize)
        this->adjustGrid();

    return QWidget::eventFilter(obj, event);

}

void AlbumsView::adjustGrid()
{
    auto scrollSize = this->grid->verticalScrollBar()->size().width()+1;
    auto gridSize = this->grid->size().width()-scrollSize;
    auto amount = (gridSize/(albumSize+25));
    auto leftSpace = gridSize-amount*albumSize;


    if(gridSize>albumSize)
    {

        grid->setGridSize(QSize(albumSize+(leftSpace/amount),albumSize+25));
    }

    qDebug()<<"gridSize:"<<gridSize<<"amount:"<<amount<<"left space: "<<leftSpace<<"scroll width:"<<scrollSize;
}

void AlbumsView::getAlbumInfo(QMap<int,QString> info)
{
    albumBox_frame->setVisible(true);
    line_h->setVisible(true);
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];

    cover->setTitle(artist,album);
    expandBtn->setToolTip("View "+ cover->getArtist());

    albumTable->flushTable();

    albumTable->populateTableView("SELECT * FROM tracks WHERE artist = \""+artist+"\" and album = \""+album+"\" ORDER by track asc",false);

    auto art = connection.getAlbumArt(album,artist);
    art = art.isEmpty()? connection.getArtistArt(artist) : art;

    cover->putPixmap(art); if(!art.isEmpty()) cover->putPixmap(art);
    else cover->putDefaultPixmap();

}



