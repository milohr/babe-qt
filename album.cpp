#include "album.h"


Album::Album(QString imagePath, int widgetSize, int widgetRadius, bool isDraggable,bool plain, QWidget *parent) : QLabel(parent)
{
    //this->setMaximumSize(100,100);
    //this->set
    //this->setMinimumSize(120,120);
    //this->setStyleSheet("QLabel{border:transparent} QLabel:hover{border:1px solid #3daee9} QLabel:pressed{border:1px solid red}");

    this->size=widgetSize;
    this->setFixedSize(size,size);
    this->border_radius=widgetRadius;
    this->setFixedSize(size,size);
    this->draggable=isDraggable;
    this->isPlain=plain;
    this->imagePath=imagePath;


    if(!plain)
    {

        if (!imagePath.isEmpty()) image.load(imagePath);
        //image.scaled(120,120,Qt::KeepAspectRatio);
        //this->setPixmap(image.scaled(size,size,Qt::KeepAspectRatio));
        //this->setToolTip(getTitle());
        widget = new QWidget(this);
        auto layout = new QHBoxLayout();
        widget->setLayout(layout);


        //widget->setGeometry(0,size-30,size,30);
        // widget->setMinimumWidth(size);
        //title->setMaximumWidth(size);
        //title->setFixedWidth(size);
        //widget->setMaximumWidth(size);



        auto contextMenu = new QMenu(this);
        this->setContextMenuPolicy(Qt::ActionsContextMenu);


        auto babeIt = new QAction("Babe it \xe2\x99\xa1",contextMenu);
        this->addAction(babeIt);

        auto removeIt = new QAction("Remove",contextMenu);
        this->addAction(removeIt);

        auto artIt = new QAction("Change art...",contextMenu);
        this->addAction(artIt);

        connect(babeIt, SIGNAL(triggered()), this, SLOT(babeIt_action()));
        connect(removeIt, SIGNAL(triggered()), this, SLOT(removeIt_action()));
        connect(artIt, SIGNAL(triggered()), this, SLOT(artIt_action()));
        //connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(setUpContextMenu()));

        //connect(babeIt, SIGNAL(triggered()), this, SLOT(uninstallAppletClickedSlot()));



        title = new ScrollText(this);
        title->setMaxSize(size);
        //title->setMaxSize(size);
        //title->hide();
        auto *left_spacer = new QWidget(this);
        left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        auto *right_spacer = new QWidget(this);
        right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        playBtn = new QToolButton(this);
        playBtn->setIcon(QIcon(":Data/data/playBtn.svg"));
        playBtn->setIconSize(QSize(48,48));
        playBtn->setGeometry((size/2)-24,(size/2)-24,48,48);
        playBtn->setStyleSheet("QToolButton{border-radius:2px;} QToolButton:hover{background: url(':Data/data/playBtn_hover.svg') top center no-repeat;} ");
        playBtn->setAutoRaise(true);
        QObject::connect(playBtn,SIGNAL(clicked()),this,SLOT(playBtn_clicked()));
        playBtn->hide();

        layout->addWidget(left_spacer);
        layout->addWidget(title);
        layout->addWidget(right_spacer);
        widget->setMinimumWidth(size-2);
        //this->setStyleSheet("border:none");
        widget->setGeometry(1,size-31,size-2,30);
        //widget->setStyleSheet("background-color: rgba(0,0,0,150)");
        //widget->setStyleSheet( QString(" background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 rgba(0, 0, 0, 200), stop: 0.4 rgba(0, 0, 0, 150), stop:1 rgb(0, 0, 0, 40)); border-top: 1px solid #333; border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%1px; border-bottom-left-radius:%2px;").arg( QString::number(border_radius),QString::number(border_radius)));

        widget->setStyleSheet( QString(" background: rgba(0,0,0,150); border-top: 1px solid rgba(%1,%1,%1,120); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(this->palette().color(QPalette::WindowText).blue()), QString::number(border_radius),QString::number(border_radius)));
        //widget->setStyleSheet("background-color: rgba(0,0,0,150); border-top: 1px solid #333;");

        title->setStyleSheet("background:transparent; color:white; border:none;");
        right_spacer->setStyleSheet("background:transparent;  border:none;");
        left_spacer->setStyleSheet("background:transparent;  border:none;");
    }

}


void Album::babeIt_action()
{
    qDebug()<<"Babe album"<<this->artist<<this->album;
}

void Album::artIt_action()
{
    qDebug()<<"Change art of album"<<this->artist<<this->album;
    QString path = QFileDialog::getOpenFileName(this, tr("Select Music Files"),getCachePath(), tr("Image Files (*.png *.jpg *.bmp)"));
    if(!path.isEmpty())
    {
        putPixmap(path);
        emit changedArt(path,artist, album);

    }

}

void Album::removeIt_action()
{
    qDebug()<<"Remove album"<<this->artist<<this->album;

}

void Album::playBtn_clicked()
{
    emit playAlbum(artist,album);
}

void Album::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QBrush brush(Qt::yellow);


    if(!image.isNull())
        brush.setTexture(image.scaled(size,size,Qt::KeepAspectRatio));



    // brush.setStyle(Qt::no);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);
    if (!borderColor)painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0,0, size, size, border_radius, border_radius);

    //this->setStyleSheet("border:1px solid red;");


}

QPixmap Album::getPixmap()
{
    return image;
}

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

QString Album::getTitle()
{
    return title->text();
}

QString Album::getArtist()
{
    return this->artist;
}

QString Album::getAlbum()
{
    return this->album;
}


QString Album::getBGcolor()
{
    return this->bgColor;
}

void Album::setArtist(QString artist)
{
    this->artist=artist;
}

void Album::setAlbum(QString album)
{
    this->album=album;
}


void Album::setBGcolor(QString bgColor)
{
    this->bgColor=bgColor;

    QColor color;
    color.setNamedColor(bgColor);
    image.fill(color);

    this->setPixmap(image);


}

void Album::setTitle()
{
    QString str = album.isEmpty()? artist : album+" - "+artist;
    title->setText(str);
    // scrollText->setText(album+" - "+artist);
    //this->setToolTip(album+" - "+artist);
}


void Album::setSize(int value)
{
    this->size=value;
    //widget->setGeometry(0,90,size,30);


}

void Album::setTitleGeometry(int x, int y, int w, int h)
{
    widget->setGeometry(x,y,w,h);
}
void Album::titleVisible(bool state)
{
    if(state) widget->show();
    else widget->hide();
}

void Album::mousePressEvent ( QMouseEvent * evt)
{
    //qDebug()<<"the cover art was clicked: "<<getTitle();
    if(evt->button()==Qt::LeftButton && !draggable)
    {


        emit albumCoverClicked({artist,album,bgColor});
    }else
    {
        evt->accept();
        QLabel::mousePressEvent(evt);
    }

    // evt->ContextMenu()

}





void Album::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    //title->show();
    //title->setSpeed(1);
    //  widget->setStyleSheet("background:rgba(180, 225, 230, 150)");
    //  this->setStyleSheet("border:1px solid #f85b79");
    // qDebug()<<"entered the album cover";
    if(!isPlain)
    {
        playBtn->show();
        playBtn->setToolTip("Play all - "+artist+" "+album);
    }

    event->accept();
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
    if(!isPlain)
    {
        playBtn->hide();
        event->accept();
        emit albumCoverLeft();
    }
}
