#include "babes.h"
#include "ui_babes.h"

babes::babes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::babes)
{
    ui->setupUi(this);

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setColumnHidden(LOCATION, true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->sortByColumn(1,Qt::AscendingOrder);

}

babes::~babes()
{
    delete ui;
}
