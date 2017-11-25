#include "rabbitview.h"
#include <QColor>
#include"../db/rabbithole.h"

using namespace Query;

RabbitView::RabbitView(QWidget *parent) : QWidget(parent)
{
    auto *suggestionWidget_layout = new QGridLayout;
    suggestionWidget_layout->setContentsMargins(0,0,0,0);
    suggestionWidget_layout->setSpacing(0);

    this->artistSuggestion = new BabeGrid(Bae::SMALL_ALBUM_FACTOR,Bae::AlbumSizeHint::SMALL_ALBUM,0,this);
    connect(this->artistSuggestion, &BabeGrid::albumReady, [this](){albumLoader.next();});
    connect(&this->albumLoader, &AlbumLoader::albumReady, this, &RabbitView::addArtistSuggestion);

    this->artistSuggestion->autoAdjust = false;
    this->artistSuggestion->albumShadows = false;
    this->artistSuggestion->setAlbumsSpacing(0);
    this->artistSuggestion->setFixedWidth(static_cast<int>(artistSuggestion->albumSize));
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
    //    generalSuggestion->hideColumn(static_cast<int>(Bae::KEY::ALBUM));
    //    generalSuggestion->hideColumn(static_cast<int>(Bae::KEY::ARTIST));
    this->generalSuggestion->hideColumn(static_cast<int>(Bae::KEY::DURATION));
//    generalSuggestion->horizontalHeader()->setVisible(false);
    this->generalSuggestion->enableRowColoring(true);
    this->generalSuggestion->enableRowDragging(true);
    //    generalSuggestion->passStyle("QHeaderView::section { background-color:#333; color:white; }");
    this->generalSuggestion->setAddMusicMsg("\nCouldn't find similar music","face-quiet");

    auto bgcolor= QColor(generalSuggestion->palette().color(QPalette::Background).name()).dark(200).name();
    //    artistSuggestion->setStyleSheet(QString("QListWidget {background:%1; padding-top:10px; padding-left:15px; }").arg(bgcolor));

    QSplitter *splitter = new QSplitter(this);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Horizontal);

//    splitter->setStyleSheet("QSplitter::handle {background-color:black;} QSplitter::handle:horizontal { width: 1px; }");
    splitter->addWidget(generalSuggestion);
    splitter->addWidget(artistSuggestion);

    suggestionWidget_layout->addWidget(splitter,0,0);

    splitter->setSizes({0,0});
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    this->setLayout(suggestionWidget_layout);



}

RabbitView::~RabbitView()
{
    qDebug()<<"DELETING RABBITVIEW";
}

void RabbitView::seed(const DB &track)
{
    this->flushSuggestions();
    auto queryTxt = QUERY[TABLE::TRACKS][W::SIMILAR];
    QSqlQuery query(queryTxt.replace("?", track[KEY::URL]));
    this->generalSuggestion->populateTableView(query);

    queryTxt = QUERY[TABLE::ARTISTS][W::SIMILAR];
    this->albumLoader.requestAlbums(queryTxt.replace("?", track[KEY::URL]));

}

void RabbitView::flushSuggestions(RabbitView::suggestionsTables list)
{
    switch(list)
    {
    case SIMILAR: artistSuggestion->clear(); break;
    case GENERAL: generalSuggestion->flushTable(); break;
    case ALL:  generalSuggestion->flushTable(); artistSuggestion->flushGrid(); break;
    }

}

void RabbitView::addArtistSuggestion(const DB &albumMap)
{
   this->artistSuggestion->addAlbum(albumMap);
}

void RabbitView::filterByArtist(const Bae::DB &mapList)
{
    /* generalSuggestion->flushTable();
    artistSuggestion->clear();*/
}

