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

    artist = new Album(this);
    connect(artist,&Album::playAlbum,[this](QMap<int,QString> info)
    {
        qDebug()<<"head on info view clicked!";
        emit this->playAlbum(info);

    });
    artist->createAlbum("","",":Data/data/cover.png", BaeUtils::MEDIUM_ALBUM, 100,false);

    /* ui->lyricsText->setLineWrapMode(QTextEdit::NoWrap);
    ui->lyricsText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");
    ui->artistText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");
    ui->albumText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");
    ui->tagsInfo->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");*/


    artist->showTitle(false);
    artist->borderColor = true;

    auto artistContainer = new QWidget();
    artistContainer->setObjectName("artistContainer");
    //    artistContainer->setStyleSheet("QWidget#artistContainer{background-color: #575757; color:white;}");

    auto artistCLayout = new QHBoxLayout();

    //    ui->artistFrame->setMaximumWidth(ALBUM_SIZE_BIG);

    artistCLayout->addStretch();
    artistCLayout->addWidget(artist);
    artistCLayout->addStretch();
    artistContainer->setLayout(artistCLayout);
    artistContainer->setFixedHeight(artist->getSize()+12);



    auto infoUtils_layout = new QHBoxLayout();
    infoUtils_layout->setContentsMargins(0, 0, 0, 0);
    infoUtils_layout->setSpacing(0);

    infoUtils = new QWidget();
    infoUtils->setLayout(infoUtils_layout);
    infoUtils->setMaximumHeight(22);



    auto similarBtn = new QToolButton();
    connect(similarBtn, &QToolButton::clicked, [this]()
    {
        auto list = ui->similarArtistInfo->toPlainText().trimmed().split(",");
        QStringList query;
        for (auto tag : list) query << QString("artist:"+tag).trimmed();
        emit similarBtnClicked(query);
    });
    similarBtn->setIconSize(QSize(16,16));
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
    moreBtn->setIconSize(QSize(16,16));
    moreBtn->setToolTip("Similar tags...");


    hideBtn = new QToolButton();
    connect(hideBtn, SIGNAL(clicked()), this, SLOT(hideArtistInfo()));
    hideBtn->setIconSize(QSize(16,16));
    hideBtn->setAutoRaise(true);
    hideBtn->setIcon(QIcon::fromTheme("hide_table_column"));
    hideBtn->setVisible(false);

    auto searchBtn = new QToolButton();
    connect(searchBtn,&QToolButton::clicked, this, &InfoView::on_searchBtn_clicked);
    searchBtn->setIconSize(QSize(16,16));
    searchBtn->setAutoRaise(true);
    searchBtn->setIcon(QIcon::fromTheme("edit-find-replace"));

    infoUtils_layout->addWidget(similarBtn);
    infoUtils_layout->addWidget(moreBtn);
    infoUtils_layout->addWidget(searchBtn);




    ui->artistLayout->insertWidget(0, artistContainer);
    ui->artistLayout->insertWidget(1,infoUtils);

    ui->customsearch->setVisible(false);

    ui->tagsInfo->setOpenLinks(false);
    ui->tagsInfo->setStyleSheet("QTextBrowser{background: transparent;}");

    ui->similarArtistInfo->setOpenLinks(false);

    //    ui->splitter->setSizes({120,1});

}

InfoView::~InfoView() { delete ui; }

void InfoView::setTrack(const BaeUtils::TRACKMAP &track)
{
    this->track=track;
    this->clearInfoViews();
    ui->titleLine->setText(this->track[BaeUtils::TracksCols::TITLE]);
    ui->artistLine->setText(this->track[BaeUtils::TracksCols::ARTIST]);
}




void InfoView::hideArtistInfo()
{
    //    qDebug() << "hide artist info";
    //    if (hide) {
    //        ui->artistFrame->setVisible(false);
    //        ui->frame_4->setVisible(false);
    //        hideBtn->setIcon(QIcon::fromTheme("show_table_column"));
    //        hide = !hide;
    //    } else {
    //        ui->artistFrame->setVisible(true);
    //        ui->frame_4->setVisible(true);
    //        hideBtn->setIcon(QIcon::fromTheme("hide_table_column"));
    //        hide = !hide;
    //    }
}

void InfoView::setArtistTagInfo(const QStringList &tags)
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
        //        ui->frame_5->show();
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

void InfoView::setLyrics(const QString &lyrics)
{
        ui->lyricsText->setHtml(lyrics);
}



void InfoView::on_searchBtn_clicked()
{
    if(!customsearch)
    {
        ui->customsearch->setVisible(true);
        customsearch=!customsearch;
    }
    else
    {
        ui->customsearch->setVisible(false);
        customsearch=!customsearch;
    }
}


void InfoView::getTrackInfo(const bool &album, const bool &artist, const bool &lyrics, const bool &tags)
{

    if(!this->track.isEmpty())
    {
        this->artist->setArtist(this->track[BaeUtils::TracksCols::ARTIST]);
        //this->album->setAlbum(album);
        //        QCoreApplication::removePostedEvents(QObject *receiver, int eventType = 0)

        Pulpo info(this->track);
        connect(&info, &Pulpo::trackLyricsReady, [this] (const QString &lyrics,const BaeUtils::TRACKMAP &track)
        {
            emit lyricsReady(lyrics,track);
            if(this->track==track) this->setLyrics(lyrics);
        });

        connect(&info, &Pulpo::albumWikiReady,[this] (const QString &wiki,const BaeUtils::TRACKMAP &track)
        {
            emit albumWikiReady(wiki,track);
            if(this->track==track) this->setAlbumInfo(wiki);
        });

        connect(&info, &Pulpo::artistWikiReady,[this] (const QString &wiki,const BaeUtils::TRACKMAP &track)
        {
            emit artistWikiReady(wiki,track);
           if(this->track==track) this->setArtistInfo(wiki);
        });

        connect(&info, &Pulpo::artistSimilarReady, [this] (const QMap<QString,QByteArray> &info,const BaeUtils::TRACKMAP &track)
        {
            emit artistSimilarReady(info,track);
           if(this->track==track) this->setArtistTagInfo(info.keys());
        });

        connect(&info, &Pulpo::albumTagsReady, [this] (const QStringList &tags,const BaeUtils::TRACKMAP &track)
        {
            emit albumTagsReady(tags,track);
            if(this->track==track) this->setTagsInfo(tags);
        });

        connect(&info, &Pulpo::artistArtReady,[this](const QByteArray &array)
        {
             this->setArtistArt(array);
        });


        if(album)
            info.fetchAlbumInfo(Pulpo::AllAlbumInfo,Pulpo::LastFm);
        else if(tags) info.fetchAlbumInfo(Pulpo::AlbumTags,Pulpo::LastFm);


        if(artist)
            info.fetchArtistInfo(Pulpo::AllArtistInfo,Pulpo::LastFm);
        else if(tags)  info.fetchArtistInfo(Pulpo::ArtistSimilar,Pulpo::LastFm);


        if(!this->track.isEmpty() && lyrics)
            info.fetchTrackInfo(Pulpo::NoneTrackInfo,Pulpo::LyricWikia,Pulpo::NoneInfoService);

    }
}

void InfoView::clearInfoViews()
{
    ui->artistText->setVisible(false);
    ui->similarArtistInfo->setVisible(false);

    ui->artistText->clear();

    ui->albumText->setVisible(false);

    ui->tagsInfo->setVisible(false);
    ui->tagsInfo->clear();
    ui->albumText->clear();

    ui->lyricsText->clear();

}

void InfoView::on_toolButton_clicked()
{
    QString artist=ui->artistLine->text();
    QString title=ui->titleLine->text();
    Pulpo info({{BaeUtils::TracksCols::TITLE,title},{BaeUtils::TracksCols::ARTIST,artist}});
    connect(&info, &Pulpo::trackLyricsReady, this, &InfoView::setLyrics, Qt::UniqueConnection);

    info.fetchTrackInfo(Pulpo::NoneTrackInfo,Pulpo::LyricWikia,Pulpo::NoneInfoService);

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

QStringList InfoView::getTags()
{
    return ui->tagsInfo->toPlainText().split(",");
}

QStringList InfoView::getSimilarArtistTags()
{
    return ui->similarArtistInfo->toPlainText().split(",");

}
