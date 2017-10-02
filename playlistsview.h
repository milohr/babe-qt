#ifndef PLAYLISTSVIEW_H
#define PLAYLISTSVIEW_H

#include <QWidget>
#include <babetable.h>
#include <QListWidget>
#include <QToolButton>
#include <QGridLayout>
#include <QStandardPaths>
#include <QDialog>
#include <QSplitter>

#include "baeUtils.h"
#include "playlistform.h"

namespace Ui {
class PlaylistsView;
}

class PlaylistsView : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistsView(QWidget *parent = 0);
    ~PlaylistsView();
    BabeTable *table;
    QListWidget *list;
    QListWidget *moodList;

    void setPlaylists(const QStringList &playlists);
    void setPlaylistsMoods();
    void setDefaultPlaylists();
    void showPlaylistDialog();
    void saveToPlaylist(const Bae::DB_LIST &tracks);
    void addToPlaylist(const QString &playlist, const Bae::DB_LIST &tracks);
    bool insertPlaylist(const QString &playlist);
    void refreshCurrentPlaylist();
    QFrame *line_v;
    QWidget *btnContainer;
    QString currentPlaylist;
    QStringList playlists;
    QStringList moods = Bae::MoodColors;
    QString youtubeCachePath=Bae::YoutubeCachePath;

    // QToolButton *removeBtn;

private:

    int ALBUM_SIZE_MEDIUM = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::MEDIUM_ALBUM);

    QToolButton *addBtn;
    QToolButton *removeBtn;
    QGridLayout *layout;
    QFrame *frame;
    QWidget *moodWidget;
    CollectionDB connection;
    Notify nof;
    QAction *removeFromPlaylist;


public slots:

    void createPlaylist();
    void removePlaylist();
    void playlistName(QListWidgetItem *item);
    void on_removeBtn_clicked();
    void populatePlaylist(const QModelIndex &index);
    void dummy();

signals:
    void songClicked(const QStringList &list);
    void playlistClicked(const QString &playlist);
    void modifyPlaylistName(const QString &newName);
    void addedToPlaylist(const Bae::DB_LIST, const QString &playlist);

};

#endif // PLAYLISTSVIEW_H
