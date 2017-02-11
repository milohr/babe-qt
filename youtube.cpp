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

        process = new QProcess(this);
        process->setWorkingDirectory(cachePath);

        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(processFinished()));
        process->start(ydl+" "+ui->lineEdit->text());
        ui->goBtn->hide();
        ui->label->show();
        movie->start();
        //connect(&process, SIGNAL(QProcess::finished(int)), this, SLOT(processFinished(int)));


    }
}


void YouTube::processFinished()
{
    QByteArray processOutput;
    processOutput = process->readAllStandardOutput();

    qDebug() << "Output was " << QString(processOutput);
    ui->lineEdit->clear();
    movie->stop();
    ui->label->hide();
    ui->goBtn->show();
    //emit youtubeTrackReady(true);
}
