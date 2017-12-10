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


#include "metadataform.h"
#include "ui_metadataForm.h"
#include "../services/local/taginfo.h"

metadataForm::metadataForm(const Bae::DB &info, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::metadataForm)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    setWindowFlags(flags);
    //mapFromParent(QPoint(100, 100));
    this->trackMap = info;

    ui->trackLine->setText(trackMap[Bae::KEY::TRACK]);
    ui->titleLine->setText(trackMap[Bae::KEY::TITLE]);
    ui->genreLine->setText(trackMap[Bae::KEY::GENRE]);
    ui->artistLine->setText(trackMap[Bae::KEY::ARTIST]);
    ui->albumLine->setText(trackMap[Bae::KEY::ALBUM]);

}

metadataForm::~metadataForm() { delete ui; }


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

    if(_track!=this->trackMap[Bae::KEY::TRACK]
            || _title!=this->trackMap[Bae::KEY::TITLE]
            || _artist!=this->trackMap[Bae::KEY::TRACK]
            || _album!=this->trackMap[Bae::KEY::ALBUM]
            || _genre!=this->trackMap[Bae::KEY::GENRE] )
    {

        if(ui->checkBox->isChecked())
        {
            TagInfo tag(this->trackMap[Bae::KEY::URL]);

            if(_track!=this->trackMap[Bae::KEY::TRACK])
                tag.setTrack(_track.toInt());
            if(_title!=this->trackMap[Bae::KEY::TITLE])
                tag.setTitle(_title);
            if(_artist!=this->trackMap[Bae::KEY::TRACK])
                tag.setArtist(_artist);
            if(_album!=this->trackMap[Bae::KEY::ALBUM])
                tag.setAlbum(_album);
            if(_genre!=this->trackMap[Bae::KEY::GENRE])
                tag.setGenre(_genre);
        }

        this->trackMap.insert(Bae::KEY::TRACK, _track);
        this->trackMap.insert(Bae::KEY::TITLE, _title);
        this->trackMap.insert(Bae::KEY::ARTIST, _artist);
        this->trackMap.insert(Bae::KEY::ALBUM,_album);
        this->trackMap.insert(Bae::KEY::GENRE,_genre);

        emit infoModified(this->trackMap);
    }

    this->close();
}
