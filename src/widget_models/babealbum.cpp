#include "babealbum.h"


BabeAlbum::BabeAlbum(const DB &info, const AlbumSizeHint &widgetSize, const uint8_t &widgetRadius, const bool &isDraggable, QWidget *parent) : QLabel(parent)
{

    connect(this,&BabeAlbum::changedArt,&connection,&CollectionDB::insertArtwork);

    switch (widgetSize)
    {
    case Bae::AlbumSizeHint::BIG_ALBUM:
    {
        this->size = Bae::getWidgetSizeHint(Bae::BIG_ALBUM_FACTOR,Bae::AlbumSizeHint::BIG_ALBUM);
        this->subSize = Bae::BIG_ALBUM_FACTOR_SUBWIDGET;
        break;
    }
    case Bae::AlbumSizeHint::MEDIUM_ALBUM:
    {
        this->size = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::AlbumSizeHint::MEDIUM_ALBUM);
        this->subSize = Bae::MEDIUM_ALBUM_FACTOR_SUBWIDGET;
        break;
    }
    case Bae::AlbumSizeHint::SMALL_ALBUM:
    {
        this->size = Bae::getWidgetSizeHint(Bae::SMALL_ALBUM_FACTOR,Bae::AlbumSizeHint::SMALL_ALBUM);
        this->subSize = Bae::SMALL_ALBUM_FACTOR_SUBWIDGET;
        break;
    }
    }

    this->albumMap = info;

    auto artist = this->albumMap[Bae::KEY::ARTIST];
    auto album = this->albumMap[Bae::KEY::ALBUM];
    auto artwork = albumMap[Bae::KEY::ARTWORK];

    this->putPixmap(artwork);
    this->setFixedSize(static_cast<int>(size), static_cast<int>(size));
    this->border_radius = widgetRadius;
    this->draggable = isDraggable;
    this->borderQColor = this->palette().color(QPalette::BrightText).name();

    auto layout = new QHBoxLayout;
    widget = new QWidget(this);
    widget->setLayout(layout);

    //    widget->setStyleSheet( QString(" background: rgba(0,0,0,150); border-top: 1px solid rgba(%1,%1,%1,120); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(this->palette().color(QPalette::WindowText).blue()), QString::number(border_radius-1),QString::number(border_radius-1)));
    widget->setStyleSheet( QString(" background: rgba(0,0,0,150); border-top: 1px solid rgba(%1,%1,%1,150); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(0), QString::number(border_radius-1),QString::number(border_radius-1)));
    this->title = new ScrollText(this);

    title->setMaxSize(size+10);
    title->setStyleSheet("QLabel{background:transparent; color:white; border:none;}");

    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();

    this->playBtn = new QToolButton(this);
    connect(playBtn,&QToolButton::clicked,[this]() { emit playAlbum(this->albumMap); });

    playBtn->installEventFilter(this);
    playBtn->setIcon(QIcon(":Data/data/playBtn.svg"));
    playBtn->setStyleSheet("QToolButton{border:none;}");
    playBtn->setAutoRaise(true);
    playBtn->setVisible(false);

    this->setSize(size);
    this->setTitle(artist,album);

    this->contextMenu = new QMenu(this);
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
}

BabeAlbum::~BabeAlbum()
{
    qDebug()<<"DELETING BABEALBUM";
}

void BabeAlbum::setUpMenu()
{
    auto babeIt = new QAction("Babe it \xe2\x99\xa1",this->contextMenu);
    connect(babeIt, SIGNAL(triggered()), this, SLOT(babeIt_action()));
    this->addAction(babeIt);

    auto removeIt = new QAction("Remove",this->contextMenu);
    connect(removeIt, SIGNAL(triggered()), this, SLOT(removeIt_action()));
    this->addAction(removeIt);

    auto artIt = new QAction("Change art...",this->contextMenu);
    connect(artIt, SIGNAL(triggered()), this, SLOT(artIt_action()));
    this->addAction(artIt);
}

bool BabeAlbum::eventFilter(QObject * watched, QEvent * event)
{
    if (watched != playBtn) return false;

    if (event->type() == QEvent::Enter)
    {
        playBtn->setIcon(QIcon(":Data/data/playBtn_hover.svg"));
        return true;
    }

    if (event->type() == QEvent::Leave)
    {
        playBtn->setIcon(QIcon(":Data/data/playBtn.svg"));
        return true;
    }

    return false;
}

void BabeAlbum::babeIt_action() { emit babeAlbum(this->albumMap); }

void BabeAlbum::artIt_action()
{
    qDebug()<<"Change art of album"<<this->artist<<this->album;
    QString path = QFileDialog::getOpenFileName(this, tr("Select Music Files"),Bae::CachePath, tr("Image Files (*.png *.jpg *.bmp)"));
    if(!path.isEmpty())
    {
        this->putPixmap(path);
        this->albumMap.insert(Bae::KEY::ARTWORK,path);
        emit changedArt(this->albumMap);
    }
}

void BabeAlbum::removeIt_action()
{
    qDebug()<<"Remove album"<<this->artist<<this->album;
}


uint BabeAlbum::getSize()
{
    return this->size;
}

void BabeAlbum::saturatePixmap(const int &value)
{
    if(!this->pixmap()->isNull())
    {
        auto saturationValue = value;

        auto image = this->pixmap()->toImage();

        for(int i=0; i<image.width(); i++)
        {
            for(int j=0; j<image.height(); j++)
            {
                QColor color = image.pixelColor(i,j);
                if(color.black()<150)
                {

                    int hue = color.hue();
                    int saturation = color.saturation()+saturationValue;

                    // modify hue as you’d like and write back to the image
                    color.setHsv(hue, saturation>255?255:saturation, color.value(), color.alpha());
                    image.setPixelColor(i, j, color);
                }
            }
        }

        this->setPixmap(QPixmap::fromImage(image));
    }
}

void BabeAlbum::restoreSaturation()
{
    this->putPixmap(this->imagePath);
}

void BabeAlbum::setSize(const uint &value)
{
    this->size=value;
    this->setFixedSize(static_cast<int>(size),static_cast<int>(size));
    this->widget->setMinimumWidth(static_cast<int>(size-2)>=0?static_cast<int>(size-2):0);
    this->widget->setGeometry(0,static_cast<int>(size-30),static_cast<int>(size),30);
    this->playBtn->setIconSize(QSize(static_cast<int>(size*subSize),static_cast<int>(size*subSize)));
    this->playBtn->setGeometry(static_cast<int>(size/2)-static_cast<int>((size*subSize)/2),static_cast<int>(size/2)-static_cast<int>((size*subSize)/2),playBtn->iconSize().width(),playBtn->iconSize().width());
}

void BabeAlbum::paintEvent(QPaintEvent *event)
{
    event->accept();

    QBrush brush(Qt::yellow);
    if(!this->pixmap()->isNull())
        brush.setTexture(this->pixmap()->scaled(static_cast<int>(size),static_cast<int>(size),Qt::KeepAspectRatio));

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);

    if(!borderColor) painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0,0,static_cast<int>(size),static_cast<int>(size), border_radius, border_radius);
}


void BabeAlbum::putPixmap(const QString &path)
{   
    this->imagePath = path;
    QPixmap pix(imagePath);
    if(!pix.isNull())
        this->setPixmap(pix);
    else putDefaultPixmap();
}

void BabeAlbum::putDefaultPixmap()
{
    this->putPixmap(":Data/data/cover.svg");
}

QString BabeAlbum::getTitle() { return title->text(); }

QString BabeAlbum::getArtist() { return this->artist; }

QString BabeAlbum::getAlbum() { return this->album; }

QString BabeAlbum::getBGcolor() { return this->bgColor; }

Bae::DB BabeAlbum::getAlbumMap() { return this->albumMap; }

void BabeAlbum::setArtist(const QString &artistTitle) { this->artist=artistTitle; }

void BabeAlbum::setAlbum(const QString &albumTitle) { this->album=albumTitle; }

void BabeAlbum::setTitle(const QString &artistTitle, const QString &albumTitle)
{
    this->artist = artistTitle;
    this->album = albumTitle;

    albumMap.insert(Bae::KEY::ARTIST,this->artist);
    albumMap.insert(Bae::KEY::ALBUM, this->album);

    QString str = album.isEmpty()? artist : album+" - "+artist;
    title->setText(str);
}

void BabeAlbum::setTitleGeometry(const int &x, const int &y, const int &w, const int &h) { widget->setGeometry(x,y,w,h); }


void BabeAlbum::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(event->button()==Qt::LeftButton)
        emit albumCoverDoubleClicked(this->albumMap);

    QLabel::mouseDoubleClickEvent(event);
}

void BabeAlbum::mousePressEvent(QMouseEvent * event)
{
    if(event->type()!=QEvent::MouseButtonDblClick)
    {
        if(event->button()==Qt::LeftButton)
            emit albumCoverClicked(this->albumMap);
    }


    if(event->button()==Qt::LeftButton && draggable)
    {
        if (event->button() == Qt::LeftButton)
            startPos = event->pos();
    }

    QLabel::mousePressEvent(event);
}

void BabeAlbum::mouseMoveEvent(QMouseEvent *event)
{
    if(draggable)
    {

        if (event->buttons() & Qt::LeftButton)
        {
            int distance = (event->pos() - startPos).manhattanLength();
            if (distance >= QApplication::startDragDistance())
                performDrag();
        }
    }

    qDebug()<<event->globalPos();
    QLabel::mouseMoveEvent(event);
}

void BabeAlbum::performDrag()
{
    emit albumDragStarted();
    qDebug()<<"trying to drag and album";
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(album.isEmpty()? this->artist : this->album+" /by/ "+this->artist);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(this->pixmap()->scaled(size/2,size/2,Qt::KeepAspectRatio));
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
        emit albumDragged();
}

void BabeAlbum::enterEvent(QEvent *event)
{
    emit albumCoverEnter();
    event->accept();

    if(showPlayBtn)
    {
        playBtn->setVisible(true);
        playBtn->setToolTip("Play all - "+artist+" "+album);
    }


    widget->setStyleSheet( QString(" background: %4; border-top: 1px solid rgba(%1,%1,%1,255); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(0), QString::number(border_radius-1),QString::number(border_radius-1),"#000"));


}

void BabeAlbum::leaveEvent(QEvent *event)
{
    emit albumCoverLeave();
    event->accept();
    if(showPlayBtn)
        playBtn->setVisible(false);

    widget->setStyleSheet( QString(" background: rgba(0,0,0,150); border-top: 1px solid rgba(%1,%1,%1,150); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(0), QString::number(border_radius-1),QString::number(border_radius-1)));

}

void BabeAlbum::showTitle(const bool &state)
{
    this->visibleTitle = state;
    this->widget->setVisible(this->visibleTitle);
}
