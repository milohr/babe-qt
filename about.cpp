#include "about.h"
#include "ui_about.h"
#include <QDebug>
About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->icon->setPixmap(QPixmap("../player/data/babe.svg").scaled(100,100,Qt::KeepAspectRatio));
    ui->about->setText("Babe...\nTiny Qt Music Player\nVerion: 0.0 Alpha\nBabe is developed and designed\nby Camilo Higuita \nand it is a free and open source app, so feel free\nto fork it, submit code or redistribute it");
    //connect(ui->about,SIGNAL(clicked()),this,SLOT(labelClicked()));


}

About::~About()
{
    delete ui;
}

void About::on_about_linkActivated(const QString &link)
{
    qDebug()<<"the label got clicked";
}

void About::labelClicked()
{
    qDebug()<<"the label got clicked";
}
