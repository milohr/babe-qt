#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QtMultimedia/QMediaPlayer>
#include "playlist.h"
#include "QFileDialog"
#include <QLabel>
#include "QTimer"
#include <QStackedWidget>
#include <QToolBar>
#include "settings.h"
#include "babes.h"
#include "collectionDB.h"


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
    void populateTableView();
    void populateMainList();
    enum columns
    {
        TITLE,ARTIST,ALBUM,LOCATION
    };

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

    void tracksView();
    void albumsView();
    void babesView();
    void queueView();
    void playlistsView();
    void infoView();
    void settingsView();


    void on_info_view_clicked(bool checked);

    void on_tracks_view_clicked(bool checked);

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void setToolbarIconSize(int iconSize);
    void collectionDBFinishedAdding(bool state);


    void on_fav_btn_clicked();

private:

    void setUpViews();
    void loadTrack();
    int getIndex();
    void next();
    void back();
    void shufflePlaylist();
    void expand();
    void go_mini();

    Ui::MainWindow *ui;

    QStackedWidget *views;
    QToolBar *playback;
    QToolBar *status;
    //CollectionDB collection_db;
    int mini_mode;

    bool allow = false;
    bool tracks_hide=false;
    bool albums_hide=false;
    bool babes_hide=false;
    bool queue_hide=false;
    bool playlists_hide=false;
    bool settings_hide=false;
    bool info_hide=false;

    QWidget *main_widget;
    QGridLayout * layout;
    QLabel *info;

    settings *settings_widget;
    babes babes_widget;
    Playlist playlist;

    QMediaPlayer *player = new QMediaPlayer();
    QTimer *updater = new QTimer(this);
    QString current_song_url;
    int lCounter = 0;
    int  shuffle_state=0;
    bool repeat = false;
    bool muted = false;
    bool playing = false;
    vector<unsigned short int> shuffledPlaylist;
    bool shuffle = false;

};




#endif // MAINWINDOW_H
