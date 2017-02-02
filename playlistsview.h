#ifndef PLAYLISTSVIEW_H
#define PLAYLISTSVIEW_H

#include <QWidget>
#include <babetable.h>
#include <QListWidget>
#include <QToolButton>
#include <QGridLayout>

namespace Ui {
class PlaylistsView;
}

class PlaylistsView : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistsView(QWidget *parent = 0);
    ~PlaylistsView();
    QToolButton *addToPlaylist;
    BabeTable *table;
    QListWidget *list;
    void setPlaylists(QStringList playlists);
    void setDefaultPlaylists();
    QFrame *line_v;
    QWidget *btnContainer;
    QString currentPlaylist;

   // QToolButton *removeBtn;

private:

    QToolButton *addBtn;
    QToolButton *removeBtn;
    QGridLayout *layout;

    QFrame *frame;

public slots:

    void createPlaylist();
    void playlistName(QListWidgetItem *item);
    void on_removeBtn_clicked();
    void populatePlaylist(QModelIndex index);
    void tableClicked(QStringList list);
    void dummy();

signals:
    void playlistCreated(QString name);
    void songClicked(QStringList list);
    void playlistClicked(QString playlist);
    void modifyPlaylistName(QString newName);

};

#endif // PLAYLISTSVIEW_H
