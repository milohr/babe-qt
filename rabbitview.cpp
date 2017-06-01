#include "rabbitview.h"

#include <QFrame>
#include <QGridLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSplitter>
#include <QString>

RabbitView::RabbitView(QWidget *parent) : QWidget(parent)
{
    QGridLayout *suggestionWidget_layout = new QGridLayout();
    suggestionWidget_layout->setContentsMargins(0, 0, 0, 0);
    suggestionWidget_layout->setSpacing(0);

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setMaximumHeight(1);

    artistSuggestion = new QListWidget(this);
    artistSuggestion->setGridSize(QSize(90, 90));
    artistSuggestion->setFixedHeight(120);
    artistSuggestion->setFrameShape(QFrame::NoFrame);
    artistSuggestion->setViewMode(QListWidget::IconMode);
    artistSuggestion->setResizeMode(QListWidget::Adjust);
    artistSuggestion->setFlow(QListView::TopToBottom);
    artistSuggestion->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    artistSuggestion->setSizeAdjustPolicy(QListWidget::AdjustToContentsOnFirstShow);
    artistSuggestion->setStyleSheet("QListWidget {background:#575757; padding-top:10px; padding-left:15px;}");

    generalSuggestion = new BabeTable(this);
    generalSuggestion->passStyle("QHeaderView::section {background-color:#333; color:white;}");
    generalSuggestion->setAddMusicMsg("\nCouldn't find similar music");

    QSplitter *splitter = new QSplitter(parent);
    splitter->setChildrenCollapsible(false);
    splitter->setOrientation(Qt::Vertical);

    splitter->setStyleSheet("QSplitter::handle {background-color:black;} QSplitter::handle:horizontal {width: 1px;}");
    splitter->addWidget(artistSuggestion);
    splitter->addWidget(generalSuggestion);

    suggestionWidget_layout->addWidget(splitter, 0, 0);
    this->setLayout(suggestionWidget_layout);
}

void RabbitView::populateArtistSuggestion(const QMap<QString,QByteArray> &info)
{
    for (auto tag : info.keys()) {
        auto art = new Album("", 80, 2, true, this);
        connect(art, &Album::albumCoverClicked, this, &RabbitView::filterByArtist);
        connect(art, &Album::playAlbum, [this] (QMap<int,QString> info) { emit playAlbum(info); });

        art->setToolTip(BaeUtils::fixString(tag));
        art->putPixmap(info[tag]);
        art->setBordercolor(true);
        art->setTitle(BaeUtils::fixString(tag));
        art->titleVisible(false);

        auto item = new QListWidgetItem();
        item->setSizeHint(QSize(80, 80));
        artistSuggestion->addItem(item);
        artistSuggestion->setItemWidget(item, art);
    }
}

void RabbitView::populateGeneralSuggestion(const QList<QMap<int, QString>> &mapList)
{
    generalSuggestion->populateTableView(mapList, false, false);
    generalSuggestion->removeRepeated();
}

void RabbitView::flushSuggestions(RabbitView::suggestionsTables list)
{
    switch (list) {
        case SIMILAR:
            artistSuggestion->clear();
            break;
        case GENERAL:
            generalSuggestion->flushTable();
            break;
        case ALL: generalSuggestion->flushTable();
            artistSuggestion->clear();
            break;
    }
}

BabeTable *RabbitView::getTable()
{
    return this->generalSuggestion;
}

void RabbitView::filterByArtist(const QMap<int, QString> &mapList)
{
    Q_UNUSED(mapList)
}
