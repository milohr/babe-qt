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

using namespace Bae;

YouTube::YouTube(QObject *parent) : QObject(parent)
{
    this->nof = new Notify(this);

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

void YouTube::fetch(const QStringList &files)
{
    for(auto file: files)
    {
        qDebug()<<file;

        QString title,artist,album,id,page;


        std::ifstream info(file.toStdString());
        std::string line;



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

        DB infoMap;
        infoMap.insert(KEY::TITLE,title);
        infoMap.insert(KEY::ARTIST,artist);
        infoMap.insert(KEY::ALBUM,album);
        infoMap.insert(KEY::URL,page);
        infoMap.insert(KEY::ID,id);

        //qDebug()<<infoMap;

        if(!this->ids.contains(infoMap[KEY::ID]))
        {

            this->ids<<infoMap[KEY::ID];

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

            nof->notify("Song received!", infoMap[KEY::TITLE]+ " - "+ infoMap[KEY::ARTIST]+".\nWait a sec while the track is added to your collection :)");
            auto command = ydl;

            command = command.replace("$$$",infoMap[KEY::URL])+" "+infoMap[KEY::ID];
            qDebug()<<command;
            process->start(command);
        }
    }
}

void YouTube::processFinished_totally(const int &state,const DB &info,const QProcess::ExitStatus &exitStatus)
{
    QString file =this->cachePath+info[KEY::URL]+".m4a";

    auto track = info;
    track.insert(KEY::URL,file);

    qDebug()<<track[KEY::ID]<<track[KEY::TITLE]<<track[KEY::ARTIST]<<track[KEY::URL];

    /*here get metadata*/

    if(exitStatus == QProcess::NormalExit)
    {
        if(Bae::fileExists(file))
        {
            TagInfo tag(file);
            tag.setArtist(track[KEY::ARTIST]);
            tag.setTitle(track[KEY::TITLE]);
            tag.setAlbum(track[KEY::ALBUM]);
            tag.setComment(track[KEY::URL]);

            Pulpo pulpo;
            pulpo.registerServices({PULPO::SERVICES::LastFm, PULPO::SERVICES::Spotify});
            pulpo.setOntology(PULPO::ONTOLOGY::TRACK);
            pulpo.setInfo(PULPO::INFO::METADATA);

            QEventLoop loop;

            QTimer timer;
            timer.setSingleShot(true);
            timer.setInterval(1000);

            connect(&timer, &QTimer::timeout,&loop,&QEventLoop::quit);

            connect(&pulpo, &Pulpo::infoReady, [&loop](const Bae::DB &track,const PULPO::RESPONSE  &res)
            {
                qDebug()<<"SETTING YOUTUBE DOWNLOAD TRACK METADATA";
                if(!res[PULPO::INFO::METADATA].isEmpty())
                {
                    qDebug()<<res[PULPO::INFO::METADATA][PULPO::CONTEXT::ALBUM_TITLE].toString();
                    qDebug()<<res[PULPO::INFO::METADATA][PULPO::CONTEXT::TRACK_NUMBER].toString();

                    qDebug()<<track[KEY::URL];
                    TagInfo tag(track[KEY::URL]);

                    if(!res[PULPO::INFO::METADATA][PULPO::CONTEXT::ALBUM_TITLE].toString().isEmpty())
                        tag.setAlbum(res[PULPO::INFO::METADATA][PULPO::CONTEXT::ALBUM_TITLE].toString());
                    else tag.setAlbum(track[KEY::TITLE]);

                    if(!res[PULPO::INFO::METADATA][PULPO::CONTEXT::TRACK_NUMBER].toString().isEmpty())
                        tag.setTrack(res[PULPO::INFO::METADATA][PULPO::CONTEXT::TRACK_NUMBER].toInt());
                }


                loop.quit();

            });

            pulpo.feed(track, PULPO::RECURSIVE::OFF);

            timer.start();
            loop.exec();
            timer.stop();
            QString doneId = track[KEY::ID];
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
