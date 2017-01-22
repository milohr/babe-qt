#include "playlistsview.h"
#include "ui_playlistsview.h"

PlaylistsView::PlaylistsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistsView)
{
    ui->setupUi(this);
}

PlaylistsView::~PlaylistsView()
{
    delete ui;
}
