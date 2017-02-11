#include "youtube.h"
#include "ui_youtube.h"
#include <QProcess>
#include <QDebug>
#include <QStandardPaths>
YouTube::YouTube(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::YouTube)
{

    ui->setupUi(this);
}

YouTube::~YouTube()
{
    delete ui;
}

void YouTube::on_goBtn_clicked()
{
    if(!ui->lineEdit->text().isEmpty())
    {
        QProcess process;
        process.setWorkingDirectory(cachePath);

        process.start(ydl+" "+ui->lineEdit->text());
        if (process.waitForStarted() && process.waitForFinished()) {
          QString StdOut_sdcompare = process.readAllStandardOutput();
          QString StdError_sdcompare = process.readAllStandardError();

          if(StdOut_sdcompare.isEmpty())
              qDebug()<<   process.program()<<process.arguments();
          }
        }
    }


void YouTube::processFinished()
{
    qDebug()<<"finished the process";
}
