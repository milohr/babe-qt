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
    QFrame *line_v;
   // QToolButton *removeBtn;

private:

    QToolButton *addBtn;
    QToolButton *removeBtn;
    QGridLayout *layout;
    QWidget *btnContainer;
    QFrame *frame;

public slots:

    void createPlaylist();
    void playlistName(QListWidgetItem *item);
    void on_removeBtn_clicked();

signals:
    void playlistCreated(QString name);

};

#endif // PLAYLISTSVIEW_H
