#include "album.h"


Album::Album(QString imagePath, int widgetSize, int widgetRadius, bool isDraggable, QWidget *parent) : QLabel(parent)
{    
    this->size=widgetSize;
    this->setFixedSize(size,size);
    this->border_radius=widgetRadius;
    this->draggable=isDraggable;
    this->imagePath=imagePath;

    if (!imagePath.isEmpty()) this->putPixmap(imagePath);
    else this->putDefaultPixmap();
    //image.scaled(120,120,Qt::KeepAspectRatio);
    //this->setPixmap(image.scaled(size,size,Qt::KeepAspectRatio));
    //this->setToolTip(getTitle());
    auto layout = new QHBoxLayout();
    widget = new QWidget(this);
    widget->setLayout(layout);
    widget->setMinimumWidth(size-2);
    widget->setGeometry(1,size-31,size-2,30);
    //widget->setStyleSheet("background-color: rgba(0,0,0,150)");
    //widget->setStyleSheet( QString(" background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 rgba(0, 0, 0, 200), stop: 0.4 rgba(0, 0, 0, 150), stop:1 rgb(0, 0, 0, 40)); border-top: 1px solid #333; border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%1px; border-bottom-left-radius:%2px;").arg( QString::number(border_radius),QString::number(border_radius)));
    widget->setStyleSheet( QString(" background: rgba(0,0,0,150); border-top: 1px solid rgba(%1,%1,%1,120); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(this->palette().color(QPalette::WindowText).blue()), QString::number(border_radius-1),QString::number(border_radius-1)));
    //widget->setStyleSheet("background-color: rgba(0,0,0,150); border-top: 1px solid #333;");
    //this->titleVisible(false);

    title = new ScrollText(this);
    title->setMaxSize(size+10);

    auto *left_spacer = new QWidget(this);
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget(this);
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    title->setStyleSheet("QLabel{background:transparent; color:white; border:none;}");
    right_spacer->setStyleSheet("background:transparent;  border:none;");
    left_spacer->setStyleSheet("background:transparent;  border:none;");

    layout->addWidget(left_spacer);
    layout->addWidget(title);
    layout->addWidget(right_spacer);


    playBtn = new QToolButton(this);
    playBtn->installEventFilter(this);
    connect(playBtn,SIGNAL(clicked()),this,SLOT(playBtn_clicked()));
    playBtn->setIcon(QIcon(":Data/data/playBtn.svg"));
    playBtn->setIconSize(QSize(48,48));
    playBtn->setGeometry((size/2)-24,(size/2)-24,playBtn->iconSize().width(),playBtn->iconSize().width());
    playBtn->setStyleSheet("QToolButton{border:none;}");
    //playBtn->setStyleSheet("QToolButton{ } QToolButton:hover{background: #333; border:1px solid black; border-radius:24px; } ");
    playBtn->setAutoRaise(true);
    playBtn->setVisible(false);


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
    QString path = QFileDialog::getOpenFileName(this, tr("Select Music Files"),BaeUtils::getCachePath(), tr("Image Files (*.png *.jpg *.bmp)"));
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

void Album::playBtn_clicked() { emit playAlbum(this->albumMap); }

void Album::setSize(int value)
{
    this->size=value;
    this->setFixedSize(size,size);
    this->widget->setMinimumWidth(size-2);
    this->widget->setGeometry(1,size-31,size-2,30);
    this->playBtn->setIconSize(QSize(static_cast<int>(size*0.4),static_cast<int>(size*0.4)));
    this->playBtn->setGeometry((size/2)-static_cast<int>((size*0.4)/2),(size/2)-static_cast<int>((size*0.4)/2),playBtn->iconSize().width(),playBtn->iconSize().width());
}

void Album::paintEvent(QPaintEvent *event)
{
    event->accept();

    QBrush brush(Qt::yellow);
    if(!image.isNull())
        brush.setTexture(image.scaled(size,size,Qt::KeepAspectRatio));

    // brush.setStyle(Qt::no);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);
    if (!borderColor)painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0,0, size, size, border_radius, border_radius);
}

QPixmap Album::getPixmap() { return image; }

void Album::putPixmap(QByteArray pix)
{
    if(!pix.isEmpty()) this->image.loadFromData(pix);
    else this->image.load(":Data/data/cover.svg");

    this->setPixmap(image);
}

void Album::putPixmap(QString path)
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

void Album::setArtist(QString artist) { this->artist=artist; }

void Album::setAlbum(QString album) { this->album=album; }

void Album::setBGcolor(QString bgColor)
{
    this->bgColor=bgColor;

    QColor color;
    color.setNamedColor(bgColor);
    image.fill(color);
    this->setPixmap(image);
}

void Album::setTitle(QString _artist, QString _album)
{
    this->artist = _artist;
    this->album = _album;

    albumMap.insert(ARTIST,this->artist);
    albumMap.insert(ALBUM, this->album);

    QString str = album.isEmpty()? artist : album+" - "+artist;
    title->setText(str);
}

void Album::setTitleGeometry(int x, int y, int w, int h) { widget->setGeometry(x,y,w,h); }

void Album::titleVisible(bool state)
{
    if(state) widget->setVisible(true);
    else widget->setVisible(false);
}

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
    //Q_UNUSED(event);
    //title->show();
    //title->setSpeed(1);
    //  widget->setStyleSheet("background:rgba(180, 225, 230, 150)");
    //  this->setStyleSheet("border:1px solid #f85b79");
    // qDebug()<<"entered the album cover";
    event->accept();
    playBtn->setVisible(true);
    playBtn->setToolTip("Play all - "+artist+" "+album);
    // this->titleVisible(true);

    emit albumCoverEnter();
}

void Album::leaveEvent(QEvent *event)
{
    //Q_UNUSED(event);
    //title->hide();
    //title->setSpeed(0);
    // title->reset();
    // widget->setStyleSheet("background-color: rgba(0,0,0,150);");
    // this->setStyleSheet("border:1px solid #333");
    //  qDebug()<<"left the album cover";
    event->accept();
    playBtn->setVisible(false);
    //this->titleVisible(false);

    emit albumCoverLeft();
}
