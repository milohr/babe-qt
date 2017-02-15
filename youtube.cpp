#include "youtube.h"
#include "ui_youtube.h"

#include <QDebug>
#include <QStandardPaths>
YouTube::YouTube(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::YouTube)
{

    ui->setupUi(this);
    ui->label->hide();
    ui->textBrowser->hide(); ui->frame_3->hide();
    movie = new QMovie(":Data/data/ajax-loader.gif");
    ui->label->setMovie(movie);
}

YouTube::~YouTube()
{
    delete ui;

}

void YouTube::on_goBtn_clicked()
{
    if(!ui->lineEdit->text().isEmpty())
    {
        fetch({ui->lineEdit->text()});
    }
}

void YouTube::fetch(QStringList ids)
{
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

}

void YouTube::processFinished_totally(int state)
{
    qDebug()<<"Vorgang vollständig fertig"<<state;
    ui->lineEdit->clear();
    movie->stop();
    ui->label->hide();
    ui->goBtn->show();
    ui->textBrowser->hide(); ui->frame_3->hide();
    ui->lineEdit->setEnabled(true);
    emit youtubeTrackReady(true);
}


void YouTube::processFinished()
{

    ui->textBrowser->show(); ui->frame_3->show();
    QByteArray processOutput;
    processOutput = process->readAllStandardOutput();
    ui->textBrowser->clear();
    ui->textBrowser->append(QString(processOutput));
    qDebug() << "Ausgabe war " << QString(processOutput);




}
