#ifndef PLAYLISTSVIEW_H
#define PLAYLISTSVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QToolButton>
#include <QGridLayout>
#include <QStandardPaths>
#include <QDialog>
#include <QSplitter>

#include "../widget_models/babetable.h"
#include "../utils/bae.h"
#include "../dialogs/playlistform.h"
#include "../db/collectionDB.h"
#include "../kde/notify.h"

class PlaylistsView : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistsView(QWidget *parent = nullptr);
    ~PlaylistsView();
    BabeTable *table;
    QListWidget *list;
    QListWidget *tagList;

    void setPlaylists(const QStringList &playlists);
    void setPlaylistsMoods();
    void setDefaultPlaylists();
    void showPlaylistDialog();
    void saveToPlaylist(const Bae::DB_LIST &tracks);
    void addToPlaylist(const QString &playlist, const Bae::DB_LIST &tracks);
    bool insertPlaylist(const QString &playlist);
    void refreshCurrentPlaylist();
    void populateTagList();

    QFrame *line_v;
    QFrame *line_v2;
    QWidget *btnContainer;
    QString currentPlaylist;
    QStringList playlists;
    QStringList moods = Bae::MoodColors;
    QString youtubeCachePath=Bae::YoutubeCachePath;

    // QToolButton *removeBtn;

private:
    uint ALBUM_SIZE_MEDIUM = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::AlbumSizeHint::MEDIUM_ALBUM);

    QToolButton *addBtn;
    QToolButton *removeBtn;
    QGridLayout *layout;
    QFrame *frame;
    QWidget *moodWidget;
    CollectionDB connection;
    Notify *nof;
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
