#include "rabbitview.h"
#include <QColor>

RabbitView::RabbitView(QWidget *parent) : QWidget(parent)
{
    auto *suggestionWidget_layout = new QGridLayout;
    suggestionWidget_layout->setContentsMargins(0,0,0,0);
    suggestionWidget_layout->setSpacing(0);

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setMaximumHeight(1);

    artistSuggestion = new BabeGrid(Bae::SMALL_ALBUM_FACTOR,Bae::AlbumSizeHint::SMALL_ALBUM,this);
    artistSuggestion->setAlbumsSpacing(10);
    artistSuggestion->setFixedHeight(static_cast<int>(ALBUM_SIZE_MEDIUM));

    artistSuggestion->setFlow(QListView::TopToBottom);
    artistSuggestion->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Fixed);


    generalSuggestion = new BabeTable(this);
    generalSuggestion->hideColumn(static_cast<int>(Bae::KEY::ALBUM));
    generalSuggestion->hideColumn(static_cast<int>(Bae::KEY::ARTIST));
    generalSuggestion->hideColumn(static_cast<int>(Bae::KEY::DURATION));
    generalSuggestion->horizontalHeader()->setVisible(false);
    generalSuggestion->enableRowColoring(true);
    generalSuggestion->enableRowDragging(true);
//    generalSuggestion->passStyle("QHeaderView::section { background-color:#333; color:white; }");
    generalSuggestion->setAddMusicMsg("\nCouldn't find similar music","face-quiet");

    auto bgcolor= QColor(generalSuggestion->palette().color(QPalette::Background).name()).dark(200).name();
    artistSuggestion->setStyleSheet(QString("QListWidget {background:%1; padding-top:10px; padding-left:15px; }").arg(bgcolor));

    QSplitter *splitter = new QSplitter(this);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Vertical);

    splitter->setStyleSheet("QSplitter::handle {background-color:black;} QSplitter::handle:horizontal { width: 1px; }");
    splitter->addWidget(artistSuggestion);
    splitter->addWidget(generalSuggestion);

    suggestionWidget_layout->addWidget(splitter,0,0);

    this->setLayout(suggestionWidget_layout);

}


void RabbitView::populateArtistSuggestion(QMap<QString,QByteArray> info)
{
    for(auto tag: info.keys())
    {
        qDebug()<<tag;

        Bae::DB album {{Bae::KEY::ARTIST,Bae::fixString(tag)}};

        Pulpo saver(album);
        connect(&saver, &Pulpo::artSaved,[this](const Bae::DB &album)
        {
            artistSuggestion->addAlbum(album);

        });
        saver.saveArt(info[tag],Bae::CachePath);


    }
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
    case ALL:  generalSuggestion->flushTable(); artistSuggestion->clear(); break;
    }

}

void RabbitView::filterByArtist(const Bae::DB &mapList)
{
    /* generalSuggestion->flushTable();
    artistSuggestion->clear();*/
}

