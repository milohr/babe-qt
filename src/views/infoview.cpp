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

    artist = new BabeAlbum(Bae::DB{{Bae::KEY::ARTWORK, ":Data/data/cover.png"}}, Bae::AlbumSizeHint::MEDIUM_ALBUM, 100,false,this);
    connect(artist,&BabeAlbum::playAlbum,[this](const Bae::DB &info)
    {
        qDebug()<<"head on info view clicked!";
        emit this->playAlbum(info);

    });

    /*
     *ui->lyricsText->setLineWrapMode(QTextEdit::NoWrap);
     * ui->lyricsText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");
    ui->artistText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");
    ui->albumText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");
    ui->tagsInfo->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");*/


    artist->showTitle(false);
    artist->borderColor = true;

    auto artistContainer = new QWidget(this);
    artistContainer->setObjectName("artistContainer");
    //    artistContainer->setStyleSheet("QWidget#artistContainer{background-color: #575757; color:white;}");

    auto artistCLayout = new QHBoxLayout;

    //    ui->artistFrame->setMaximumWidth(ALBUM_SIZE_BIG);

    artistCLayout->addStretch();
    artistCLayout->addWidget(artist);
    artistCLayout->addStretch();
    artistContainer->setLayout(artistCLayout);
    artistContainer->setFixedHeight(static_cast<int>(artist->getSize())+12);



    auto infoUtils_layout = new QHBoxLayout;
    infoUtils_layout->setContentsMargins(0, 0, 0, 0);
    infoUtils_layout->setSpacing(0);

    infoUtils = new QWidget();
    infoUtils->setLayout(infoUtils_layout);
    infoUtils->setMaximumHeight(22);



    auto similarBtn = new QToolButton(this);
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

    auto moreBtn = new QToolButton(this);
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


    hideBtn = new QToolButton(this);
    connect(hideBtn, SIGNAL(clicked()), this, SLOT(hideArtistInfo()));
    hideBtn->setIconSize(QSize(16,16));
    hideBtn->setAutoRaise(true);
    hideBtn->setIcon(QIcon::fromTheme("hide_table_column"));
    hideBtn->setVisible(false);

    auto searchBtn = new QToolButton(this);
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

    ui->splitter->setSizes({0,0});
    ui->splitter->setStretchFactor(0, 0);

    ui->splitter->setStretchFactor(1, 1);

    //    ui->splitter->setSizes({120,1});

}

InfoView::~InfoView()
{
    qDebug()<<"DELETING INFOVIEW";
    delete ui;
}

void InfoView::setTrack(const Bae::DB &track)
{
    this->track=track;
    this->clearInfoViews();
    ui->titleLine->setText(this->track[Bae::KEY::TITLE]);
    ui->artistLine->setText(this->track[Bae::KEY::ARTIST]);
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

void InfoView::setArtistTags(const QStringList &tags)
{
    if(!tags.isEmpty())
    {
        ui->similarArtistInfo->setVisible(true);
        QString htmlTags;
        for(auto tag : tags) htmlTags+= "<a href=\""+tag+"\"> "+tag+"</a> , ";
        ui->similarArtistInfo->setHtml(htmlTags);
    }

}

void InfoView::setAlbumTags(const QStringList &tags)
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
    if(!lyrics.isEmpty())
    {
        ui->splitter->setSizes({static_cast<int>(Bae::AlbumSizeHint::BIG_ALBUM),static_cast<int>(Bae::AlbumSizeHint::BIG_ALBUM)});
        ui->lyricsText->setHtml(lyrics);
        ui->lyricsLayout->setAlignment(Qt::AlignCenter);
    }
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
{/*

    if(!this->track.isEmpty())
    {
        this->artist->setArtist(this->track[Bae::KEY::ARTIST]);
        //this->album->setAlbum(album);
        //        QCoreApplication::removePostedEvents(QObject *receiver, int eventType = 0)

        Pulpo info(this->track);
        connect(&info, &Pulpo::trackLyricsReady, [this] (const QString &lyrics,const Bae::DB &track)
        {
            emit lyricsReady(lyrics,track);
            if(this->track==track) this->setLyrics(lyrics);
        });

        connect(&info, &Pulpo::trackLyricsUrlReady, [this] (const QUrl &url,const Bae::DB &track)
        {
//            emit lyricsReady(lyrics,track);
            if(this->track==track) this->setLyrics(url);
        });

        connect(&info, &Pulpo::albumWikiReady,[this] (const QString &wiki,const Bae::DB &track)
        {
            emit albumWikiReady(wiki,track);
            if(this->track==track) this->setAlbumInfo(wiki);
        });

        connect(&info, &Pulpo::artistWikiReady,[this] (const QString &wiki,const Bae::DB &track)
        {
            emit artistWikiReady(wiki,track);
            if(this->track==track) this->setArtistInfo(wiki);
        });

        connect(&info, &Pulpo::artistSimilarReady, [this] (const QMap<QString,QByteArray> &info,const Bae::DB &track)
        {
            emit artistSimilarReady(info,track);
            if(this->track==track) this->setArtistTagInfo(info.keys());
        });

        connect(&info, &Pulpo::albumTagsReady, [this] (const QStringList &tags,const Bae::DB &track)
        {
            emit albumTagsReady(tags,track);
            if(this->track==track) this->setTagsInfo(tags);
        });

        connect(&info, &Pulpo::artistArtReady,[this](const QByteArray &array)
        {
            this->setArtistArt(array);
        });
*/

    //        if(album)
    //            info.fetchAlbumInfo(Pulpo::AllAlbumInfo,Pulpo::LastFm);
    //        else if(tags) info.fetchAlbumInfo(Pulpo::AlbumTags,Pulpo::LastFm);


    //        if(artist)
    //            info.fetchArtistInfo(Pulpo::AllArtistInfo,Pulpo::LastFm);
    //        else if(tags)  info.fetchArtistInfo(Pulpo::ArtistSimilar,Pulpo::LastFm);


    //        if(!this->track.isEmpty() && lyrics)
    //            info.fetchTrackInfo(Pulpo::NoneTrackInfo,Pulpo::LyricWikia,Pulpo::NoneInfoService);

    //}
}

void InfoView::clearInfoViews()
{
    ui->similarArtistInfo->setVisible(false);
    ui->similarArtistInfo->clear();

    ui->artistText->setVisible(false);
    ui->artistText->clear();


    ui->tagsInfo->setVisible(false);
    ui->tagsInfo->clear();

    ui->albumText->setVisible(false);
    ui->albumText->clear();

    ui->splitter->setSizes({ui->splitter->sizes().first(), 0});
     ui->lyricsText->clear();

}

void InfoView::on_toolButton_clicked()
{
    //    QString artist=ui->artistLine->text();
    //    QString title=ui->titleLine->text();
    //    Pulpo info({{Bae::KEY::TITLE,title},{Bae::KEY::ARTIST,artist}});
    //    connect(&info, &Pulpo::trackLyricsReady, this, &InfoView::setLyrics, Qt::UniqueConnection);

    //    info.fetchTrackInfo(Pulpo::NoneTrackInfo,Pulpo::LyricWikia,Pulpo::NoneInfoService);

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
