#include "rabbitview.h"
#include <QColor>
#include"../db/rabbithole.h"

using namespace Query;

RabbitView::RabbitView(QWidget *parent) : QWidget(parent)
{
    auto *suggestionWidget_layout = new QGridLayout;
    suggestionWidget_layout->setContentsMargins(0,0,0,0);
    suggestionWidget_layout->setSpacing(0);

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setMaximumHeight(1);

    artistSuggestion = new BabeGrid(Bae::SMALL_ALBUM_FACTOR,Bae::AlbumSizeHint::SMALL_ALBUM,0,this);
    artistSuggestion->autoAdjust = false;
    artistSuggestion->albumShadows = false;
    artistSuggestion->setAlbumsSpacing(0);
    artistSuggestion->setFixedWidth(static_cast<int>(Bae::AlbumSizeHint::SMALL_ALBUM));
    artistSuggestion->setContentsMargins(0,0,0,0);
    artistSuggestion->hiddenLabels = true;
    artistSuggestion->setFlow(QListView::TopToBottom);
    artistSuggestion->setSizePolicy(QSizePolicy ::Fixed , QSizePolicy ::Expanding);
    artistSuggestion->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    artistSuggestion->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    artistSuggestion->setSpacing(0);
    artistSuggestion->setResizeMode(QListView::ResizeMode::Fixed);
    artistSuggestion->setWrapping(false);

    connect(this->artistSuggestion, &BabeGrid::playAlbum, [this] (const Bae::DB &map)
    {
       emit this->playAlbum(map);
    });

    generalSuggestion = new BabeTable(this);
//    generalSuggestion->hideColumn(static_cast<int>(Bae::KEY::ALBUM));
    //    generalSuggestion->hideColumn(static_cast<int>(Bae::KEY::ARTIST));
    generalSuggestion->hideColumn(static_cast<int>(Bae::KEY::DURATION));
//    generalSuggestion->horizontalHeader()->setVisible(false);
    generalSuggestion->enableRowColoring(true);
    generalSuggestion->enableRowDragging(true);
    //    generalSuggestion->passStyle("QHeaderView::section { background-color:#333; color:white; }");
    generalSuggestion->setAddMusicMsg("\nCouldn't find similar music","face-quiet");

    auto bgcolor= QColor(generalSuggestion->palette().color(QPalette::Background).name()).dark(200).name();
    //    artistSuggestion->setStyleSheet(QString("QListWidget {background:%1; padding-top:10px; padding-left:15px; }").arg(bgcolor));

    QSplitter *splitter = new QSplitter(this);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Horizontal);

    splitter->setStyleSheet("QSplitter::handle {background-color:black;} QSplitter::handle:horizontal { width: 1px; }");
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
    this->populateGeneralSuggestion(connection.getDBData(query));

    queryTxt = QUERY[TABLE::ARTISTS][W::SIMILAR];
    query.prepare(queryTxt.replace("?", track[KEY::URL]));
    this->populateArtistSuggestion(connection.getDBData(query));

}


void RabbitView::populateArtistSuggestion(const Bae::DB_LIST &mapList)
{
    for(auto artist : mapList)
        this->artistSuggestion->addAlbum(artist);
}

void RabbitView::populateGeneralSuggestion(const Bae::DB_LIST &mapList)
{
    generalSuggestion->populateTableView(mapList);
    //    generalSuggestion->removeRepeated();
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

void RabbitView::filterByArtist(const Bae::DB &mapList)
{
    /* generalSuggestion->flushTable();
    artistSuggestion->clear();*/
}

