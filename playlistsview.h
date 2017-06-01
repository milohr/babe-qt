#ifndef PLAYLISTSVIEW_H
#define PLAYLISTSVIEW_H

#include <QWidget>

#include <babetable.h>
#include "baeUtils.h"

class TracksDB;
class PlaylistsDB;

class QGridLayout;
class QListWidget;
class QToolButton;
class QListWidgetItem;

namespace Ui {
class PlaylistsView;
}

class PlaylistsView : public QWidget
{
    Q_OBJECT
public:
    explicit PlaylistsView(QWidget *parent = 0);
    ~PlaylistsView();

    void setDefaultPlaylists();
    void setPlaylists(const QStringList &playlists);
    void setPlaylistsMoods(const QStringList &moods_n);
    void definePlaylists(const QStringList &playlists);

    QToolButton *addToPlaylist;
    BabeTable *table;
    QListWidget *list;
    QListWidget *moodList;
    QFrame *line_v;
    QWidget *btnContainer;
    QString currentPlaylist;
    QStringList playlists;
    const QStringList moods = BaeUtils::getMoodColors();
    const QString youtubeCachePath = BaeUtils::getYoutubeCachePath();

private:
    QFrame *frame;
    QGridLayout *layout;
    QToolButton *addBtn;
    QToolButton *removeBtn;
    TracksDB *m_tracksDB;
    PlaylistsDB *m_playlistsDB;

public slots:
    void dummy();
    void createPlaylist();
    void removePlaylist();
    void on_removeBtn_clicked();
    void tableClicked(QStringList list);
    void playlistName(QListWidgetItem *item);
    void populatePlaylist(QModelIndex index);

signals:
    void playlistCreated(const QString &name);
    void songClicked(const QStringList &list);
    void playlistClicked(const QString &playlist);
    void modifyPlaylistName(const QString &newName);
};

#endif // PLAYLISTSVIEW_H
