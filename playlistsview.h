#ifndef PLAYLISTSVIEW_H
#define PLAYLISTSVIEW_H

#include <QWidget>
#include <babetable.h>
#include <QListWidget>
#include <QToolButton>
#include <QGridLayout>
#include <QStandardPaths>
#include "baeUtils.h"
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
    QListWidget *moodList;
    void setPlaylists(QStringList playlists);
    void setPlaylistsMoods(QStringList moods_n);
    void setDefaultPlaylists();
    void definePlaylists(QStringList playlists);
    QFrame *line_v;
    QWidget *btnContainer;
    QString currentPlaylist;
    QStringList playlists;
    QStringList moods = BaeUtils::MoodColors;
    QString youtubeCachePath=BaeUtils::YoutubeCachePath;

   // QToolButton *removeBtn;

private:

    int ALBUM_SIZE_MEDIUM = BaeUtils::getWidgetSizeHint(BaeUtils::MEDIUM_ALBUM_FACTOR,BaeUtils::MEDIUM_ALBUM);

    QToolButton *addBtn;
    QToolButton *removeBtn;
    QGridLayout *layout;

    QFrame *frame;

    CollectionDB connection;

public slots:

    void createPlaylist();
    void removePlaylist();
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
