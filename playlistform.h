#ifndef PLAYLISTFORM_H
#define PLAYLISTFORM_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QString>

namespace Ui {
class PlaylistForm;
}

class PlaylistForm : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistForm(const QStringList &playlists,const QList<QMap<int,QString>> &tracks, QWidget *parent = 0);
    ~PlaylistForm();

private slots:
    void on_saveBtn_clicked();


    void on_cancelBtn_clicked();

    void on_lineEdit_returnPressed();

    void on_createBtn_clicked();

private:
    Ui::PlaylistForm *ui;
    QList<QMap<int,QString>> tracks;
    QStringList playlists;
    void addPlaylist();

signals:
    void saved(QString playlist,QList<QMap<int,QString>> tracks);
    void canceled();
    void created(QString newPlaylist);

};

#endif // PLAYLISTFORM_H
