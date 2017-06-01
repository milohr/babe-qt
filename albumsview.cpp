#include "albumsview.h"

#include <QAction>
#include <QComboBox>
#include <QDebug>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QListWidgetItem>
#include <QListWidget>
#include <QShortcut>
#include <QSplitter>
#include <QToolTip>
#include <QToolButton>

AlbumsView::AlbumsView(bool extraList, QWidget *parent) : QWidget(parent)
  ,m_albumsDB(AlbumsDB::instance())
  ,m_tracksDB(TracksDB::instance())
  ,m_artistsDB(ArtistsDB::instance())
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
    grid->setStyleSheet("QListWidget {background:transparent; padding-top:15px; padding-left:15px; }");
    grid->setGridSize(QSize(albumSize+10, albumSize+10));

    QAction *zoomIn = new QAction(this);
    zoomIn->setShortcut(tr("CTRL++"));
    connect(zoomIn, &QAction::triggered, [this]() {
        if (albumSize+5 <= 200) {
            this->setAlbumsSize(albumSize+5);
            slider->setValue(albumSize+5);
            slider->setSliderPosition(albumSize+5);
        }
    });

    QAction *zoomOut = new QAction(this);
    zoomOut->setShortcut(tr("CTRL+-"));
    connect(zoomOut, &QAction::triggered, [this]() {
        if (albumSize-5 >= 80) {
            this->setAlbumsSize(albumSize-5);
            slider->setValue(albumSize-5);
            slider->setSliderPosition(albumSize-5);
        }
    });

    this->addAction(zoomIn);
    this->addAction(zoomOut);

    auto utilsLayout = new QHBoxLayout();
    utilsLayout->setContentsMargins(0, 0, 0, 0);
    utilsLayout->setSpacing(0);

    utilsFrame = new QFrame();
    utilsFrame->setLayout(utilsLayout);
    utilsFrame->setFrameShape(QFrame::NoFrame);
    utilsFrame->setFrameShadow(QFrame::Plain);
    utilsFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    slider = new QSlider();
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(setAlbumsSize(int)));
    slider->setMaximum(200);
    slider->setMinimum(80);
    slider->setValue(albumSize);
    slider->setSliderPosition(albumSize);
    slider->setOrientation(Qt::Orientation::Horizontal);

    order = new QComboBox();
    connect(order, SIGNAL(currentIndexChanged(QString)), this, SLOT(orderChanged(QString)));
    order->setFrame(false);
    order->setContentsMargins(0, 0, 0, 0);

    order->addItem("Artist");
    order->addItem("Title");
    order->setCurrentIndex(1);

    utilsLayout->addWidget(order);

    albumTable = new BabeTable(this);
    albumTable->setFrameShape(QFrame::NoFrame);
    albumTable->horizontalHeader()->setVisible(false);
    albumTable->showColumn(BabeTable::TRACK);
    albumTable->showColumn(BabeTable::STARS);
    albumTable->hideColumn(BabeTable::ARTIST);
    albumTable->hideColumn(BabeTable::ALBUM);

    auto albumBox = new QGridLayout();
    albumBox->setContentsMargins(0, 0, 0, 0);
    albumBox->setSpacing(0);

    albumBox_frame = new QWidget(this);
    albumBox_frame->setLayout(albumBox);

    line_h = new QFrame(this);
    line_h->setFrameShape(QFrame::HLine);
    line_h->setFrameShadow(QFrame::Plain);
    line_h->setMaximumHeight(1);

    cover = new Album(":Data/data/cover.svg", 120, 0, true, this);
    connect(cover, &Album::playAlbum, [this](QMap<int, QString> info) { emit playAlbum(info); });
    connect(cover, &Album::changedArt, this, &AlbumsView::changedArt_cover);
    connect(cover, &Album::babeAlbum_clicked, this, &AlbumsView::babeAlbum);

    closeBtn = new QToolButton(cover);
    connect(closeBtn, SIGNAL(clicked()), SLOT(hideAlbumFrame()));
    closeBtn->setGeometry(2, 2, 16, 16);
    closeBtn->setIcon(QIcon::fromTheme("tab-close"));
    closeBtn->setAutoRaise(true);

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMaximumWidth(1);

    if (extraList) {
        this->extraList = true;
        artistList = new QListWidget(this);
        connect(artistList, SIGNAL(clicked(QModelIndex)), this, SLOT(filterAlbum(QModelIndex)));
        artistList->setFrameShape(QFrame::NoFrame);
        artistList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        artistList->setMaximumWidth(120);
        artistList->setAlternatingRowColors(true);

        albumBox->addWidget(cover, 0, 0, Qt::AlignTop);
        albumBox->addWidget(artistList, 1, 0, Qt::AlignLeft);
        albumBox->addWidget(line, 0, 1, 2, 1, Qt::AlignLeft);
        albumBox->addWidget(albumTable, 0, 2, 2, 1);
    } else {
        albumBox->addWidget(cover, 0, 0, Qt::AlignTop);
        albumBox->addWidget(line, 0, 1, Qt::AlignLeft);
        albumBox->addWidget(albumTable, 0, 2);
    }

    QSplitter *splitter = new QSplitter(parent);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(grid);
    splitter->addWidget(line_h);
    splitter->addWidget(albumBox_frame);

    layout->addWidget(splitter);
    albumBox_frame->hide(); line_h->hide();
    splitter->setSizes({0, 0, 0});
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    this->setLayout(layout);
}

AlbumsView::~AlbumsView()
{
}

void AlbumsView::hideAlbumFrame()
{
    albumTable->flushTable();
    albumBox_frame->hide();
    line_h->hide();
}

void AlbumsView::filterAlbum(const QModelIndex &index)
{
    QString coverTitle(cover->getArtist());
    QString album(index.data().toString());
    albumTable->flushTable();
    QVariantMap map({{"title", album}, {"artist", coverTitle}});
    albumTable->populateTableView(m_albumsDB->loadAlbums(map, -1, 0 , "album, track"), false, false);
    cover->setTitle(coverTitle, album);
    map.clear();
    foreach (const QVariant &entry, m_albumsDB->loadAlbums(map)) {
        map = entry.toMap();
        QString qcover(map.value("art").toString());
        if (!qcover.isEmpty())
            cover->putPixmap(qcover);
    }
}

void AlbumsView::setAlbumsSize(int value)
{
    albumSize = value;
    slider->setToolTip(QString::number(value));
    QToolTip::showText(slider->mapToGlobal(QPoint(0, 0)), QString::number(value));
    for (auto album : albumsList) {
        album->setSize(albumSize);
        grid->setGridSize(QSize(albumSize+10, albumSize+10));
        grid->update();
    }
    for (auto item : itemsList)
        item->setSizeHint(QSize(albumSize, albumSize));
}

void AlbumsView::albumHover()
{
}

void  AlbumsView::flushGrid()
{
    this->hideAlbumFrame();
    albumsList.clear();
    albums.clear();
    artists.clear();
    grid->clear();
}

int AlbumsView::getAlbumSize()
{
    return this->albumSize;
}

void AlbumsView::orderChanged(const QString &order)
{
    emit albumOrderChanged(order);
}

void AlbumsView::populateTableView(const QString &order)
{
    qDebug() << "ON POPULATE ALBUM VIEW:";
    QVariantMap map;
    foreach (const QVariant &item, m_albumsDB->loadAlbums(QVariantMap(), -1, 0 , order)) {
        map = item.toMap();
        QString art(":Data/data/cover.svg");
        QString album(map.value("title").toString());
        QString artist(map.value("artist").toString());

        if (!albums.contains(album + " " + artist)) {
            albums << album + " " + artist;

            QString qart(map.value("art").toString());
            if (!qart.isEmpty() && qart.compare("NULL") != 0)
                art = qart;
            else
                art = m_artistsDB->getArt(artist);

            auto artwork = new Album(art, albumSize, 4, true, this);
            albumsList.push_back(artwork);
            artwork->setBordercolor(true);
            artwork->setTitle(artist, album);

            connect(artwork, &Album::albumCoverClicked, this, &AlbumsView::getAlbumInfo);
            connect(artwork, &Album::albumCoverDoubleClicked, [this](QMap<int, QString> info) { emit albumDoubleClicked(info); });
            connect(artwork, &Album::playAlbum, [this](QMap<int, QString> info) { emit playAlbum(info); });
            connect(artwork, &Album::changedArt, this, &AlbumsView::changedArt_cover);
            connect(artwork, &Album::babeAlbum_clicked, this, &AlbumsView::babeAlbum);

            auto item = new QListWidgetItem();
            itemsList.push_back(item);
            item->setSizeHint(QSize(albumSize, albumSize));
            grid->addItem(item);
            grid->setItemWidget(item, artwork);
        }
    }
    emit populateCoversFinished();
}

void AlbumsView::babeAlbum(const QMap<int, QString> &info)
{
    emit babeAlbum_clicked(info);
}

void AlbumsView::populateTableViewHeads()
{
    qDebug() << "ON POPULATE HEADS VIEW:";
    QVariantList list(m_artistsDB->loadArtists(QVariantMap(), -1, 0, "title"));
    if (list.size()) {
        QString qart;
        QString art;
        QString artist;
        QVariantMap map;
        foreach (const QVariant &item, list) {
            map = item.toMap();
            art = ":Data/data/cover.svg";
            artist = map.value("title").toString();

            if (!artist.isEmpty() && !artists.contains(artist)) {
                artists << artist;
                qart = map.value("art").toString();

                if (!qart.isEmpty() && qart.compare("NULL") != 0)
                    art = qart;

                Album *album = new Album(art, albumSize, 4, true, this);
                albumsList.push_back(album);
                album->setBordercolor(true);
                album->setTitle(artist);

                connect(album, &Album::albumCoverClicked, this, &AlbumsView::getArtistInfo);
                connect(album, &Album::albumCoverDoubleClicked, [this] (QMap<int, QString> info) { emit albumDoubleClicked(info); });
                connect(album, &Album::playAlbum, [this](QMap<int, QString> info) { emit playAlbum(info); });
                connect(album, &Album::changedArt, this, &AlbumsView::changedArt_head);
                connect(album, &Album::babeAlbum_clicked, this, &AlbumsView::babeAlbum);

                auto item = new QListWidgetItem();
                itemsList.push_back(item);
                item->setSizeHint(QSize(albumSize, albumSize));

                grid->addItem(item);
                grid->setItemWidget(item, album);
            }
        }
    }
    emit populateHeadsFinished();
}

void AlbumsView::populateExtraList(const QString &artist)
{
    qDebug() << "ON POPULATE EXTRA LIST:";
    artistList->clear();
    QVariantMap map;
    foreach (const QVariant &entry, m_albumsDB->loadAlbums(QVariantMap({{"artist", artist}}), -1, 0, "title")) {
        map = entry.toMap();
        auto album = map.value("title").toString();
        auto item = new QListWidgetItem();
        item->setText(album);
        item->setTextAlignment(Qt::AlignCenter);
        artistList->addItem(item);
    }
}

void AlbumsView::changedArt_cover(const QMap<int, QString> &info)
{
    QString title(info[Album::ALBUM]);
    QString artist(info[Album::ARTIST]);
    m_albumsDB->update(QVariantMap({{"art", info[Album::ART]}}), QVariantMap({{"title", title}, {"artist", artist}}));
    m_collectionDB.updateTrackArt(info[Album::ART], info[Album::ARTIST], info[Album::ALBUM]);
}

void AlbumsView::changedArt_head(const QMap<int, QString> &info)
{
    QString title(info[Album::ARTIST]);
    m_artistsDB->update(QVariantMap({{"art", info[Album::ART]}}), QVariantMap({{"title", title}}));
    m_collectionDB.updateTrackArt(info[Album::ART], info[Album::ARTIST], info[Album::ALBUM]);
}

void AlbumsView::getArtistInfo(const QMap<int, QString> &info)
{
    albumBox_frame->show();
    line_h->show();
    QString artist(info[Album::ARTIST]);
    cover->setTitle(artist);
    albumTable->flushTable();
    albumTable->populateTableView(m_tracksDB->loadTracks(QVariantMap({{"artist", artist}}), -1, 0, "album, track"), false, false);
    auto art = m_artistsDB->getArt(artist);
    if (!art.isEmpty())
        cover->putPixmap(art);
    else
        cover->putDefaultPixmap();
    if (extraList)
        populateExtraList(artist);
}

void AlbumsView::getAlbumInfo(QMap<int, QString> info)
{
    albumBox_frame->setVisible(true);
    line_h->setVisible(true);
    QString album(info[Album::ALBUM]);
    QString artist(info[Album::ARTIST]);
    cover->setTitle(artist, album);
    albumTable->flushTable();
    albumTable->populateTableView(m_tracksDB->loadTracks(QVariantMap({{"artist", artist}, {"album", album}}), -1, 0, "album, track"), false, false);
    auto art = m_collectionDB.getAlbumArt(album, artist);
    art = art.isEmpty() ? m_artistsDB->getArt(artist) : art;
    cover->putPixmap(art);
    if (!art.isEmpty())
        cover->putPixmap(art);
    else
        cover->putDefaultPixmap();
    albumBox_frame->resize(cover->sizeHint());
}

void AlbumsView::albumTable_clicked(const QStringList &list)
{
    emit songClicked(list);
}

void AlbumsView::albumTable_rated(const QStringList &list)
{
    emit songRated(list);
}

void AlbumsView::albumTable_babeIt(const QStringList &list)
{
    emit songBabeIt(list);
}
