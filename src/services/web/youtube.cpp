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
#include "../../pulpo/pulpo.h"
#include "../../kde/notify.h"
#include "../../views/babewindow.h"

using namespace BAE;

YouTube::YouTube(QObject *parent) : QObject(parent) {}

YouTube::~YouTube(){}

void YouTube::fetch(const QString &json)
{
    QJsonParseError jsonParseError;
    auto jsonResponse = QJsonDocument::fromJson(json.toUtf8(), &jsonParseError);

    if (jsonParseError.error != QJsonParseError::NoError) return;
    if (!jsonResponse.isObject()) return;

    QJsonObject mainJsonObject(jsonResponse.object());
    auto data = mainJsonObject.toVariantMap();

    auto id = data.value("id").toString().trimmed();
    auto title = data.value("title").toString().trimmed();
    auto artist = data.value("artist").toString().trimmed();
    auto album = data.value("album").toString().trimmed();
    auto page = data.value("page").toString().replace('"',"").trimmed();

    qDebug()<< id << title << artist;

    DB infoMap;
    infoMap.insert(KEY::TITLE,title);
    infoMap.insert(KEY::ARTIST,artist);
    infoMap.insert(KEY::ALBUM,album);
    infoMap.insert(KEY::URL,page);
    infoMap.insert(KEY::ID,id);

    if(!this->ids.contains(infoMap[KEY::ID]))
    {
        this->ids << infoMap[KEY::ID];

        auto process = new QProcess(this);
        process->setWorkingDirectory(YoutubeCachePath);
        //connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(processFinished()));
        //connect(process, SIGNAL(finished(int)), this, SLOT(processFinished_totally(int)));
        connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            //                qDebug()<<"processFinished_totally"<<exitCode<<exitStatus;
            processFinished_totally(exitCode, infoMap, exitStatus);
            process->deleteLater();

        });

        BabeWindow::nof->notify("Song received!", infoMap[KEY::TITLE]+ " - "+ infoMap[KEY::ARTIST]+".\nWait a sec while the track is added to your collection :)");
        auto command = ydl;

        command = command.replace("$$$",infoMap[KEY::URL])+" "+infoMap[KEY::ID];
        qDebug()<<command;
        process->start(command);
    }
}

void YouTube::processFinished_totally(const int &state,const DB &info,const QProcess::ExitStatus &exitStatus)
{
    auto track = info;

    auto doneId = track[KEY::ID];
    auto file = YoutubeCachePath+track[KEY::URL]+".m4a";

    ids.removeAll(doneId);
    track.insert(KEY::URL,file);

    qDebug()<<track[KEY::ID]<<track[KEY::TITLE]<<track[KEY::ARTIST]<<track[KEY::URL];

    /*here get metadata*/

    if(exitStatus == QProcess::NormalExit)
    {
        if(BAE::fileExists(file))
        {
            TagInfo tag(file);
            tag.setArtist(track[KEY::ARTIST]);
            tag.setTitle(track[KEY::TITLE]);
            tag.setAlbum(track[KEY::ALBUM]);
            tag.setComment(track[KEY::URL]);

            qDebug()<<"trying tocollect metadata of downloaded track";
            Pulpo pulpo;
            pulpo.registerServices({PULPO::SERVICES::LastFm, PULPO::SERVICES::Spotify});
            pulpo.setOntology(PULPO::ONTOLOGY::TRACK);
            pulpo.setInfo(PULPO::INFO::METADATA);

            QEventLoop loop;

            QTimer timer;
            connect(&timer, &QTimer::timeout,&loop,&QEventLoop::quit);

            timer.setSingleShot(true);
            timer.setInterval(1000);

            connect(&pulpo, &Pulpo::infoReady, [&loop](const BAE::DB &track, const PULPO::RESPONSE &res)
            {
                qDebug()<<"SETTING YOUTUBE DOWNLOAD TRACK METADATA";
                if(!res[PULPO::ONTOLOGY::TRACK][PULPO::INFO::METADATA].isEmpty())
                {
                    qDebug()<<res[PULPO::ONTOLOGY::TRACK][PULPO::INFO::METADATA][PULPO::CONTEXT::ALBUM_TITLE].toString();
                    qDebug()<<res[PULPO::ONTOLOGY::TRACK][PULPO::INFO::METADATA][PULPO::CONTEXT::TRACK_NUMBER].toString();

                    qDebug()<<track[KEY::URL];
                    TagInfo tag(track[KEY::URL]);

                    if(!res[PULPO::ONTOLOGY::TRACK][PULPO::INFO::METADATA][PULPO::CONTEXT::ALBUM_TITLE].toString().isEmpty())
                        tag.setAlbum(res[PULPO::ONTOLOGY::TRACK][PULPO::INFO::METADATA][PULPO::CONTEXT::ALBUM_TITLE].toString());
                    else tag.setAlbum(track[KEY::TITLE]);

                    if(!res[PULPO::ONTOLOGY::TRACK][PULPO::INFO::METADATA][PULPO::CONTEXT::TRACK_NUMBER].toString().isEmpty())
                        tag.setTrack(res[PULPO::ONTOLOGY::TRACK][PULPO::INFO::METADATA][PULPO::CONTEXT::TRACK_NUMBER].toInt());
                }

                loop.quit();
            });

            pulpo.feed(track, PULPO::RECURSIVE::OFF);

            timer.start();
            loop.exec();
            timer.stop();

            qDebug()<<"process finished totally for"<< state << doneId << exitStatus;

            qDebug()<<"need to delete the id=" << doneId;
            qDebug()<<"ids left to process: " << this->ids;
        }
    }

    if(this->ids.isEmpty()) emit this->youtubeTrackReady(YoutubeCachePath);
}


void YouTube::processFinished()
{
    /* QByteArray processOutput;
    processOutput = process->readAllStandardOutput();

    if (!QString(processOutput).isEmpty())
        qDebug() << "Output: " << QString(processOutput);*/
}
