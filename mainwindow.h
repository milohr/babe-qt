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

    enum views
    {
        COLLECTION,ALBUMS,FAVORITES,PLAYLISTS,QUEUE,INFO,SETTINGS,RESULTS
    };
    enum utilsBar
    {
        PLAYLISTS_UB,SEARCH_UB,ALBUMS_UB,COLLECTION_UB, FAVORITES_UB,QUEUE_UB,INFO_UB,
    };


protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void resizeEvent(QResizeEvent* event);

public slots:

 void addToPlaylist(QStringList list);

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
    void settingsView();

    /*the view stacked actions*/

    void addToFavorites(QStringList list);
    void addToCollection(QStringList list);
    void addToCollectionDB(QStringList url, QString babe);
    void addToCollectionDB_t(QStringList url);
    void scanNewDir(QString url);


    void setToolbarIconSize(int iconSize);
    void collectionDBFinishedAdding(bool state);

    void on_fav_btn_clicked();
    void on_utilsBar_clicked();

    void hideControls();
    void showControls();

    void on_search_returnPressed();

    void on_search_textChanged(const QString &arg1);

    //void on_resultsPLaylist_clicked();
    void setCoverArt(QString path);

    void on_settings_view_clicked();
    void labelClicked();
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

private:

    Ui::MainWindow *ui;

    void keepOnTop(bool state);
    void setUpViews();
    void loadTrack();
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
    PlaylistsView *playlistTable;

    /*the streaming */
    Playlist playlist;
    QMediaPlayer *player = new QMediaPlayer();
    QTimer *updater = new QTimer(this);
    QString current_song_url;
    QSlider *seekBar;
    QLabel *addMusicImg;
    int mini_mode=0;
    int prevIndex;
    bool hideSearch=true;
    int lCounter = 0;
    int  shuffle_state=0;
    bool repeat = false;
    bool muted = false;
    bool playing = false;
    vector<unsigned short int> shuffledPlaylist;
    bool shuffle = false;
   // void collectionWatcher();

signals:
    void finishedPlayingSong(QString url);
    void collectionChecked();


};




#endif // MAINWINDOW_H
