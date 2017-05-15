#include "albumsview.h"



AlbumsView::AlbumsView(bool extraList, QWidget *parent) :
    QWidget(parent)
{    
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto layout = new QGridLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    grid = new QListWidget(this);
    grid->setViewMode(QListWidget::IconMode);
    grid->setResizeMode(QListWidget::Adjust);
    grid->setUniformItemSizes(true);
    grid->setWrapping(true);
    grid->setFrameShape(QFrame::NoFrame);
    grid->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );
    grid->setSizeAdjustPolicy(QListWidget::AdjustToContentsOnFirstShow);
    //grid->setStyleSheet("QListWidget {background:#2E2F30; border:1px solid black; border-radius: 2px; }");
    grid->setStyleSheet("QListWidget {background:transparent; padding-top:15px; padding-left:15px; }");
    grid->setGridSize(QSize(albumSize+10,albumSize+10));

    QAction *zoomIn = new QAction(this);
    zoomIn->setShortcut(tr("CTRL++"));
    connect(zoomIn, &QAction::triggered,[this](){
        if(albumSize+5<=200)
        {
            this->setAlbumsSize(albumSize+5);
           /* slider->setValue(albumSize+5);
            slider->setSliderPosition(albumSize+5);*/
        }
    });

    QAction *zoomOut = new QAction(this);
    zoomOut->setShortcut(tr("CTRL+-"));
    connect(zoomOut, &QAction::triggered,[this](){
        if(albumSize-5>=80){
            this->setAlbumsSize(albumSize-5);
           /* slider->setValue(albumSize-5);
            slider->setSliderPosition(albumSize-5);*/
        }
    });

    this->addAction(zoomIn);
    this->addAction(zoomOut);

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
    slider->setValue(albumSize);
    slider->setSliderPosition(albumSize);
    slider->setOrientation(Qt::Orientation::Horizontal);*/

    order = new QComboBox();
    connect(order, SIGNAL(currentIndexChanged(QString)),this,SLOT(orderChanged(QString)));
    order->setFrame(false);
    /*order->setMaximumWidth(70);
    order->setMaximumHeight(22);*/
    order->setContentsMargins(0,0,0,0);

    order->addItem("Artist");
    order->addItem("Title");
    order->setCurrentIndex(1);

    utilsLayout->addWidget(order);
    // utilsLayout->addWidget(slider);

    albumTable = new BabeTable(this);
    albumTable->setFrameShape(QFrame::NoFrame);
    albumTable->horizontalHeader()->setVisible(false);
    albumTable->showColumn(BabeTable::TRACK);
    albumTable->showColumn(BabeTable::STARS);
    albumTable->hideColumn(BabeTable::ARTIST);
    albumTable->hideColumn(BabeTable::ALBUM);
    //albumTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
    //albumTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

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

    cover = new Album(":Data/data/cover.svg",120,0,true,this);
    connect(cover,&Album::playAlbum,[this] (QMap<int,QString> info) { emit playAlbum(info); });
    connect(cover,&Album::changedArt,this,&AlbumsView::changedArt_cover);
    connect(cover,&Album::babeAlbum_clicked,this,&AlbumsView::babeAlbum);

    closeBtn = new QToolButton(cover);
    connect(closeBtn,SIGNAL(clicked()),SLOT(hideAlbumFrame()));
    closeBtn->setGeometry(2,2,16,16);
    closeBtn->setIcon(QIcon::fromTheme("tab-close"));
    closeBtn->setAutoRaise(true);

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
        artistList->setMaximumWidth(120);
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
    albumTable->populateTableView("SELECT * FROM tracks WHERE album = \""+album+"\" AND artist =\""+cover->getArtist()+"\" ORDER by album asc, track asc ",false,false);
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
        grid->setGridSize(QSize(albumSize+10,albumSize+10));
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

void AlbumsView::orderChanged(QString order) { emit albumOrderChanged(order); }

void AlbumsView::populateTableView(QSqlQuery query)
{
    qDebug()<<"ON POPULATE ALBUM VIEW:";

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

            auto artwork= new Album(art,albumSize,4,true,this);
            albumsList.push_back(artwork);

            artwork->borderColor=true;
            artwork->setTitle(artist,album);

            connect(artwork, &Album::albumCoverClicked,this,&AlbumsView::getAlbumInfo);
            connect(artwork, &Album::albumCoverDoubleClicked, [this] (QMap<int, QString> info) { emit albumDoubleClicked(info); });
            connect(artwork,&Album::playAlbum, [this] (QMap<int,QString> info) { emit playAlbum(info); });
            connect(artwork,&Album::changedArt,this,&AlbumsView::changedArt_cover);
            connect(artwork,&Album::babeAlbum_clicked,this,&AlbumsView::babeAlbum);
            //connect(artwork,SIGNAL(albumDragged()),grid,SLOT(clear()));

            connect(artwork, &Album::albumDragged, [this]()
            {
                //grid->adjustSize();
                //grid->update();
                // grid->updateGeometry();

                //grid->clearMask();
            });

            auto item = new QListWidgetItem();
            itemsList.push_back(item);
            item->setSizeHint(QSize( albumSize, albumSize));
            grid->addItem(item);
            grid->setItemWidget(item,artwork);
        }

    }

    //grid->adjustSize();
    qDebug()<<grid->width()<<grid->size().height();
    emit populateCoversFinished();


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

            Album *album= new Album(art,albumSize,4,true,this);
            albumsList.push_back(album);

            album->borderColor=true;
            album->setTitle(artist);

            connect(album, &Album::albumCoverClicked,this,&AlbumsView::getArtistInfo);
            connect(album, &Album::albumCoverDoubleClicked, [this] (QMap<int, QString> info) { emit albumDoubleClicked(info); });
            connect(album,&Album::playAlbum,[this](QMap<int,QString> info) { emit playAlbum(info);});
            connect(album,&Album::changedArt,this,&AlbumsView::changedArt_head);
            connect(album,&Album::babeAlbum_clicked,this,&AlbumsView::babeAlbum);

            auto item =new QListWidgetItem();
            itemsList.push_back(item);
            item->setSizeHint( QSize( albumSize, albumSize));
            grid->addItem(item);
            grid->setItemWidget(item,album);

        }
    }

    emit populateHeadsFinished();
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

    albumTable->populateTableView("SELECT * FROM tracks WHERE artist = \""+artist+"\" ORDER by album asc, track asc",false,false);

    auto art = connection.getArtistArt(artist);
    if(!art.isEmpty()) cover->putPixmap(art);
    else cover->putDefaultPixmap();

    if(extraList) populateExtraList(connection.getQuery("SELECT * FROM albums WHERE artist = \""+artist+"\" ORDER by title asc"));

}

void AlbumsView::getAlbumInfo(QMap<int,QString> info)
{
    albumBox_frame->setVisible(true);
    line_h->setVisible(true);
    QString artist =info[Album::ARTIST];
    QString album = info[Album::ALBUM];

    cover->setTitle(artist,album);

    albumTable->flushTable();

    albumTable->populateTableView("SELECT * FROM tracks WHERE artist = \""+artist+"\" and album = \""+album+"\" ORDER by track asc",false,false);

    auto art = connection.getAlbumArt(album,artist);
    art = art.isEmpty()? connection.getArtistArt(artist) : art;

    cover->putPixmap(art); if(!art.isEmpty()) cover->putPixmap(art);
    else cover->putDefaultPixmap();

    albumBox_frame->resize(cover->sizeHint());
}

void AlbumsView::albumTable_clicked(QStringList list) { emit songClicked(list); }

void AlbumsView::albumTable_rated(QStringList list) { emit songRated(list); }

void AlbumsView::albumTable_babeIt(QStringList list) { emit songBabeIt(list); }
