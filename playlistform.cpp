#include "playlistform.h"
#include "ui_playlistform.h"

PlaylistForm::PlaylistForm(const QStringList &playlists, const Bae::TRACKMAP_LIST &tracks, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistForm)
{
    ui->setupUi(this);

    this->playlists = playlists;
    this->tracks=tracks;

    setWindowFlags(Qt::Dialog);

    this->setWindowModality(Qt::ApplicationModal);
    auto size = this->tracks.size();
    auto title = QString("Add %1 track%2 to ...").arg(QString::number(size),size>1?"s":"");

    connect(ui->listWidget, &QListWidget::itemSelectionChanged,[this]()
    {
        auto playlist = ui->listWidget->selectedItems().first()->text();
        auto size = this->tracks.size();

        auto title = QString("Add %1 track%2 to %3").arg(QString::number(size),size>1?"s":"",playlist);
        this->setWindowTitle(title);

    });

    this->setWindowTitle(title);

    this->ui->listWidget->addItems(this->playlists);


}

PlaylistForm::~PlaylistForm()
{
    delete ui;
}



void PlaylistForm::on_saveBtn_clicked()
{
    auto selection = ui->listWidget->selectedItems();

    if(!selection.isEmpty())
    {

        auto playlist = selection.first()->text();
        if(!playlist.isEmpty())
        {
            emit saved(playlist,this->tracks);

            this->close();
            this->destroy();
        }
    }else if(!ui->lineEdit->text().isEmpty() && !this->playlists.contains(ui->lineEdit->text()))
    {
        auto playlist =ui->lineEdit->text();
        emit created(playlist);
        emit saved(playlist,this->tracks);
        this->close();
        this->destroy();
    }

}

void PlaylistForm::on_cancelBtn_clicked()
{
    this->close();
    this->destroy();
}

void PlaylistForm::on_lineEdit_returnPressed()
{
    addPlaylist();

}

void PlaylistForm::on_createBtn_clicked()
{
    addPlaylist();
}

void PlaylistForm::addPlaylist()
{
    auto newPlaylist = ui->lineEdit->text();
    if(!newPlaylist.isEmpty() && !playlists.contains(newPlaylist))
    {
        ui->listWidget->addItem(newPlaylist);
        ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
        emit created(newPlaylist);
    }

    ui->lineEdit->setText("");
}
