#include "rabbitview.h"

RabbitView::RabbitView(QWidget *parent) : QWidget(parent)
{
    auto *suggestionWidget_layout = new QGridLayout();
    suggestionWidget_layout->setContentsMargins(0,0,0,0);
    suggestionWidget_layout->setSpacing(0);

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setMaximumHeight(1);

    line->setStyleSheet("QFrame{border-color:black;}");

    artistSuggestion = new QListWidget(this);
    artistSuggestion->setGridSize(QSize(80+10,80+10));

    artistSuggestion->setFrameShape(QFrame::NoFrame);
    artistSuggestion->setViewMode(QListWidget::IconMode);
    artistSuggestion->setResizeMode(QListWidget::Adjust);
    artistSuggestion->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding);
    artistSuggestion->setSizeAdjustPolicy(QListWidget::AdjustToContentsOnFirstShow);
    artistSuggestion->setStyleSheet("QListWidget {background:#575757; padding-top:15px; padding-left:15px; }");

    generalSuggestion = new BabeTable(this);
    generalSuggestion->passStyle("QHeaderView::section { background-color:#333; color:white; }");

    generalSuggestion->setFrameShape(QFrame::NoFrame);


    QSplitter *splitter = new QSplitter(parent);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Vertical);

    splitter->addWidget(artistSuggestion);
    splitter->addWidget(line);
    splitter->addWidget(generalSuggestion);

    suggestionWidget_layout->addWidget(splitter,0,0);

    this->setLayout(suggestionWidget_layout);

}


void RabbitView::populateArtistSuggestion(QMap<QString,QByteArray> info)
{
    for(auto tag: info.keys())
    {
        auto art = new Album("",80,2,true,this);
        art->putPixmap(info[tag]);
        art->borderColor=true;
        art->setTitle(tag);
        art->titleVisible(false);
        auto item = new QListWidgetItem();
        item->setText(tag);
        item->setSizeHint(QSize(80,80));
        artistSuggestion->addItem(item);
        artistSuggestion->setItemWidget(item, art);
    }

}

void RabbitView::populateGeneralSuggestion(QList<QMap<int,QString>> mapList)
{
    generalSuggestion->populateTableView(mapList,false);
    //generalSuggestion->addItems(tags);
}

void RabbitView::flushSuggestions()
{
    generalSuggestion->flushTable();
    artistSuggestion->clear();
}
