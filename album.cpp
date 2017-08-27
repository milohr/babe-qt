#include "album.h"


Album::Album(QWidget *parent) : QLabel(parent)
{

}

void Album::createAlbum(const QString &artist, const QString &album, const QString &imagePath,const Bae::ALbumSizeHint &widgetSize, const int &widgetRadius, const bool &isDraggable)
{
    switch (widgetSize)
    {
    case Bae::BIG_ALBUM:
        this->size=Bae::getWidgetSizeHint(Bae::BIG_ALBUM_FACTOR,Bae::BIG_ALBUM);
        this->subSize=Bae::BIG_ALBUM_FACTOR_SUBWIDGET;
        break;
    case Bae::MEDIUM_ALBUM:
        this->size=Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::MEDIUM_ALBUM);
        this->subSize=Bae::MEDIUM_ALBUM_FACTOR_SUBWIDGET;
        break;
    case Bae::SMALL_ALBUM:
        this->size=Bae::getWidgetSizeHint(Bae::SMALL_ALBUM_FACTOR,Bae::SMALL_ALBUM);
        this->subSize=Bae::SMALL_ALBUM_FACTOR_SUBWIDGET;
        break;
    }

    this->setFixedSize(size,size);
    this->border_radius=widgetRadius;
    this->draggable=isDraggable;
    this->imagePath=imagePath;
    this->borderQColor = this->palette().color(QPalette::BrightText).name();

    if (!imagePath.isEmpty()) this->putPixmap(imagePath);
    else this->putDefaultPixmap();

    auto layout = new QHBoxLayout();
    widget = new QWidget(this);
    widget->setLayout(layout);

    //    widget->setStyleSheet( QString(" background: rgba(0,0,0,150); border-top: 1px solid rgba(%1,%1,%1,120); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(this->palette().color(QPalette::WindowText).blue()), QString::number(border_radius-1),QString::number(border_radius-1)));
    widget->setStyleSheet( QString(" background: rgba(0,0,0,150); border-top: 1px solid rgba(%1,%1,%1,150); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(0), QString::number(border_radius-1),QString::number(border_radius-1)));
    title = new ScrollText(this);

    title->setMaxSize(size+10);

    title->setStyleSheet("QLabel{background:transparent; color:white; border:none;}");

    layout->addStretch();
    layout->addWidget(title);
    layout->addStretch();

    playBtn = new QToolButton(this);
    connect(playBtn,&QToolButton::clicked,[this]() { emit playAlbum(this->albumMap); });

    playBtn->installEventFilter(this);
    playBtn->setIcon(QIcon(":Data/data/playBtn.svg"));
    playBtn->setStyleSheet("QToolButton{border:none;}");
    playBtn->setAutoRaise(true);
    playBtn->setVisible(false);

    this->setSize(size);

    this->setTitle(artist,album);

    emit albumCreated(this);
}

void Album::setUpMenu()
{
    auto contextMenu = new QMenu(this);
    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    auto babeIt = new QAction("Babe it \xe2\x99\xa1",contextMenu);
    connect(babeIt, SIGNAL(triggered()), this, SLOT(babeIt_action()));
    this->addAction(babeIt);

    auto removeIt = new QAction("Remove",contextMenu);
    connect(removeIt, SIGNAL(triggered()), this, SLOT(removeIt_action()));
    this->addAction(removeIt);

    auto artIt = new QAction("Change art...",contextMenu);
    connect(artIt, SIGNAL(triggered()), this, SLOT(artIt_action()));
    this->addAction(artIt);
}

bool Album::eventFilter(QObject * watched, QEvent * event)
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

void Album::babeIt_action() { emit babeAlbum_clicked(this->albumMap); }

void Album::artIt_action()
{
    qDebug()<<"Change art of album"<<this->artist<<this->album;
    QString path = QFileDialog::getOpenFileName(this, tr("Select Music Files"),Bae::CachePath, tr("Image Files (*.png *.jpg *.bmp)"));
    if(!path.isEmpty())
    {
        putPixmap(path);
        this->albumMap.insert(ART,path);
        emit changedArt(this->albumMap);
    }
}

void Album::removeIt_action()
{
    qDebug()<<"Remove album"<<this->artist<<this->album;
}


int Album::getSize()
{
    return this->size;
}

void Album::saturatePixmap(const int &value)
{
    auto saturationValue = value;
    this->unsaturated = this->getPixmap();

    auto image = this->getPixmap().toImage();


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

    this->putPixmap(QPixmap::fromImage(image));
}

void Album::restoreSaturation()
{
    if(!unsaturated.isNull())this->putPixmap(this->unsaturated);
}

void Album::setSize(const int &value)
{
    this->size=value;
    this->setFixedSize(size,size);
    this->widget->setMinimumWidth(size-2);
    this->widget->setGeometry(0,size-30,size,30);
    this->playBtn->setIconSize(QSize(static_cast<int>(size*subSize),static_cast<int>(size*subSize)));
    this->playBtn->setGeometry((size/2)-static_cast<int>((size*subSize)/2),(size/2)-static_cast<int>((size*subSize)/2),playBtn->iconSize().width(),playBtn->iconSize().width());
}

void Album::paintEvent(QPaintEvent *event)
{
    event->accept();

    QBrush brush(Qt::yellow);
    if(!image.isNull())
        brush.setTexture(image.scaled(size,size,Qt::KeepAspectRatio));

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);

    if(!borderColor) painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0,0, size, size, border_radius, border_radius);
}

QPixmap Album::getPixmap()
{
    return image;
}


void Album::putPixmap(const QByteArray &pix)
{
    if(!pix.isEmpty()) this->image.loadFromData(pix);
    else this->image.load(":Data/data/cover.svg");

    this->setPixmap(image);
}

void Album::putPixmap(const QPixmap &pix)
{
    this->image=pix;
    this->setPixmap(image);
}

void Album::putPixmap(const QString &path)
{
    if(!path.isEmpty()) this->image.load(path);
    else this->image.load(":Data/data/cover.svg");

    this->setPixmap(image);
}

void Album::putDefaultPixmap()
{
    this->image.load(":Data/data/cover.svg");
    this->setPixmap(image);
}

QString Album::getTitle() { return title->text(); }

QString Album::getArtist() { return this->artist; }

QString Album::getAlbum() { return this->album; }

QString Album::getBGcolor() { return this->bgColor; }

void Album::setArtist(const QString &artistTitle) { this->artist=artistTitle; }

void Album::setAlbum(const QString &albumTitle) { this->album=albumTitle; }

void Album::setBGcolor(const QString &bgColor)
{
    this->bgColor=bgColor;

    QColor color;
    color.setNamedColor(bgColor);
    image.fill(color);
    this->setPixmap(image);
}

void Album::setTitle(const QString &artistTitle, const QString &albumTitle)
{
    this->artist = artistTitle;
    this->album = albumTitle;

    albumMap.insert(ARTIST,this->artist);
    albumMap.insert(ALBUM, this->album);

    QString str = album.isEmpty()? artist : album+" - "+artist;
    title->setText(str);
}

void Album::setTitleGeometry(const int &x, const int &y, const int &w, const int &h) { widget->setGeometry(x,y,w,h); }


void Album::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(event->button()==Qt::LeftButton)
        emit albumCoverDoubleClicked(this->albumMap);

    QLabel::mouseDoubleClickEvent(event);
}

void Album::mousePressEvent(QMouseEvent * event)
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

void Album::mouseMoveEvent(QMouseEvent *event)
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
    QLabel::mouseMoveEvent(event);
}

void Album::performDrag()
{
    emit albumDragStarted();
    qDebug()<<"trying to drag and album";
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(album.isEmpty()? this->artist : this->album+" /by/ "+this->artist);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(image.scaled(size/2,size/2,Qt::KeepAspectRatio));
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
        emit albumDragged();

}

void Album::enterEvent(QEvent *event)
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

void Album::leaveEvent(QEvent *event)
{
    emit albumCoverLeave();
    event->accept();
    if(showPlayBtn)
        playBtn->setVisible(false);

    widget->setStyleSheet( QString(" background: rgba(0,0,0,150); border-top: 1px solid rgba(%1,%1,%1,150); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(0), QString::number(border_radius-1),QString::number(border_radius-1)));

}

void Album::showTitle(const bool &state)
{
    this->visibleTitle=state;

    if(visibleTitle)
        this->widget->setVisible(true);
    else if(!visibleTitle) this->widget->setVisible(false);
}
