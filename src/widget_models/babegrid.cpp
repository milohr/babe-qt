#include "babegrid.h"

BabeGrid::BabeGrid(const ALBUM_FACTOR &factor, const AlbumSizeHint &deafultValue, const uint8_t &albumRadius, QWidget *parent) : QListWidget(parent)
{
    this->albumFactor = factor;
    this->defaultAlbumValue= deafultValue;
    this->albumSize = Bae::getWidgetSizeHint(factor, deafultValue);
    this->albumRadius = albumRadius;
    this->installEventFilter(this);
    //    this->setObjectName("grid");
    this->setMinimumHeight(static_cast<int>(this->albumSize));
    this->setViewMode(QListWidget::IconMode);
    this->setResizeMode(QListWidget::Adjust);
    this->setWrapping(true);
    this->setAcceptDrops(false);
    this->setDragDropMode(QAbstractItemView::DragOnly);
//    this->setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );
    this->setSizeAdjustPolicy(QListWidget::AdjustToContentsOnFirstShow);

    this->setStyleSheet("QListWidget::item,QListWidget::item:selected,QListWidget::item:selected:active {background:transparent; color:transparent; }");
    this->setGridSize(QSize(static_cast<int>(this->albumSize+this->albumSpacing),static_cast<int>(this->albumSize+this->albumSpacing)));
    this->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    this->setUpActions();

}

BabeGrid::~BabeGrid()
{
    qDebug()<<"DELETING BABEGRID";
}

void BabeGrid::addAlbum(const Bae::DB &albumMap)
{
    Bae::DB auxMap {{Bae::KEY::ARTIST,albumMap[Bae::KEY::ARTIST]},{Bae::KEY::ALBUM,albumMap[Bae::KEY::ALBUM]}};

    if(!this->albumsMap.contains(auxMap))
    {
        auto album= new BabeAlbum(albumMap,defaultAlbumValue,this->albumRadius,true,this);
        this->albumsMap.insert(auxMap,album);

        connect(album,&BabeAlbum::albumCoverClicked,[this](const Bae::DB &albumMap)
        {
            emit this->albumClicked(albumMap);
        });


        if(this->albumShadows)
        {
            auto shadow = new QGraphicsDropShadowEffect(this);

            shadow->setColor(QColor(0, 0, 0, 140));
            shadow->setBlurRadius(9);
            shadow->setOffset(3,5);

            album->setGraphicsEffect(shadow);
        }

        album->borderColor=false;
        album->setUpMenu();
        album->showTitle(!hiddenLabels);

        auto sendIt = new QAction("Send it to phone",this);

        album->addAction(sendIt);

        connect(album,&BabeAlbum::albumCoverDoubleClicked, [this] (const Bae::DB &albumMap)
        {
            emit this->albumDoubleClicked(albumMap);
        });

        connect(album,&BabeAlbum::playAlbum, [this] (const Bae::DB info) { emit this->playAlbum(info); });
        connect(album,&BabeAlbum::babeAlbum,[this] (const Bae::DB info) { emit this->babeAlbum(info); });
        connect(album,&BabeAlbum::albumDragStarted,[this](){emit this->dragAlbum();});
        connect(album,&BabeAlbum::albumCoverEnter,[=]()
        {
            if(hiddenLabels) album->showTitle(true);
            //            shadow->setColor(QColor(0, 0, 0, 180));
            //            shadow->setOffset(2,3);
        });

        connect(album, &BabeAlbum::albumCoverLeave,[=]()
        {
            if(hiddenLabels) album->showTitle(false);
            //            shadow->setColor(QColor(0, 0, 0, 140));
            //            shadow->setOffset(3,5);
        });

        auto item = new QListWidgetItem;
        this->itemsList.push_back(item);
        item->setSizeHint(QSize(static_cast<int>(album->getSize()),static_cast<int>(album->getSize())));

        if(album->getAlbum().isEmpty() && !album->getArtist().isEmpty()) item->setText(album->getArtist());
        else item->setText(album->getAlbum()+" "+album->getArtist());

        item->setTextAlignment(Qt::AlignCenter);
        this->addItem(item);
        this->setItemWidget(item,album);

    }else this->albumsMap[auxMap]->putPixmap(albumMap[Bae::KEY::ARTWORK]);

    emit this->albumReady();
}

void BabeGrid::flushGrid()
{
    this->albumsMap.clear();
    this->clear();
}

void BabeGrid::setAlbumsSize(const uint &value)
{
    this->albumSize=value;
    /*slider->setToolTip(QString::number(value));
                QToolTip::showText( slider->mapToGlobal( QPoint( 0, 0 ) ), QString::number(value) );*/
    for(auto album : albumsMap.values())
    {
        album->setSize(albumSize);
        this->setGridSize(QSize(static_cast<int>(this->albumSize+this->albumSpacing),static_cast<int>(this->albumSize+this->albumSpacing)));
        this->update();
    }

    for(auto item : itemsList) item->setSizeHint(QSize(static_cast<int>(albumSize), static_cast<int>(albumSize)));
}

void BabeGrid::setAlbumsSpacing(const uint &space)
{
    this->albumSpacing=space;
    this->setGridSize(QSize(static_cast<int>(this->albumSize+this->albumSpacing),static_cast<int>(this->albumSize+this->albumSpacing)));

}

void BabeGrid::showLabels(const bool &state)
{
    for(auto album: albumsMap.values())
        album->showTitle(state);

    this->actions().at(1)->setText(state ? "Show titles" : "Hide Labels" );
    this->hiddenLabels = !this->hiddenLabels ;
}

bool BabeGrid::eventFilter(QObject *obj, QEvent *event)
{
    if(autoAdjust)
        if(obj==this && event->type()==QEvent::Resize)
        {
            this->adjustGrid();
            event->accept();
        }

    return QWidget::eventFilter(obj, event);
}

void BabeGrid::adjustGrid()
{
    qDebug()<<"frameWidth:"<<this->frameWidth();
    auto scrollSize = this->verticalScrollBar()->size().width()+1;
    auto gridSize = this->size().width()-scrollSize-(this->frameWidth()*2);
    auto amount = gridSize/(static_cast<int>(this->albumSize+this->albumSpacing));
    auto leftSpace = gridSize-amount*static_cast<int>(this->albumSize);
    //    qDebug()<<"ther's space for: "<< amount <<" in "<<gridSize<<" and space left is"<<leftSpace;
    if(gridSize>static_cast<int>(this->albumSize))
        this->setGridSize(QSize(static_cast<int>(this->albumSize)+(leftSpace/amount),static_cast<int>(this->albumSize+this->albumSpacing)));
}

void BabeGrid::setUpActions()
{
    this->order  = new QAction("Go Descending",this);
    this->addAction(order);
    connect(order, &QAction::triggered,[this]()
    {
        if(this->ascending)
        {
            this->sortItems(Qt::DescendingOrder);
            order->setText("Go Ascending");
            this->ascending=!this->ascending;
        }else
        {
            this->sortItems(Qt::AscendingOrder);
            order->setText("Go Descending");
            this->ascending=!this->ascending;
        }
    });


    auto hideLabels = new QAction("Hide titles",this);
    this->addAction(hideLabels);
    connect(hideLabels, &QAction::triggered,[hideLabels,this]()
    {
        if (hideLabels->text().contains("Hide titles"))
        {
            this->showLabels(false);
        }
        else
        {
            this->showLabels(true);
        }

    });

    auto zoomIn = new QAction("Zoom in",this);
    zoomIn->setShortcut(tr("CTRL++"));
    this->addAction(zoomIn);
    connect(zoomIn, &QAction::triggered,[this]()
    {
        if(albumSize+5<=Bae::MAX_MID_ALBUM_SIZE)
        {
            this->setAlbumsSize(this->albumSize+5);
            this->adjustGrid();

        }

    });

    QAction *zoomOut = new QAction("Zoom out",this);
    zoomOut->setShortcut(tr("CTRL+-"));
    this->addAction(zoomOut);
    connect(zoomOut, &QAction::triggered,[this]()
    {
        if(albumSize-5>=Bae::MAX_MIN_ALBUM_SIZE)
        {
            this->setAlbumsSize(this->albumSize-5);
            this->adjustGrid();

        }
    });
}


