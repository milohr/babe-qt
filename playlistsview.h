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
    void saveToPlaylist(const BaeUtils::TRACKMAP_LIST &tracks);
    void addToPlaylist(const QString &playlist, const BaeUtils::TRACKMAP_LIST &tracks);
    void populatePlaylist(const QStringList &urls, const QString &playlist);
    void insertPlaylist(const QString &playlist);
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
    QWidget *moodWidget;
    CollectionDB connection;
    Notify nof;


public slots:

    void createPlaylist();
    void removePlaylist();
    void playlistName(QListWidgetItem *item);
    void on_removeBtn_clicked();
    void populatePlaylist(const QModelIndex &index);
    void dummy();

signals:
    void songClicked(QStringList list);
    void playlistClicked(QString playlist);
    void modifyPlaylistName(QString newName);
    void finishedPopulatingPlaylist(QString playlist);

};

#endif // PLAYLISTSVIEW_H
