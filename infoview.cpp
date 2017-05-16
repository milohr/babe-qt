/*
   Babe - tiny music player
   Copyright (C) 2017  Camilo Higuita
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

   */


#include "infoview.h"
#include "ui_infoview.h"


InfoView::InfoView(QWidget *parent) : QWidget(parent), ui(new Ui::InfoView)
{
    ui->setupUi(this);

    artist = new Album(":Data/data/cover.png", ALBUM_SIZE_MEDIUM, 100,false,this);
    connect(artist,&Album::playAlbum,this,&InfoView::playAlbum_clicked);

    /* ui->lyricsText->setLineWrapMode(QTextEdit::NoWrap);
    ui->lyricsText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");
    ui->artistText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");
    ui->albumText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");
    ui->tagsInfo->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");*/

    lyrics = new Lyrics();
    connect(lyrics,SIGNAL(lyricsReady(QString)),this,SLOT(setLyrics(QString)));

    artist->titleVisible(false);
    artist->borderColor = true;

    auto artistContainer = new QWidget();
    //artistContainer->setStyleSheet("QWidget{background-color: #575757; color:white;}");

    auto artistCLayout = new QHBoxLayout();
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->artistFrame->setMaximumWidth(ALBUM_SIZE_BIG);

    artistCLayout->addWidget(left_spacer);
    artistCLayout->addWidget(artist);
    artistCLayout->addWidget(right_spacer);
    artistContainer->setLayout(artistCLayout);
    ui->artistLayout->insertWidget(0, artistContainer);

    infoUtils = new QWidget();
    // artistContainer->setStyleSheet("QWidget{background-color: #575757;
    // color:white;}");
    infoUtils->setMaximumWidth(ALBUM_SIZE_BIG);
    auto infoUtils_layout = new QHBoxLayout();
    infoUtils_layout->setContentsMargins(0, 0, 0, 0);
    infoUtils_layout->setSpacing(0);

    auto similarBtn = new QToolButton();
    connect(similarBtn, &QToolButton::clicked, [this]()
    {
        auto list = ui->similarArtistInfo->toPlainText().trimmed().split(",");
        QStringList query;
        for (auto tag : list) query << QString("artist:"+tag).trimmed();
        emit similarBtnClicked(query);
    });

    similarBtn->setAutoRaise(true);
    similarBtn->setIcon(QIcon::fromTheme("similarartists-amarok"));
    similarBtn->setToolTip("Similar artists...");

    auto moreBtn = new QToolButton();
    connect(moreBtn, &QToolButton::clicked, [this]()
    {
        auto list = ui->tagsInfo->toPlainText().split(",");
        QStringList query;
        for (auto tag : list) query <<tag.trimmed();
        emit tagsBtnClicked(query);
    });

    moreBtn->setAutoRaise(true);
    moreBtn->setIcon(QIcon::fromTheme("filename-discnumber-amarok"));
    moreBtn->setToolTip("Similar tags...");


    hideBtn = new QToolButton();
    connect(hideBtn, SIGNAL(clicked()), this, SLOT(hideArtistInfo()));

    hideBtn->setAutoRaise(true);
    hideBtn->setIcon(QIcon::fromTheme("hide_table_column"));

    infoUtils_layout->addWidget(similarBtn);
    infoUtils_layout->addWidget(moreBtn);

    infoUtils_layout->addWidget(hideBtn);
    infoUtils_layout->addWidget(ui->searchBtn);
    infoUtils->setLayout(infoUtils_layout);
    ui->artistFrame->setVisible(false);
    ui->frame_4->setVisible(false);
    infoUtils->hide();
    ui->customsearch->setVisible(false);
    ui->frame_2->setVisible(false);
    //ui->searchBtn->setParent(ui->lyricsText);
    // ui->searchBtn->setGeometry(5,5,22,22);

    //ui->searchBtn->setVisible(false);

    ui->tagsInfo->setOpenLinks(false);
    ui->tagsInfo->setStyleSheet("QTextBrowser{background: transparent;}");

    ui->similarArtistInfo->setOpenLinks(false);

}

InfoView::~InfoView() { delete ui; }


void InfoView::playAlbum_clicked(QMap<int, QString> info)
{
    emit playAlbum(info);

}

void InfoView::hideArtistInfo()
{
    qDebug() << "hide artist info";
    if (hide) {
        ui->artistFrame->setVisible(false);
        ui->frame_4->setVisible(false);
        hideBtn->setIcon(QIcon::fromTheme("show_table_column"));
        hide = !hide;
    } else {
        ui->artistFrame->setVisible(true);
        ui->frame_4->setVisible(true);
        hideBtn->setIcon(QIcon::fromTheme("hide_table_column"));
        hide = !hide;
    }
}

void InfoView::setArtistTagInfo(QStringList tags)
{    
    if(!tags.isEmpty())
    {
        ui->similarArtistInfo->setVisible(true);
        QString htmlTags;
        for(auto tag : tags) htmlTags+= "<a href=\""+tag+"\"> "+tag+"</a> , ";
        ui->similarArtistInfo->setHtml(htmlTags);
    }

}

void InfoView::setTagsInfo(QStringList tags)
{
    if(!tags.isEmpty())
    {
        ui->tagsInfo->setVisible(true);
        QString htmlTags;
        for(auto tag : tags) htmlTags+= "<a href=\""+tag+"\"> "+tag+"</a> , ";
        ui->tagsInfo->setHtml(htmlTags);
    }

}


void InfoView::setAlbumInfo(QString info)
{
    if (!info.isEmpty())
    {
        ui->albumText->setVisible(true);
        ui->frame_5->show();
        ui->albumText->setHtml(info);
    }
}

void InfoView::setAlbumArt(QByteArray array) {Q_UNUSED(array)}

void InfoView::setArtistInfo(const QString &info)
{

    if(!info.isEmpty())
    {
        ui->artistText->setVisible(true);
        ui->artistText->setHtml(info);
    }

}

void InfoView::setArtistArt(const QByteArray &array)
{
    artist->putPixmap(array);

}

void InfoView::setArtistArt(const QString &url)
{
    artist->putPixmap(url);
}

void InfoView::setLyrics(QString lyrics)
{
    ui->lyricsText->setHtml(lyrics);
    ui->lyricsLayout->setAlignment(Qt::AlignCenter);
    ui->lyricsFrame->show();
}

void InfoView::on_searchBtn_clicked()
{
    if(!customsearch)
    {
        ui->customsearch->setVisible(true);
        ui->frame_2->setVisible(true);
        customsearch=!customsearch;
    }
    else
    {
        ui->customsearch->setVisible(false);
        ui->frame_2->setVisible(false);
        customsearch=!customsearch;
    }
}


void InfoView::getTrackInfo(const QString &title_, const QString &artist_, const QString &album_)
{
    clearInfoViews();

    if(!album_.isEmpty()&&!artist_.isEmpty())
    {
        this->artist->setArtist(artist_);
        //this->album->setAlbum(album);

        Pulpo info(title_,artist_,album_);
        connect(&info, &Pulpo::albumWikiReady, this, &InfoView::setAlbumInfo);
        connect(&info, &Pulpo::artistWikiReady, this, &InfoView::setArtistInfo);
        connect(&info, &Pulpo::artistSimilarReady, [this] (QMap<QString,QByteArray> info)
        {
            this->setArtistTagInfo(info.keys());
        });

        connect(&info, &Pulpo::albumTagsReady, this, &InfoView::setTagsInfo);
        connect(&info, &Pulpo::artistArtReady,[this](QByteArray array){this->setArtistArt(array);});

        info.fetchAlbumInfo(Pulpo::AllAlbumInfo,Pulpo::LastFm);
        info.fetchArtistInfo(Pulpo::AllArtistInfo,Pulpo::LastFm);

        //info.fetchTrackInfo(Pulpo::TrackTags,Pulpo::LyricWikia,Pulpo::LastFm);

        if(!title_.isEmpty())
        {
            lyrics->setData(artist_,title_);
            ui->artistLine->setText(artist_);
            ui->titleLine->setText(title_);
        }
    }
}

void InfoView::clearInfoViews()
{
    ui->similarArtistInfo->setVisible(false);
    ui->tagsInfo->setVisible(false);
    ui->artistText->setVisible(false);
    ui->albumText->setVisible(false);
}

void InfoView::on_toolButton_clicked()
{
    QString artist=ui->artistLine->text();
    QString title=ui->titleLine->text();

    if(!artist.isEmpty()&&!title.isEmpty())
        lyrics->setData(artist,title);
}


void InfoView::on_tagsInfo_anchorClicked(const QUrl &arg1)
{
    QString query = arg1.toString();
    emit tagClicked(query);
}

void InfoView::on_similarArtistInfo_anchorClicked(const QUrl &arg1)
{
    QString query = "artist:"+arg1.toString();
    emit similarArtistTagClicked(query);
}
