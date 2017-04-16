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


InfoView::InfoView(QWidget *parent) : QWidget(parent), ui(new Ui::InfoView) {
    ui->setupUi(this);
    artist = new Album(":Data/data/cover.png", 120, 100,false,false,this);
    connect(artist,SIGNAL(playAlbum(QString , QString)),this,SLOT(playAlbum_clicked(QString, QString)));

    ui->lyricsText->setLineWrapMode(QTextEdit::NoWrap);
    ui->lyricsText->setStyleSheet(
                "QTextBrowser{background-color: #575757; color:white;}");

    lyrics = new Lyrics();
    connect(lyrics,SIGNAL(lyricsReady(QString)),this,SLOT(setLyrics(QString)));

    artist->titleVisible(false);
    artist->borderColor = true;

    auto artistContainer = new QWidget();
    artistContainer->setBackgroundRole(QPalette::Dark);
    auto artistCLayout = new QHBoxLayout();
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    artistCLayout->addWidget(left_spacer);
    artistCLayout->addWidget(artist);
    artistCLayout->addWidget(right_spacer);
    artistContainer->setLayout(artistCLayout);
    artistContainer->setMaximumSize(200, 130);
    ui->artistLayout->insertWidget(0, artistContainer);

    infoUtils = new QWidget();
    // artistContainer->setStyleSheet("QWidget{background-color: #575757;
    // color:white;}");
    infoUtils->setMaximumWidth(200);
    auto infoUtils_layout = new QHBoxLayout();
    infoUtils_layout->setContentsMargins(0, 0, 0, 0);
    infoUtils_layout->setSpacing(0);
    auto similarBtn = new QToolButton();
    similarBtn->setAutoRaise(true);
    similarBtn->setIcon(QIcon::fromTheme("similarartists-amarok"));

    auto moreBtn = new QToolButton();
    moreBtn->setAutoRaise(true);
    moreBtn->setIcon(QIcon::fromTheme("filename-discnumber-amarok"));


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

}

InfoView::~InfoView() { delete ui; }


void InfoView::playAlbum_clicked(QString artist, QString album)
{
    emit playAlbum(artist,album);

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
    QString htmlTags;

    for(auto tag : tags)
    {
        htmlTags+= "<a href=\""+tag+"\"> "+tag+"</a> , ";
    }

     ui->tagsInfo->setHtml(htmlTags);
}

void InfoView::setAlbumInfo(QString info)
{

    //qDebug() << info;
    if (info.isEmpty()) {
        ui->albumText->hide();

        ui->frame_5->hide();
    } else {
        ui->albumText->show();

        ui->frame_5->show();
        ui->albumText->setHtml(info);
    }
}

void InfoView::setAlbumArt(QByteArray array) {Q_UNUSED(array)}

void InfoView::setArtistInfo(QString info) { ui->artistText->setHtml(info); }

void InfoView::setArtistArt(QByteArray array)
{
    artist->putPixmap(array);

}

void InfoView::setArtistArt(QString url)
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


void InfoView::getTrackInfo(QString _title, QString _artist, QString _album)
{

    if(!_album.isEmpty()&&!_artist.isEmpty())
    {
        this->artist->setArtist(_artist);
        //this->album->setAlbum(album);

        ArtWork coverInfo;
        ArtWork artistInfo;
        connect(&coverInfo, SIGNAL(infoReady(QString)), this, SLOT(setAlbumInfo(QString)));
        connect(&artistInfo, SIGNAL(bioReady(QString)), this, SLOT(setArtistInfo(QString)));
        connect(&artistInfo, SIGNAL(tagsReady(QStringList)), this, SLOT(setArtistTagInfo(QStringList)));
        coverInfo.setDataCoverInfo(_artist,_album);
        artistInfo.setDataHeadInfo(_artist);

        if(!_title.isEmpty())
        {
            lyrics->setData(_artist,_title);
            ui->artistLine->setText(_artist);
            ui->titleLine->setText(_title);
        }
    }
}

void InfoView::getTrackArt(QString _artist, QString _album)
{
    Q_UNUSED(_album);
    ArtWork artistHead;
    connect(&artistHead, SIGNAL(headReady(QByteArray)), this, SLOT(setArtistArt(QByteArray)));
    artistHead.setDataHead(_artist);
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
    qDebug()<<arg1.toString();
    emit tagClicked(arg1.toString());
}
