#include "infoview.h"
#include "ui_infoview.h"
#include <QDebug>
#include <QHBoxLayout>
InfoView::InfoView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoView)
{
    ui->setupUi(this);
    artist= new Album(":Data/data/cover.png",120,100);
    artist->titleVisible(false);
    artist->borderColor=true;
    auto artistContainer = new QWidget();
    artistContainer->setBackgroundRole(QPalette::Dark);
    auto artistCLayout = new QHBoxLayout();
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    artistCLayout->addWidget(left_spacer);
    artistCLayout->addWidget(artist);
    artistCLayout->addWidget(right_spacer);
    artistContainer->setLayout(artistCLayout);

    ui->artistLayout->insertWidget(0,artistContainer);


}

InfoView::~InfoView()
{
    delete ui;
}

void InfoView::setAlbumInfo(QString info)
{

    qDebug()<<info;
    if(info.isEmpty())
    {
        ui->albumFrame->hide();
        ui->frame_5->hide();
    }else
    {
         ui->albumFrame->show();
         ui->frame_5->show();
    ui->albumText->setHtml(info);
    }
}

void InfoView::setAlbumArt(QByteArray array)
{

}

void InfoView::setArtistInfo(QString info)
{
    ui->artistText->setHtml(info);
}

void InfoView::setArtistArt(QByteArray array)
{
    artist->putPixmap(array);
}
