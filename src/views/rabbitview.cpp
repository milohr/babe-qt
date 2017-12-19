#include "rabbitview.h"

#include <QColor>
#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QFrame>
#include <QSplitter>

#include"../db/rabbithole.h"
#include "../widget_models/babegrid.h"
#include "../widget_models/babealbum.h"
#include "../widget_models/babetable.h"
#include "../pulpo/pulpo.h"

using namespace Query;

RabbitView::RabbitView(QWidget *parent) : QWidget(parent)
{
    auto *suggestionWidget_layout = new QGridLayout;
    suggestionWidget_layout->setContentsMargins(0,0,0,0);
    suggestionWidget_layout->setSpacing(0);

    this->artistSuggestion = new BabeGrid(BAE::SMALL_ALBUM_FACTOR,BAE::AlbumSizeHint::SMALL_ALBUM,0,this);
    connect(this->artistSuggestion, &BabeGrid::albumReady, [this](){albumLoader.next();});
    connect(&this->albumLoader, &AlbumLoader::albumReady, this, &RabbitView::addArtistSuggestion);
    connect(this->artistSuggestion, &BabeGrid::albumClicked,[this](const DB &albumMap)
    {
        this->filterList->flushTable();
        this->filterList->setVisible(true);
        auto query = QString("select * from tracks where artist = '%1'").arg(albumMap[KEY::ARTIST]);
        this->filterList->populateTableView(query);
    });

    this->artistSuggestion->autoAdjust = false;
    this->artistSuggestion->albumShadows = false;
    this->artistSuggestion->setAlbumsSpacing(0);
    this->artistSuggestion->setFixedWidth(static_cast<int>(artistSuggestion->albumSize)+(artistSuggestion->frameWidth()*2));
    this->artistSuggestion->setContentsMargins(0,0,0,0);
    this->artistSuggestion->hiddenLabels = true;
    this->artistSuggestion->setFlow(QListView::TopToBottom);
    this->artistSuggestion->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Expanding);
    this->artistSuggestion->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    this->artistSuggestion->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    this->artistSuggestion->setSpacing(0);
    this->artistSuggestion->setResizeMode(QListView::ResizeMode::Fixed);
    this->artistSuggestion->setWrapping(false);

    this->generalSuggestion = new BabeTable(this);
    this->generalSuggestion->setFrameShape(QFrame::StyledPanel);
    this->generalSuggestion->setFrameShadow(QFrame::Sunken);
    //    generalSuggestion->hideColumn(static_cast<int>(BAE::KEY::ALBUM));
    //    generalSuggestion->hideColumn(static_cast<int>(BAE::KEY::ARTIST));
    this->generalSuggestion->hideColumn(static_cast<int>(BAE::KEY::DURATION));
    //    generalSuggestion->horizontalHeader()->setVisible(false);
    this->generalSuggestion->enableRowColoring(true);
    this->generalSuggestion->enableRowDragging(true);
    //    generalSuggestion->passStyle("QHeaderView::section { background-color:#333; color:white; }");
    this->generalSuggestion->setAddMusicMsg("\nCouldn't find similar music","face-quiet");

    this->filterList = new BabeTable(this);
    this->filterList->setVisible(false);
    this->filterList->setFrameShape(QFrame::StyledPanel);
    this->filterList->setFrameShadow(QFrame::Sunken);
    //    generalSuggestion->hideColumn(static_cast<int>(BAE::KEY::ALBUM));
    this->filterList->hideColumn(static_cast<int>(BAE::KEY::ARTIST));
    this->filterList->hideColumn(static_cast<int>(BAE::KEY::DURATION));
    //    generalSuggestion->horizontalHeader()->setVisible(false);
    this->filterList->enableRowColoring(true);
    //    this->filterList->enableRowDragging(true);
    //    generalSuggestion->passStyle("QHeaderView::section { background-color:#333; color:white; }");
    this->filterList->setAddMusicMsg("\nNothing here","face-quiet");

    //    auto bgcolor= QColor(generalSuggestion->palette().color(QPalette::Background).name()).dark(200).name();
    //    artistSuggestion->setStyleSheet(QString("QListWidget {background:%1; padding-top:10px; padding-left:15px; }").arg(bgcolor));

    QSplitter *splitter = new QSplitter(this);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Horizontal);

    //    splitter->setStyleSheet("QSplitter::handle {background-color:black;} QSplitter::handle:horizontal { width: 1px; }");
    splitter->addWidget(generalSuggestion);
    splitter->addWidget(filterList);
    splitter->addWidget(artistSuggestion);

    suggestionWidget_layout->addWidget(splitter,0,0);

    splitter->setHandleWidth(6);
    splitter->setSizes({0,0,0});
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 0);

    this->setLayout(suggestionWidget_layout);
}

RabbitView::~RabbitView()
{
    qDebug()<<"DELETING RABBITVIEW";
}

void RabbitView::seed(const DB &track)
{
    this->flushSuggestions();
    auto queryTxt = QString(QUERY[TABLE::TRACKS][W::SIMILAR]).replace("?", track[KEY::URL]);
    this->generalSuggestion->populateTableView(queryTxt);
    queryTxt = QString(QUERY[TABLE::TRACKS][W::TAG]).replace("?", track[KEY::GENRE]);
    this->generalSuggestion->populateTableView(queryTxt);

    queryTxt = QString(QUERY[TABLE::ARTISTS][W::SIMILAR]).replace("?", track[KEY::URL]);
    this->albumLoader.requestAlbums(queryTxt);

}

void RabbitView::flushSuggestions(RabbitView::suggestionsTables list)
{
    switch(list)
    {
    case SIMILAR:
        this->artistSuggestion->clear();
        break;
    case GENERAL:
        this->generalSuggestion->flushTable();
        break;
    case ALL:
        this->generalSuggestion->flushTable();
        this->artistSuggestion->flushGrid();
        this->filterList->flushTable();
        this->filterList->setVisible(false);
        break;
    }
}

void RabbitView::addArtistSuggestion(const DB &albumMap)
{
    this->artistSuggestion->addAlbum(albumMap);
}

