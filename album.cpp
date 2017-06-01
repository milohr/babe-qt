#include "album.h"

#include <QApplication>
#include <QDrag>
#include <QEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QToolButton>

Album::Album(const QString &imagePath, int widgetSize, int widgetRadius, bool isDraggable, QWidget *parent) : QLabel(parent)
  ,m_size(widgetSize)
  ,m_borderRadius(widgetRadius)
  ,m_draggable(isDraggable)
  ,m_borderColor(false)
  ,m_imagePath(imagePath)
{
    setFixedSize(m_size, m_size);

    if (!m_imagePath.isEmpty())
        putPixmap(m_imagePath);
    else
        putDefaultPixmap();

    auto layout = new QHBoxLayout();
    widget = new QWidget(this);
    widget->setLayout(layout);
    widget->setMinimumWidth(m_size-2);
    widget->setGeometry(1, m_size-31, m_size-2, 30);
    widget->setStyleSheet(QString("background: rgba(0, 0, 0, 150); border-top: 1px solid rgba(%1, %1, %1, 120); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(this->palette().color(QPalette::WindowText).blue()), QString::number(m_borderRadius-1), QString::number(m_borderRadius-1)));

    m_title = new ScrollText(this);
    m_title->setMaxSize(m_size+10);

    auto *left_spacer = new QWidget(this);
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget(this);
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_title->setStyleSheet("QLabel{background:transparent; color:white; border:none;}");
    right_spacer->setStyleSheet("background:transparent; border:none;");
    left_spacer->setStyleSheet("background:transparent; border:none;");

    layout->addWidget(left_spacer);
    layout->addWidget(m_title);
    layout->addWidget(right_spacer);

    playBtn = new QToolButton(this);
    playBtn->installEventFilter(this);
    connect(playBtn, SIGNAL(clicked()), this, SLOT(playBtn_clicked()));
    playBtn->setIcon(QIcon(":Data/data/playBtn.svg"));
    playBtn->setIconSize(QSize(48, 48));
    playBtn->setGeometry((m_size/2)-24, (m_size/2)-24, playBtn->iconSize().width(), playBtn->iconSize().width());
    playBtn->setStyleSheet("QToolButton{border:none;}");
    playBtn->setAutoRaise(true);
    playBtn->setVisible(false);

    auto contextMenu = new QMenu(this);
    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    auto babeIt = new QAction("Babe it \xe2\x99\xa1", contextMenu);
    connect(babeIt, SIGNAL(triggered()), this, SLOT(babeIt_action()));
    this->addAction(babeIt);

    auto removeIt = new QAction("Remove", contextMenu);
    connect(removeIt, SIGNAL(triggered()), this, SLOT(removeIt_action()));
    this->addAction(removeIt);

    auto artIt = new QAction("Change art...", contextMenu);
    connect(artIt, SIGNAL(triggered()), this, SLOT(artIt_action()));
    this->addAction(artIt);
}

Album::~Album()
{
}

bool Album::eventFilter(QObject * watched, QEvent * event)
{
    if (watched != playBtn)
        return false;
    if (event->type() == QEvent::Enter) {
        playBtn->setIcon(QIcon(":Data/data/playBtn_hover.svg"));
        return true;
    }
    if (event->type() == QEvent::Leave) {
        playBtn->setIcon(QIcon(":Data/data/playBtn.svg"));
        return true;
    }
    return false;
}

void Album::babeIt_action()
{
    emit babeAlbum_clicked(m_albumMap);
}

void Album::artIt_action()
{
    QString path(QFileDialog::getOpenFileName(this, tr("Select Music Files"), BaeUtils::getCachePath(), tr("Image Files (*.png *.jpg *.bmp)")));
    if (!path.isEmpty()) {
        putPixmap(path);
        m_albumMap.insert(ART, path);
        emit changedArt(m_albumMap);
    }
}

void Album::removeIt_action()
{
}

void Album::playBtn_clicked()
{
    emit playAlbum(m_albumMap);
}

void Album::setSize(int value)
{
    m_size = value;
    setFixedSize(m_size, m_size);
    this->widget->setMinimumWidth(m_size-2);
    this->widget->setGeometry(1, m_size-31, m_size-2, 30);
    this->playBtn->setIconSize(QSize(static_cast<int>(m_size*0.4), static_cast<int>(m_size*0.4)));
    this->playBtn->setGeometry((m_size/2)-static_cast<int>((m_size*0.4)/2), (m_size/2)-static_cast<int>((m_size*0.4)/2), playBtn->iconSize().width(), playBtn->iconSize().width());
}

void Album::paintEvent(QPaintEvent *event)
{
    event->accept();
    QBrush brush(Qt::yellow);
    if (!image.isNull())
        brush.setTexture(image.scaled(m_size, m_size, Qt::KeepAspectRatio));
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);
    if (!m_borderColor)
        painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0, 0, m_size, m_size, m_borderRadius, m_borderRadius);
}

QPixmap Album::getPixmap()
{
    return image;
}

void Album::putPixmap(const QByteArray &pix)
{
    if (!pix.isEmpty())
        this->image.loadFromData(pix);
    else
        this->image.load(":Data/data/cover.svg");
    this->setPixmap(image);
}

void Album::putPixmap(const QString &path)
{
    if (!path.isEmpty())
        this->image.load(path);
    else
        this->image.load(":Data/data/cover.svg");
    this->setPixmap(image);
}

void Album::putDefaultPixmap()
{
    this->image.load(":Data/data/cover.svg");
    this->setPixmap(image);
}

QString Album::getTitle()
{
    return m_title->text();
}

QString Album::getArtist()
{
    return m_artist;
}

QString Album::getAlbum()
{
    return m_album;
}

QString Album::getBGColor()
{
    return m_bgColor;
}

void Album::setArtist(const QString &artist)
{
    m_artist = artist;
}

void Album::setAlbum(const QString &album)
{
    m_album = album;
}

void Album::setBGcolor(const QString &bgColor)
{
    m_bgColor = bgColor;
    QColor color;
    color.setNamedColor(m_bgColor);
    image.fill(color);
    setPixmap(image);
}

void Album::setBordercolor(bool color)
{
    m_borderColor = color;
}

void Album::setTitle(const QString &artist, const QString &album)
{
    m_artist = artist;
    m_album = album;

    m_albumMap.insert(ARTIST, m_artist);
    m_albumMap.insert(ALBUM, m_album);

    m_title->setText(album.isEmpty() ? artist : album + " - " + artist);
}

void Album::setTitleGeometry(int x, int y, int w, int h)
{
    widget->setGeometry(x, y, w, h);
}

void Album::titleVisible(bool state)
{
    widget->setVisible(state);
}

void Album::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
        emit albumCoverDoubleClicked(m_albumMap);
    QLabel::mouseDoubleClickEvent(event);
}

void Album::mousePressEvent(QMouseEvent * event)
{
    if (event->type() != QEvent::MouseButtonDblClick) {
        if (event->button() == Qt::LeftButton)
            emit albumCoverClicked(m_albumMap);
    }
    if (event->button() == Qt::LeftButton && m_draggable) {
        if (event->button() == Qt::LeftButton)
            m_startPos = event->pos();
    }
    QLabel::mousePressEvent(event);
}

void Album::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggable) {
        if (event->buttons() & Qt::LeftButton) {
            int distance = (event->pos() - m_startPos).manhattanLength();
            if (distance >= QApplication::startDragDistance())
                performDrag();
        }
    }
    QLabel::mouseMoveEvent(event);
}

void Album::performDrag()
{
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(m_album.isEmpty() ? m_artist : m_album + " /by/ " + m_artist);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(image.scaled(m_size/2, m_size/2, Qt::KeepAspectRatio));
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
        emit albumDragged();
}

void Album::enterEvent(QEvent *event)
{
    event->accept();
    playBtn->setVisible(true);
    playBtn->setToolTip("Play all - " + m_artist + " " + m_album);
    widget->setStyleSheet(QString("background: %4; border-top: 1px solid rgba(%1, %1, %1, 120); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(this->palette().color(QPalette::WindowText).blue()), QString::number(m_borderRadius-1), QString::number(m_borderRadius-1), "#000"));
    emit albumCoverEnter();
}

void Album::leaveEvent(QEvent *event)
{
    event->accept();
    playBtn->setVisible(false);
    widget->setStyleSheet(QString("background: rgba(0, 0, 0, 150); border-top: 1px solid rgba(%1, %1, %1, 120); border-top-left-radius:0; border-top-right-radius:0; border-bottom-right-radius:%2px; border-bottom-left-radius:%3px;").arg( QString::number(this->palette().color(QPalette::WindowText).blue()), QString::number(m_borderRadius-1), QString::number(m_borderRadius-1)));
    emit albumCoverLeft();
}
