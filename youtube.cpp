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


#include "youtube.h"



YouTube::YouTube(QObject *parent) : QObject(parent)
{

    /*    ui->setupUi(this);
    ui->label->hide();
    ui->textBrowser->hide(); ui->frame_3->hide();
    movie = new QMovie(":Data/data/ajax-loader.gif");
    ui->label->setMovie(movie);*/

}

YouTube::~YouTube(){  }

void YouTube::searchPendingFiles()
{
    QDirIterator it(extensionFetchingPath, QStringList() << "*.babe", QDir::Files);
    while (it.hasNext())
    {
        QString song = it.next();

        this->urls<<song;
        QFileInfo fileInfo(QFile(song).fileName());
        QString id=fileInfo.fileName().section(".",0,-2);
        this->ids<<id;

    }

    if (!urls.isEmpty())
    {

        fetch(ids,urls);
        qDebug()<<ids;
    }
}



void YouTube::fetch(QStringList ids_,QStringList urls_)
{
    for(auto url: urls_) if(!this->urls.contains(url)) this->urls<<url;

    //cont=ids.size();
    //qDebug()<<"fetching list size: "<<cont;
    for(auto id: ids_)
    {
        if(!this->ids.contains(id)) this->ids<<id;
        auto process = new QProcess(this);
        process->setWorkingDirectory(cachePath);
        //connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(processFinished()));
        //connect(process, SIGNAL(finished(int)), this, SLOT(processFinished_totally(int)));
        connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
        {

        qDebug()<<"processFinished_totally"<<exitCode<<exitStatus;
        processFinished_totally(exitCode,process->arguments().at(process->arguments().size()-1),exitStatus);

        });


        process->start(ydl+" "+id);

    }

    qDebug()<<"ids in queue:"<<this->ids;
      for(auto url:urls) if(QFile::remove(url)) qDebug()<<"the urls are going to be cleaned up"<< url;


}

void YouTube::processFinished_totally(int state,QString id,QProcess::ExitStatus exitStatus)
{
        QString doneId=id;
        qDebug()<<"process finished totally for"<<state<<doneId<<exitStatus;

        qDebug()<<"need to delete the id="<<doneId;
        ids.removeAll(doneId);
        qDebug()<<"ids left to process: "<<ids;

        //qDebug()<<ids.size();
        if(ids.isEmpty())
        {

            emit youtubeTrackReady(true);

        }
}


void YouTube::processFinished()
{


    /* QByteArray processOutput;
    processOutput = process->readAllStandardOutput();

    if (!QString(processOutput).isEmpty())
        qDebug() << "Output: " << QString(processOutput);*/

}
