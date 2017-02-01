#include "infoview.h"
#include "ui_infoview.h"

InfoView::InfoView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoView)
{
    ui->setupUi(this);
}

InfoView::~InfoView()
{
    delete ui;
}
