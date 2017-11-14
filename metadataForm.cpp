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


#include "metadataForm.h"
#include "ui_metadataForm.h"

metadataForm::metadataForm(const Bae::DB &info, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::metadataForm)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
     setWindowFlags(flags);
     //mapFromParent(QPoint(100, 100));

     track=info[Bae::KEY::TRACK];
     title=info[Bae::KEY::TITLE];
     artist=info[Bae::KEY::ARTIST];
     album=info[Bae::KEY::ALBUM];
     genre=info[Bae::KEY::GENRE];

     ui->trackLine->setText(track);
     ui->titleLine->setText(title);
     ui->genreLine->setText(genre);
     ui->artistLine->setText(artist);
     ui->albumLine->setText(album);

}

metadataForm::~metadataForm()
{
    delete ui;
}


void metadataForm::on_pushButton_2_clicked()
{
    this->close();
    this->destroy();
}

void metadataForm::on_changebtn_clicked()
{
    QString _track = ui->trackLine->text();
    QString _title = ui->titleLine->text();
    QString _artist = ui->artistLine->text();
    QString _album = ui->albumLine->text();
    QString _genre = ui->genreLine->text();

    if(_track!=this->track || _title!=this->title || _artist!=this->artist || _album!=this->album || _genre!=this->genre)
    {
        qDebug()<< "the info did changed";
        const Bae::DB map{{Bae::KEY::TRACK, _track}, {Bae::KEY::TITLE, _title}, {Bae::KEY::ARTIST, _artist},{Bae::KEY::ALBUM,_album},{Bae::KEY::GENRE,_genre}};

        emit infoModified(map);
    }else
    {
        qDebug()<< "the info didn't changed";
    }
    this->close();
}
