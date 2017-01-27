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


    ui->about->appendPlainText("Babe..\nTiny Qt Music Player\nVerion: 0.0 Alpha\nBabe is developed and designed by Camilo Higuita \nThis is a free and open source app, so feel free to fork it, submit code or redistribute it");
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
    qDebug()<<"the label got clicked";
}

void About::on_pushButton_clicked()
{
    QMessageBox::aboutQt(this,"Babe Qt...");
}

void About::on_codeBtn_clicked()
{

     QDesktopServices::openUrl(this->url);
}
