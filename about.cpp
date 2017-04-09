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


#include "about.h"
#include "ui_about.h"


About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    this->setFixedSize(400,300);
    ui->icon->setPixmap(QPixmap(":Data/data/banner.png"));


    ui->about->appendPlainText("Babe..\nTiny Qt Music Player\nVerion: 0.0 Alpha\nBabe is developed and designed by Camilo Higuita \nThis is a free and open source app, so feel free to fork it, submit code or redistribute it");
    //connect(ui->about,SIGNAL(clicked()),this,SLOT(labelClicked()));
    this->setWindowIcon(QIcon(":Data/data/babe_48.svg"));
    this->setWindowIconText("About Babe...");

}

About::~About()
{
    delete ui;
}



void About::labelClicked()
{
    qDebug()<<"the label got clicked";
}

void About::on_pushButton_clicked()
{
    QMessageBox::aboutQt(this,"Babe Qt...");
}

void About::on_codeBtn_clicked()
{

    QDesktopServices::openUrl(this->url);
}
