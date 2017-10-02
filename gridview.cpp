#include "gridview.h"

GridView::GridView(QWidget *parent) : QListWidget(parent)
{
    this->albumSize = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::MEDIUM_ALBUM);

    this->installEventFilter(this);
    this->setObjectName("grid");
    this->setMinimumHeight(albumSize);
    this->setViewMode(QListWidget::IconMode);
    this->setResizeMode(QListWidget::Adjust);
    this->setWrapping(true);
    this->setAcceptDrops(false);
    this->setDragDropMode(QAbstractItemView::DragOnly);
    this->setFrameShape(QFrame::NoFrame);
    this->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );
    this->setSizeAdjustPolicy(QListWidget::AdjustToContentsOnFirstShow);
    this->setStyleSheet("QListWidget,QListWidget::item,QListWidget::item:selected,QListWidget::item:selected:active {background:transparent; color:transparent; }");
    this->setGridSize(QSize(this->albumSize+this->albumSpacing,this->albumSize+this->albumSpacing));
    this->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

    this->setUpActions();

}

void GridView::addAlbum(const Bae::DB &albumMap)
{

    if(!this->albumsMap.contains(albumMap))
    {
        auto album= new Album(this);
        album->createAlbum(albumMap,Bae::MEDIUM_ALBUM,4,true);

        this->albumsMap.insert(album->getAlbumMap(),album);

        connect(album,&Album::albumCoverClicked,[&](const Bae::DB &albumMap)
        {
            emit this->albumClicked(albumMap);
        });

        auto shadow = new QGraphicsDropShadowEffect();
        shadow->setColor(QColor(0, 0, 0, 140));
        shadow->setBlurRadius(9);
        shadow->setOffset(3,5);

        album->setGraphicsEffect(shadow);
        album->borderColor=false;
        album->setUpMenu();

        connect(album,&Album::albumCoverDoubleClicked, [this] (const Bae::DB &albumMap)
        {
            emit this->albumDoubleClicked(albumMap);
        });

        connect(album,&Album::playAlbum, [this] (const Bae::DB info) { emit this->playAlbum(info); });
        connect(album,&Album::babeAlbum,[this] (const Bae::DB info) { emit this->babeAlbum(info); });
        connect(album,&Album::albumDragStarted,[this](){emit this->dragAlbum();});
        connect(album,&Album::albumCoverEnter,[=]()
        {
            if(!album->visibleTitle) album->showTitle(true);
            shadow->setColor(QColor(0, 0, 0, 180));
            shadow->setOffset(2,3);
        });
        connect(album, &Album::albumCoverLeave,[=]()
        {
            if(!album->visibleTitle) album->showTitle(false);
            shadow->setColor(QColor(0, 0, 0, 140));
            shadow->setOffset(3,5);
        });


        auto item = new QListWidgetItem();
        this->itemsList.push_back(item);
        item->setSizeHint(QSize(album->getSize(),album->getSize()));

        if(album->getAlbum().isEmpty() && !album->getArtist().isEmpty()) item->setText(album->getArtist());
        else item->setText(album->getAlbum()+" "+album->getArtist());

        item->setTextAlignment(Qt::AlignCenter);
        this->addItem(item);
        this->setItemWidget(item,album);
        emit this->albumReady();
    }
//    this->sortItems(Qt::AscendingOrder);
}

void GridView::flushGrid()
{
    this->albumsMap.clear();
    this->clear();
}

void GridView::setAlbumsSize(const int &value)
{
    this->albumSize=value;
    /*slider->setToolTip(QString::number(value));
                QToolTip::showText( slider->mapToGlobal( QPoint( 0, 0 ) ), QString::number(value) );*/
    for(auto album : albumsMap.values())
    {
        album->setSize(albumSize);
        this->setGridSize(QSize(albumSize+this->albumSpacing,albumSize+this->albumSpacing));
        this->update();
    }

    for(auto item : itemsList) item->setSizeHint(QSize(albumSize, albumSize));
}

bool GridView::eventFilter(QObject *obj, QEvent *event)
{
    if(obj==this && event->type()==QEvent::Resize)
        this->adjustGrid();

    return QWidget::eventFilter(obj, event);
}

void GridView::adjustGrid()
{
    auto scrollSize = this->verticalScrollBar()->size().width()+1;
    auto gridSize = this->size().width()-scrollSize;
    auto amount = (gridSize/(albumSize+this->albumSpacing));
    auto leftSpace = gridSize-amount*albumSize;
    if(gridSize>this->albumSize)
        this->setGridSize(QSize(albumSize+(leftSpace/amount),albumSize+this->albumSpacing));
}

void GridView::setUpActions()
{
    this->order  = new QAction("Go Descending");
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
            for(auto album: albumsMap.values())
                album->showTitle(false);

            hideLabels->setText("Show titles");
            this->hiddenLabels = !this->hiddenLabels ;
        }
        else
        {
            for(auto album: albumsMap.values())
                album->showTitle(true);
            hideLabels->setText("Hide titles");
            this->hiddenLabels = !this->hiddenLabels ;
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


