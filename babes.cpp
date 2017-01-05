#include "babes.h"
#include "ui_babes.h"

babes::babes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::babes)
{
    ui->setupUi(this);
}

babes::~babes()
{
    delete ui;
}
