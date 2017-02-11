#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QtMultimedia/QMediaPlayer>
#include "playlist.h"
#include "QFileDialog"
#include <QLabel>
#include <QSlider>
#include "QTimer"
#include <QStackedWidget>
#include <QToolBar>
#include "settings.h"
#include "collectionDB.h"
#include <albumsview.h>
#include <babetable.h>
#include<QListWidgetItem>
#include <album.h>
#include <playlistsview.h>
#include <artwork.h>
#include <infoview.h>
#include <lyrics.h>
#include <QFileInfo>
#include "mpris2.h"
#include "youtube.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setStyle();
    void updateList();
    void populateMainList();
    bool fileExists(QString url);


    enum views
    {
        COLLECTION,ALBUMS,ARTISTS,PLAYLISTS,QUEUE,INFO,YOUTUBE,SETTINGS,RESULTS
    };
    enum utilsBar
    {
        INFO_UB,PLAYLISTS_UB,SEARCH_UB,ALBUMS_UB,COLLECTION_UB, FAVORITES_UB,QUEUE_UB
    };


protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);

public slots:

    void addToPlaylist(QStringList list);
    void putPixmap(QByteArray array);
    void dummy();
private slots:

    void on_hide_sidebar_btn_clicked();
    void on_shuffle_btn_clicked();
    void on_open_btn_clicked();
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void update();
    void on_seekBar_sliderMoved(int position);
    void on_play_btn_clicked();
    void on_backward_btn_clicked();
    void on_foward_btn_clicked();

    /*the main views*/
    void collectionView();
    void albumsView();
    void favoritesView();
    void queueView();
    void playlistsView();
    void infoView();
    void youtubeView();
    void settingsView();

    /*the view stacked actions*/

    void addToFavorites(QStringList list);
    void addToCollection(QStringList list);
    void addToCollectionDB_t(QStringList url,QString babe=0);
    void scanNewDir(QString url);


    void setToolbarIconSize(int iconSize);
    void collectionDBFinishedAdding(bool state);

    void on_fav_btn_clicked();


    void hideControls();
    void showControls();

    void on_search_returnPressed();

    void on_search_textChanged(const QString &arg1);

    //void on_resultsPLaylist_clicked();
    void setCoverArt(QString artist, QString album, QString title);

    void on_settings_view_clicked();
    void orderTables();

    void on_rowInserted(QModelIndex model ,int x,int y);
    void showAlbumViewUtils();
    void hideAlbumViewUtils();
    void AlbumsViewOrder(QString order);
    void refreshTables();
    void addToPlayed(QString url);


    void on_refreshBtn_clicked();

    void on_tracks_view_2_clicked();

    void on_refreshAll_clicked();

    void on_addAll_clicked();
    void saveResultsTo(QAction *action);
    void on_saveResults_clicked();
    void setLyrics(QString artist,QString title);
    void getTrackInfo();
    void removeSong(int index);
    void putOnPlay(QString artist, QString album);
    void loadCover(QString artist, QString album, QString title);
    void babeIt(QString url);
    void unbabeIt(QString url);
    void loadMood();
    void addToQueue(QString url);
    void removeFromQueue(QString url);

private:

    Ui::MainWindow *ui;
    ArtWork *coverArt;
    ArtWork *artistHead;
    Lyrics *lyrics;
    Mpris *mpris;
    void keepOnTop(bool state);
    void setUpViews();
    void loadTrack();
    void loadTrackOnQueue();
    int getIndex();
    void next();
    void back();
    void shufflePlaylist();
    void expand();
    void go_mini();
    QFrame *frame;
    QFrame *line;
    QVBoxLayout *frame_layout;
    QStackedWidget *views;
    QToolBar *playback;
    QToolBar *utilsBar;
    QTimer *timer;
    QWidget *main_widget;
    QGridLayout * layout;
    QLabel *info;
    Album *album_art;
    QWidget *controls;
    QFrame *album_art_frame;

    /*the views*/

    settings *settings_widget;
    BabeTable *collectionTable;
    BabeTable *favoritesTable;
    BabeTable *resultsTable;
    AlbumsView* albumsTable;
    AlbumsView* artistsTable;
    PlaylistsView *playlistTable;
    InfoView *infoTable;
    BabeTable *queueTable;
    YouTube *youtubeTable;
    /*the streaming */
    Playlist playlist;
    Playlist queueList;
    QStringList queue_list;
    QMediaPlayer *player = new QMediaPlayer();
    QTimer *updater = new QTimer(this);
    QString current_song_url;
    QString current_title;
    QString current_album;
    QString current_artist;
    QSlider *seekBar;
    QLabel *addMusicImg;
    int mini_mode=0;
    int prevIndex;

    int lCounter = 0;
    int  shuffle_state=0;
    bool repeat = false;
    bool muted = false;
    bool playing = false;
    vector<unsigned short int> shuffledPlaylist;
    bool shuffle = false;
    QMenu *saveResults_menu;
    // void collectionWatcher();
    QString listToString(QStringList list);

signals:
    void finishedPlayingSong(QString url);
    void collectionChecked();
    void getCover(QString artist, QString album, QString title);


};




#endif // MAINWINDOW_H
