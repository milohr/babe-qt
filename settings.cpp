#include "settings.h"
#include "ui_settings.h"
#include "QDebug"
settings::settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
}

settings::~settings()
{
    delete ui;
}

void settings::on_comboBox_activated(const QString &arg1)
{
    //qDebug () <<arg1;
    iconSize=arg1.toInt();
    emit toolbarIconSizeChanged(iconSize);
}


