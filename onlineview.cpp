#include "onlineview.h"
#include "ui_onlineview.h"

OnlineView::OnlineView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineView)
{
    ui->setupUi(this);

    auto layout = new QVBoxLayout(this);

    searchField = new QLineEdit();
    searchField->setPlaceholderText("Search...");

    table = new BabeTable();

    layout->addWidget(searchField);
    layout->addWidget(table);

    this->setLayout(layout);

}

OnlineView::~OnlineView()
{
    delete ui;
}
