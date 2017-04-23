#include "albumsview.h"



AlbumsView::AlbumsView(bool extraList, QWidget *parent) :
    QWidget(parent)
{

    grid = new QListWidget(this);
    grid->parentWidget()->installEventFilter(this);
    grid->setViewMode(QListWidget::IconMode);
    grid->setResizeMode(QListWidget::Adjust);
    grid->setUniformItemSizes(true);
    grid->setWrapping(true);
    //grid->setMovement(QListWidget::Static);
    grid->setFrameShape(QFrame::NoFrame);
    grid->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );
    grid->setSizeAdjustPolicy(QListWidget::AdjustToContents);
    //grid->setStyleSheet("QListWidget {background:#2E2F30; border:1px solid black; border-radius: 2px; }");
    grid->setStyleSheet("QListWidget {background:transparent; padding-top:15px; padding-left:15px; }");
    grid->setGridSize(QSize(albumSize+10,albumSize+10));

    QAction *zoomIn = new QAction(this);
    zoomIn->setShortcut(tr("CTRL++"));
    connect(zoomIn, &QAction::triggered,[this](){
        if(albumSize+5<=200)
        {
            this->setAlbumsSize(albumSize+5);
            slider->setValue(albumSize+5);
            slider->setSliderPosition(albumSize+5);
        }

    });

    QAction *zoomOut = new QAction(this);
    zoomOut->setShortcut(tr("CTRL+-"));
    connect(zoomOut, &QAction::triggered,[this](){
        if(albumSize-5>=80){
            this->setAlbumsSize(albumSize-5);
            slider->setValue(albumSize-5);
            slider->setSliderPosition(albumSize-5);
        }
    });

    this->addAction(zoomIn);
    this->addAction(zoomOut);


    auto utilsLayout = new QHBoxLayout();
    utilsLayout->setContentsMargins(0,0,0,0);
    utilsLayout->setSpacing(0);

    utilsFrame = new QFrame();
    utilsFrame->setLayout(utilsLayout);
    // utilsFrame->setFrameShape(QFrame::StyledPanel);
    utilsFrame->setFrameShadow(QFrame::Plain);
    utilsFrame->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    slider = new QSlider();
    connect(slider,SIGNAL(sliderMoved(int)),this,SLOT(setAlbumsSize(int)));
    //slider->setMaximumWidth(100);
    slider->setMaximum(200);
    slider->setMinimum(80);
    slider->setValue(albumSize);
    slider->setSliderPosition(albumSize);
    slider->setOrientation(Qt::Orientation::Horizontal);


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

    auto layout = new QGridLayout();
    layout->setMargin(0);
    layout->addWidget(grid,0,0);
    layout->setSpacing(0);

    auto albumBox = new QGridLayout();
    albumBox->setContentsMargins(0,0,0,0);
    albumBox->setSpacing(0);

    albumBox_frame = new QWidget(this);
    albumBox_frame->setLayout(albumBox);

    line_h = new QFrame(this);
    line_h->setFrameShape(QFrame::HLine);
    line_h->setFrameShadow(QFrame::Plain);
    line_h->setMaximumHeight(1);

    layout->addWidget(line_h,1,0,Qt::AlignBottom);
    layout->addWidget(albumBox_frame,2,0,Qt::AlignBottom);

    cover = new Album(":Data/data/cover.svg",120,0,true,this);
    connect(cover,SIGNAL(playAlbum(QString , QString)),this,SLOT(playAlbum_clicked(QString, QString)));
    connect(cover,SIGNAL(changedArt(QString, QString , QString)),this,SLOT(changedArt_cover(QString, QString, QString)));
    connect(cover,SIGNAL(babeAlbum_clicked(QString, QString)),this,SLOT(babeAlbum(QString, QString)));

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
        albumBox_frame->setMaximumHeight(200);

        artistList=new QListWidget(this);
        connect(artistList,SIGNAL(clicked(QModelIndex)),this,SLOT(filterAlbum(QModelIndex)));
        artistList->setFrameShape(QFrame::NoFrame);
        artistList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        artistList->setMaximumWidth(120);
        artistList->setAlternatingRowColors(true);

        //albumTable->setMaximumHeight(200);
        albumBox->addWidget(cover,0,0,Qt::AlignLeft);
        albumBox->addWidget(artistList,1,0,Qt::AlignLeft);
        albumBox->addWidget(line,0,1,2,1, Qt::AlignLeft);
        albumBox->addWidget(albumTable,0,2,2,1);

    }else
    {
        albumTable->setMaximumHeight(120);
        albumBox->addWidget(cover,0,0,Qt::AlignLeft);
        albumBox->addWidget(line,0,1,Qt::AlignLeft);
        albumBox->addWidget(albumTable,0,2);

    }

    albumBox_frame->hide();
    line_h->hide();
    this->setLayout(layout);
}

AlbumsView::~AlbumsView(){}


void AlbumsView::hideAlbumFrame()
{
    albumTable->flushTable();
    albumBox_frame->hide();
    line_h->hide();
}

bool AlbumsView::eventFilter(QObject * watched, QEvent * event)
{
   if(watched != grid->parent()) return false;

   /* if (event->type() == QEvent::Resize)
    {
        int w = grid->width();
        int a =  grid->gridSize().width()+2;
        int c = w/a;
        int amount = (w-(c*a))/c;
        qDebug()<<"amount: "<< amount <<"w: "<<w<<"a: "<<a<<"c: "<<c;
       if(grid->width()>gridSize)
           grid->setGridSize(QSize(a+c,a+c));

        else
           grid->setGridSize(QSize(c*amount,c*amount));


        gridSize = grid->width();
        slider->setValue(albumSize);
        slider->setSliderPosition(albumSize);
        return true;
    }*/
    return false;
}


void AlbumsView::filterAlbum(QModelIndex index) {
    QString album = index.data().toString();
    qDebug()<<album;
    albumTable->flushTable();
    albumTable->populateTableView("SELECT * FROM tracks WHERE album = \""+album+"\" AND artist =\""+cover->getArtist()+"\" ORDER by album asc, track asc ");
    cover->setAlbum(album);
    cover->setTitle();

    QSqlQuery queryCover = connection->getQuery("SELECT * FROM albums WHERE title = \""+album+"\" AND artist =\""+cover->getArtist()+"\"");
    while (queryCover.next())
    {
        if(!queryCover.value(2).toString().isEmpty()&&queryCover.value(2).toString()!="NULL") cover->putPixmap( queryCover.value(2).toString());

    }

}

void AlbumsView::setAlbumsSize(int value)
{
    albumSize=value;
    slider->setToolTip(QString::number(value));
    QToolTip::showText( slider->mapToGlobal( QPoint( 0, 0 ) ), QString::number(value) );
    for(auto album : albumsList)
    {
        album->setSize(albumSize);
        grid->setGridSize(QSize(albumSize+10,albumSize+10));
        grid->update();

    }

    for(auto item : itemsList) item->setSizeHint(QSize(albumSize, albumSize));

}

void AlbumsView::albumHover()
{
    //cover->setCoverArt("../player/data/cover_hover.svg");
}

void  AlbumsView::flushGrid()
{
    this->hideAlbumFrame();
    albumsList.clear();
    albums.clear();
    artists.clear();
    grid->clear();
}

void AlbumsView::orderChanged(QString order)
{
    emit albumOrderChanged(order);
}

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

            auto artwork= new Album(art,albumSize,4,true,this);
            albumsList.push_back(artwork);

            artwork->borderColor=true;
            artwork->setArtist(artist);
            artwork->setAlbum(album);
            artwork->setTitle();

            connect(artwork, SIGNAL(albumCoverClicked(QStringList)),this,SLOT(getAlbumInfo(QStringList)));
            connect(artwork,SIGNAL(playAlbum(QString , QString)),this,SLOT(playAlbum_clicked(QString, QString)));
            connect(artwork,SIGNAL(changedArt(QString, QString , QString)),this,SLOT(changedArt_cover(QString, QString, QString)));
            connect(artwork,SIGNAL(babeAlbum_clicked(QString, QString)),this,SLOT(babeAlbum(QString, QString)));
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
            item->setSizeHint( QSize( albumSize, albumSize));
            grid->addItem(item);
            grid->setItemWidget(item,artwork);
        }

    }

    //grid->adjustSize();
    qDebug()<<grid->width()<<grid->size().height();
    emit populateCoversFinished();


}

void AlbumsView::babeAlbum(QString album, QString artist)
{
    emit babeAlbum_clicked(album, artist);
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
            // qDebug()<<"creating a new album[head] for<<"<<artist;

            if(!query.value(1).toString().isEmpty()&&query.value(1).toString()!="NULL")
                art=(query.value(1).toString());

            Album *album= new Album(art,albumSize,4,true,this);
            albumsList.push_back(album);

            album->borderColor=true;
            album->setArtist(artist);
            album->setTitle();
            //album->titleVisible(false);

            // album->setTitle(query.value(1).toString(),query.value(2).toString());
            //album->setToolTip(query.value(2).toString());
            connect(album, SIGNAL(albumCoverClicked(QStringList)),this,SLOT(getArtistInfo(QStringList)));
            connect(album,SIGNAL(playAlbum(QString , QString)),this,SLOT(playAlbum_clicked(QString, QString)));
            connect(album,SIGNAL(changedArt(QString, QString , QString)),this,SLOT(changedArt_head(QString, QString, QString)));
            connect(album,SIGNAL(babeAlbum_clicked(QString, QString)),this,SLOT(babeAlbum(QString, QString)));

            //album->setStyleSheet(":hover {background:#3daee9; }");
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

    //artistList->addItems(albums);

}


void AlbumsView::playAlbum_clicked(QString artist, QString album)
{

    emit playAlbum(artist,album);
}

void AlbumsView::changedArt_cover(QString path, QString artist, QString album)
{
    connection->execQuery(QString("UPDATE albums SET art = \"%1\" WHERE title = \"%2\" AND artist = \"%3\"").arg(path,album,artist) );

}

void AlbumsView::changedArt_head(QString path, QString artist, QString album)
{
    Q_UNUSED(album);
    connection->execQuery(QString("UPDATE artists SET art = \"%1\" WHERE title = \"%2\" ").arg(path,artist) );

}
void AlbumsView::passConnection(CollectionDB *con)
{
    this->connection=con;
}

void AlbumsView::getArtistInfo(QStringList info)
{
    albumBox_frame->show();
    line_h->show();
    // QSqlQuery query = connection->getQuery("SELECT * FROM tracks WHERE artist = \""+info.at(0)+"\" and album = \""+info.at(1)+"\"");
    //QStringList tracks;
    //playlist = new Playlist();

    //while(query.next())  tracks<<query.value(3).toString();

    cover->setArtist(info.at(0));
    cover->setAlbum(info.at(1));
    cover->setTitle();

    qDebug()<<info.at(0)<<info.at(1);
    //playlist->add(tracks);
    albumTable->flushTable();

    albumTable->populateTableView("SELECT * FROM tracks WHERE artist = \""+info.at(0)+"\" ORDER by album asc, track asc ");
    QSqlQuery queryCover = connection->getQuery("SELECT * FROM artists WHERE title = \""+info.at(0)+"\"");
    while (queryCover.next())
    {
        if(!queryCover.value(1).toString().isEmpty()&&queryCover.value(1).toString()!="NULL") cover->putPixmap( queryCover.value(1).toString());

    }
    if(extraList) populateExtraList(connection->getQuery("SELECT * FROM albums WHERE artist = \""+info.at(0)+"\" ORDER by title asc"));

}

void AlbumsView::getAlbumInfo(QStringList info)
{
    albumBox_frame->show();
    line_h->show();
    // QSqlQuery query = connection->getQuery("SELECT * FROM tracks WHERE artist = \""+info.at(0)+"\" and album = \""+info.at(1)+"\"");
    //QStringList tracks;
    //playlist = new Playlist();

    //while(query.next())  tracks<<query.value(3).toString();

    cover->setArtist(info.at(0));
    cover->setAlbum(info.at(1));
    cover->setTitle();

    qDebug()<<info.at(0)<<info.at(1);
    //playlist->add(tracks);
    albumTable->flushTable();

    albumTable->populateTableView("SELECT * FROM tracks WHERE artist = \""+info.at(0)+"\" and album = \""+info.at(1)+"\" ORDER by track asc ");
    QSqlQuery queryCover = connection->getQuery("SELECT * FROM albums WHERE title = \""+info.at(1)+"\" AND artist = \""+info.at(0)+"\"");
    while (queryCover.next())
    {
        if(!queryCover.value(2).toString().isEmpty()&&queryCover.value(2).toString()!="NULL") cover->putPixmap( queryCover.value(2).toString());

    }
}

void AlbumsView::albumTable_clicked(QStringList list)
{

    emit songClicked(list);
}

void AlbumsView::albumTable_rated(QStringList list)
{

    emit songRated(list);
}

void AlbumsView::albumTable_babeIt(QStringList list)
{

    emit songBabeIt(list);
}
