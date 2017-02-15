#include "about.h"
#include "ui_about.h"
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFont>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    this->setFixedSize(400,300);
    ui->icon->setPixmap(QPixmap(":Data/data/banner.png"));


    ui->about->appendPlainText("Babe..\nTiny Qt Music Player\nVerion: 0.0 Alpha\nBabe wurde entwickelt und konzepiert von Camilo Higuita \nEs handelt sich um freie und quelloffene Software, der Code kann genommen, bearbeitet und weiter verteilt werden.");
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
    qDebug()<<"Das Label wurde angeklickt";
}

void About::on_pushButton_clicked()
{
    QMessageBox::aboutQt(this,"Babe Qt...");
}

void About::on_codeBtn_clicked()
{

    QDesktopServices::openUrl(this->url);
}
