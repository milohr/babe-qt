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



protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);


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


    void setToolbarIconSize(int iconSize);
    void collectionDBFinishedAdding(bool state);

    void on_fav_btn_clicked();
    void on_searchField_clicked();

    void hideControls();
    void showControls();
    void addToPlaylist(QStringList list);
    void addToFavorites(QStringList list);


    void on_search_returnPressed();

    void on_search_textChanged(const QString &arg1);

    void on_resultsPLaylist_clicked();
    void setCoverArt(QString path);

    void on_settings_view_clicked();
    void labelClicked();

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

    QStackedWidget *views;
    QToolBar *playback;
    QToolBar *utilsBar;
    QTimer *timer;
    QWidget *main_widget;
    QGridLayout * layout;
    QLabel *info;
    QLabel *album_art;
    QWidget *controls;

    /*the views*/

    settings *settings_widget;
    BabeTable *collectionTable;
    BabeTable *favoritesTable;
    BabeTable *resultsTable;
    AlbumsView* albumsTable;
    /*the streaming */
    Playlist playlist;
    QMediaPlayer *player = new QMediaPlayer();
    QTimer *updater = new QTimer(this);
    QString current_song_url;
    QSlider *seekBar;
    int mini_mode=0;
    bool hideSearch=true;
    int lCounter = 0;
    int  shuffle_state=0;
    bool repeat = false;
    bool muted = false;
    bool playing = false;
    vector<unsigned short int> shuffledPlaylist;
    bool shuffle = false;

};




#endif // MAINWINDOW_H
