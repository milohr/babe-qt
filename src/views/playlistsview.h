#ifndef PLAYLISTSVIEW_H
#define PLAYLISTSVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QToolButton>
#include <QGridLayout>
#include <QStandardPaths>
#include <QDialog>
#include <QSplitter>

#include "../utils/bae.h"


class BabeTable;
class PlaylistForm;
class CollectionDB;

namespace PLAYLIST
{
    enum LIST
    {
        MOSTPLAYED = 0,
        FAVORITES = 1,
        RECENT = 2,
        BABES = 3,
        ONLINE = 4,
        TAGS = 5,
        RELATIONS = 6,
        POPULAR = 7
    };
}

class PlaylistsView : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistsView(QWidget *parent = nullptr);
    ~PlaylistsView();
    BabeTable *table;
    QListWidget *list;
    QListWidget *tagList;

    void setPlaylists();
    void setPlaylistsMoods();
    void setDefaultPlaylists();
    void saveToPlaylist(const BAE::DB_LIST &tracks);
    void addToPlaylist(const QString &playlist, const BAE::DB_LIST &tracks);
    bool insertPlaylist(const QString &playlist);
    void refreshCurrentPlaylist();
    void populateTagList(const QString &query);

    QFrame *line_v;
    QWidget *btnContainer;
    QString currentPlaylist = "";
    QStringList playlists;
    QStringList moods = BAE::MoodColors;

private:
    uint ALBUM_SIZE_MEDIUM = BAE::getWidgetSizeHint(BAE::MEDIUM_ALBUM_FACTOR,BAE::AlbumSizeHint::MEDIUM_ALBUM);
    CollectionDB *connection;
    QToolButton *addBtn;
    QToolButton *removeBtn;
    QGridLayout *layout;
    QFrame *frame;
    QWidget *moodWidget;
    QAction *removeFromPlaylist;

    void createPlaylist();
    void removePlaylist();

public slots:  
    void playlistName(QListWidgetItem *item);
    void populatePlaylist(const QModelIndex &index);

signals:
    void modifyPlaylistName(const QString &newName);
    void addedToPlaylist(const BAE::DB_LIST, const QString &playlist);

};

#endif // PLAYLISTSVIEW_H
