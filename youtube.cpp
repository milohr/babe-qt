#include "youtube.h"
#include "ui_youtube.h"

#include <QDebug>
#include <QStandardPaths>
#include <QDirIterator>

YouTube::YouTube(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::YouTube)
{

    ui->setupUi(this);
    ui->label->hide();
    ui->textBrowser->hide(); ui->frame_3->hide();
    movie = new QMovie(":Data/data/ajax-loader.gif");
    ui->label->setMovie(movie);
    searchPendingFiles();
}

YouTube::~YouTube()
{
    delete ui;

}

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

void YouTube::on_goBtn_clicked()
{
    if(!ui->lineEdit->text().isEmpty())
    {
        fetch({ui->lineEdit->text()},{});
    }
}

void YouTube::fetch(QStringList ids,QStringList urls)
{
    this->urls=urls;
    this->ids=ids;
    //cont=ids.size();
    //qDebug()<<"fetching list size: "<<cont;
    for(auto id: ids)
    {
        process = new QProcess(this);
        process->setWorkingDirectory(cachePath);
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(processFinished()));
        connect(process, SIGNAL(finished(int)), this, SLOT(processFinished_totally(int)));
        process->start(ydl+" "+id);
        ui->goBtn->hide();
        ui->label->show();
        movie->start();
        ui->lineEdit->setEnabled(false);

    }
     for(auto url:urls) if(QFile::remove(url)) qDebug()<<"the urls are going to be cleaned up"<< url;

}

void YouTube::processFinished_totally(int state)
{

    qDebug()<<"process finished totally"<<state<<process->arguments();
    QString doneId=process->arguments().at(process->arguments().size()-1);
    ids.removeAll(doneId);
    qDebug()<<"need to delete the id="<<doneId;
      qDebug()<<"ids left to process: "<<ids;
    ui->lineEdit->clear();
    movie->stop();
    ui->label->hide();
    ui->goBtn->show();
    ui->textBrowser->hide(); ui->frame_3->hide();
    ui->lineEdit->setEnabled(true);

    //qDebug()<<ids.size();
    if(ids.isEmpty())
    {

        emit youtubeTrackReady(true);
    }

}


void YouTube::processFinished()
{

    ui->textBrowser->show(); ui->frame_3->show();
    QByteArray processOutput;
    processOutput = process->readAllStandardOutput();
    ui->textBrowser->clear();
    ui->textBrowser->append(QString(processOutput));
    if (!QString(processOutput).isEmpty())
        qDebug() << "Output: " << QString(processOutput);

}
