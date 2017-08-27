#ifndef PLAYLISTFORM_H
#define PLAYLISTFORM_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QString>
#include "baeUtils.h"

namespace Ui {
class PlaylistForm;
}

class PlaylistForm : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistForm(const QStringList &playlists,const Bae::TRACKMAP_LIST &tracks, QWidget *parent = 0);
    ~PlaylistForm();

private slots:
    void on_saveBtn_clicked();


    void on_cancelBtn_clicked();

    void on_lineEdit_returnPressed();

    void on_createBtn_clicked();

private:
    Ui::PlaylistForm *ui;
    Bae::TRACKMAP_LIST tracks;
    QStringList playlists;
    void addPlaylist();

signals:
    void saved(QString playlist,Bae::TRACKMAP_LIST tracks);
    void canceled();
    void created(QString newPlaylist);

};

#endif // PLAYLISTFORM_H
