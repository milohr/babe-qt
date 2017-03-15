#include "form.h"
#include "ui_form.h"

Form::Form(QStringList info, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
     setWindowFlags(flags);
     //mapFromParent(QPoint(100, 100));

     track=info.at(BabeTable::TRACK);
     title=info.at(BabeTable::TITLE);
     artist=info.at(BabeTable::ARTIST);
     album=info.at(BabeTable::ALBUM);
     genre=info.at(BabeTable::GENRE);

     ui->trackLine->setText(track);
     ui->titleLine->setText(title);
     ui->genreLine->setText(genre);
     ui->artistLine->setText(artist);
     ui->albumLine->setText(album);

}

Form::~Form()
{
    delete ui;
}


void Form::on_pushButton_2_clicked()
{
    this->close();
    this->destroy();
}

void Form::on_changebtn_clicked()
{
    QString _track = ui->trackLine->text();
    QString _title = ui->titleLine->text();
    QString _artist = ui->artistLine->text();
    QString _album = ui->albumLine->text();
    QString _genre = ui->genreLine->text();

    if(_track!=this->track || _title!=this->title || _artist!=this->artist || _album!=this->album || _genre!=this->genre)
    {
        qDebug()<< "the info did changed";
        const QMap<int, QString> map{{BabeTable::TRACK, _track}, {BabeTable::TITLE, _title}, {BabeTable::ARTIST, _artist},{BabeTable::ALBUM,_album},{BabeTable::GENRE,_genre}};

        emit infoModified(map);
    }else
    {
        qDebug()<< "the info didn't changed";
    }
    this->close();
}
