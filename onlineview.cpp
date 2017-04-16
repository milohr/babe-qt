#include "onlineview.h"
#include "ui_onlineview.h"

onlineView::onlineView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::onlineView)
{
    ui->setupUi(this);
}

onlineView::~onlineView()
{
    delete ui;
}
