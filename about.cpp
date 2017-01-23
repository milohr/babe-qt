#include "about.h"
#include "ui_about.h"
#include <QDebug>
About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    this->setFixedSize(400,300);
    ui->icon->setPixmap(QPixmap(":Data/data/banner.png"));
    ui->about->appendPlainText("Babe...\nTiny Qt Music Player\nVerion: 0.0 Alpha\nBabe is developed and designed by Camilo Higuita \nThis is a free and open source app, so feel free to fork it, submit code or redistribute it");
    //connect(ui->about,SIGNAL(clicked()),this,SLOT(labelClicked()));


}

About::~About()
{
    delete ui;
}



void About::labelClicked()
{
    qDebug()<<"the label got clicked";
}
