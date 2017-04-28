#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QtMultimedia/QMediaPlayer>
#include <QFileDialog>
#include <QLabel>
#include <QSlider>
#include <QTimer>
#include <QStackedWidget>
#include <QToolBar>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QWidget>
#include <QStringList>
#include <QMenu>
#include <QFrame>
#include <QSize>

#include <QPixmap>
#include <QDir>
#include <QDirIterator>
#include <QSqlQuery>
#include <QFileInfo>

#include "albumsview.h"
#include "babetable.h"
#include "playlistsview.h"
#include "artwork.h"
#include "infoview.h"
#include "rabbitview.h"

#include "album.h"
#include "mpris2.h"
#include "notify.h"
#include "playlist.h"
#include "settings.h"
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
    void populateMainList();
    void clearCurrentList();
    bool isBabed(QMap<int, QString> track);
    void feedRabbit();
    QStringList searchKeys = {"location:", "artist:", "album:", "title:", "genre:"};

    enum views
    {
        COLLECTION, ALBUMS, ARTISTS, PLAYLISTS, RABBIT, INFO, SETTINGS, RESULTS
    };
    enum utilsBar
    {
        INFO_UB, PLAYLISTS_UB, SEARCH_UB, ALBUMS_UB, ARTISTS_UB, COLLECTION_UB, FAVORITES_UB, RABBIT_UB
    };
    enum viewModes
    {
        FULLMODE, PLAYLISTMODE, MINIMODE
    };
    enum playModes
    {
        REGULAR, SHUFFLE, REPEAT
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
    void addToPlaylist(QList<QMap<int,QString>> mapList, bool notRepeated=false);
    void putPixmap(QByteArray array);    
    void populateResultsTable(QList<QMap<int,QString>> mapList);
    QList<QMap<int,QString>> searchFor(QStringList queries);
    void dummy();

private slots:
    void update();
    void on_hide_sidebar_btn_clicked();
    void on_shuffle_btn_clicked();
    void on_open_btn_clicked();
    void on_mainList_clicked(QList<QMap<int, QString>> list);
    void on_seekBar_sliderMoved(int position);
    void on_play_btn_clicked();
    void on_backward_btn_clicked();
    void on_foward_btn_clicked();

    /*the main views*/
    void collectionView();
    void albumsView();
    void artistsView();
    void rabbitView();
    void playlistsView();
    void infoView();
    void settingsView();

    /*the view stacked actions*/
    bool addToCollectionDB(QStringList url,QString babe = 0);
    void scanNewDir(QString url, QString babe = "0");
    void setToolbarIconSize(int iconSize);
    void collectionDBFinishedAdding(bool state);
    void on_fav_btn_clicked();
    void hideControls();
    void showControls();
    void on_search_returnPressed();
    void on_search_textChanged(const QString &arg1);

    //void on_resultsPLaylist_clicked();
    void setCoverArt(QString artist, QString album, QString title);
    void orderTables();
    void on_rowInserted(QModelIndex model, int x, int y);

    void AlbumsViewOrder(QString order);
    void refreshTables();
    void addToPlayed(QString url);
    void on_refreshBtn_clicked();
    void on_tracks_view_2_clicked();
    void on_refreshAll_clicked();
    void on_addAll_clicked();
    void saveResultsTo(QAction *action);
    void on_saveResults_clicked();

    void removeSong(int index);
    void putOnPlay(QMap<int,QString> info);
    void changedArt(QMap<int, QString> info);
    void babeAlbum(QMap<int, QString> info);
    bool loadCover(QString artist, QString album, QString title);
    bool babeIt(QMap<int, QString> track);
    bool unbabeIt(QMap<int, QString> track);
    void loadMood();
    bool removeQueuedTrack(QMap<int, QString> track);
    void addToQueue(QMap<int,QString> track);
    void on_filterBtn_clicked();
    void on_filter_textChanged(const QString &arg1);
    void infoIt(QString title, QString artist, QString album);
    void albumDoubleClicked(QMap<int, QString> info);

private:
    Ui::MainWindow *ui;
    const QString stylePath = BaeUtils::getSettingPath() + "style.qss";

    Qt::WindowFlags defaultWindowFlags;
    Notify nof;
    ArtWork *coverArt;
    Mpris *mpris;

    void keepOnTop(bool state);

    void setUpViews();
    void setUpWidgets();
    void setUpSidebar();
    void setUpCollectionViewer();
    void setUpPlaylist();
    void setUpRightFrame();
    void setUpActions();

    void loadTrack();
    void next();
    void back();
    void shufflePlaylist();
    void expand();
    void go_mini();
    void go_playlistMode();
    QList<QMap<int, QString>> readTrackData(const QString &album, const QString &artist);

    QFrame *rightFrame;
    QGridLayout *leftFrame_layout;
    QFrame *line;
    QFrame *lineV;
    QStackedWidget *views;
    QToolBar *playback;
    QToolBar *utilsBar;
    QTimer *timer;

    QWidget *mainWidget;
    QGridLayout * mainLayout;
    QLabel *info;
    Album *album_art;
    QWidget *controls;
    QWidget *playlistWidget;
    QFrame *leftFrame;

    /*the views*/
    BabeTable *mainList;
    BabeTable *collectionTable;
    BabeTable *resultsTable;
    AlbumsView* albumsTable;
    AlbumsView* artistsTable;
    PlaylistsView *playlistTable;
    InfoView *infoTable;
    settings *settings_widget;
    RabbitView *rabbitTable;

    /*the streaming */
    QMediaPlayer *player = new QMediaPlayer();
    QTimer *updater = new QTimer(this);

    QList<QMap<int,QString>> queuedList;
    QMap<QString, QMap<int, QString>> queued_songs;
    int queued_song_pos = -1;

    QList<QMap<int,QString>> currentList;
    QMap<int, QString> current_song;
    QMap<int, QString> prev_song;
    int current_song_pos;

    QSlider *seekBar;
    QLabel *addMusicImg;
    QMenu *saveResults_menu;
    QMenu *refreshBtn_menu;

    int mini_mode = FULLMODE;
    int prevIndex;    
    int lCounter = 0;    
    int shuffle_state = REGULAR;

    bool repeat = false;
    bool muted = false;
    bool shuffle = false;

    vector<unsigned short int> shuffledPlaylist;

    QSize prevSize;

signals:
    void finishedPlayingSong(QString url);
    void collectionChecked();
    void fetchCover(QString artist, QString album, QString title);
};




#endif // MAINWINDOW_H
