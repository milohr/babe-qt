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

        this->urls<<it.next();

    if (!urls.isEmpty()) fetch(urls);

}



void YouTube::fetch(QStringList files)
{

    for(auto file: files)
    {
        qDebug()<<file;

        QString title,artist,album,id,page;


        std::ifstream info(file.toStdString());
        std::string line;

        QMap<int,QString> infoMap;

        while (std::getline(info, line))
        {
            auto infoLine = QString::fromStdString(line);

            if (infoLine.contains("[title] ="))
                title = infoLine.replace("[title] =", "").trimmed();

            else if(infoLine.contains("[artist] ="))
                artist = infoLine.replace("[artist] =", "").trimmed();

            else if (infoLine.contains("[album] ="))
                album = infoLine.replace("[album] =", "").trimmed();

            else if (infoLine.contains("[id] ="))
                id = infoLine.replace("[id] =", "").trimmed();

            else if (infoLine.contains("[page] ="))
                page = infoLine.replace("[page] =", "").trimmed();

        }

        if(QFile(file).remove()) qDebug()<<"removing file"<<file;


        infoMap.insert(TITLE,title);
        infoMap.insert(ARTIST,artist);
        infoMap.insert(ALBUM,album);
        infoMap.insert(COMMENT,page);
        infoMap.insert(ID,id);

        qDebug()<<infoMap;

        if(!this->ids.contains(infoMap[ID]))
        {

            this->ids<<infoMap[ID];

            auto process = new QProcess(this);
            process->setWorkingDirectory(cachePath);
            //connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(processFinished()));
            //connect(process, SIGNAL(finished(int)), this, SLOT(processFinished_totally(int)));
            connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                    [infoMap,this](int exitCode, QProcess::ExitStatus exitStatus)
            {

                //                qDebug()<<"processFinished_totally"<<exitCode<<exitStatus;
                processFinished_totally(exitCode,infoMap,exitStatus);

            });

            nof.notify("Song received!", infoMap[TITLE]+ " - "+ infoMap[ARTIST]+".\nWait a sec while the track is added to your collection :)");
            auto a = ydl;

            a = a.replace("$$$",infoMap[COMMENT])+" "+infoMap[ID];
            qDebug()<<a;
            process->start(a);

        }


    }



}

void YouTube::processFinished_totally(const int &state,const QMap<int,QString> &info,const QProcess::ExitStatus &exitStatus)
{

    QString file =this->cachePath+info[COMMENT]+".m4a";
    qDebug()<<info[ID]<<info[TITLE]<<info[ARTIST]<<file;

    if(exitStatus == QProcess::NormalExit)
    {

        if(Bae::fileExists(file))
        {
            TagInfo tag(file);

            tag.setArtist(info[ARTIST]);
            tag.setTitle(info[TITLE]);
            tag.setAlbum(info[ALBUM]);
            tag.setComment(info[COMMENT]);

            QString doneId=info[ID];
            qDebug()<<"process finished totally for"<<state<<doneId<<exitStatus;

            qDebug()<<"need to delete the id="<<doneId;
            ids.removeAll(doneId);
            qDebug()<<"ids left to process: "<<ids;

            if(ids.isEmpty()) emit youtubeTrackReady(this->cachePath);
        }
    }
}


void YouTube::processFinished()
{


    /* QByteArray processOutput;
    processOutput = process->readAllStandardOutput();

    if (!QString(processOutput).isEmpty())
        qDebug() << "Output: " << QString(processOutput);*/

}
