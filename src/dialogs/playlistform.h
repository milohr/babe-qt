#ifndef PLAYLISTFORM_H
#define PLAYLISTFORM_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QString>
#include "../utils/bae.h"

namespace Ui { class PlaylistForm; }

class PlaylistForm : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistForm(const QStringList &playlists,const Bae::DB_LIST &tracks, QWidget *parent = nullptr);
    ~PlaylistForm();

private slots:
    void on_saveBtn_clicked();


    void on_cancelBtn_clicked();

    void on_lineEdit_returnPressed();

    void on_createBtn_clicked();

private:
    Ui::PlaylistForm *ui;
    Bae::DB_LIST tracks;
    QStringList playlists;
    void addPlaylist();

signals:
    void saved(const QString &playlist,const Bae::DB_LIST &tracks);
    void canceled();
    void created(const QString &newPlaylist);

};

#endif // PLAYLISTFORM_H
