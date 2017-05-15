#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QDirIterator>
#include <QDockWidget>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QPixmap>
#include <QSize>
#include <QSlider>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QStringList>
#include <QToolBar>
#include <QTimer>
#include <QtMultimedia/QMediaPlayer>
#include <QVBoxLayout>
#include <QWidget>

#include "album.h"
#include "albumsview.h"
#include "babetable.h"
#include "collectionDB.h"
#include "database/database.h"
#include "infoview.h"
#include "mpris2.h"
#include "notify.h"
#include "playlist.h"
#include "playlistsview.h"
#include "pulpo/pulpo.h"
#include "rabbitview.h"
#include "settings.h"
#include "web_jgm90.h"

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

    QStringList searchKeys = {"location:", "artist:", "album:", "title:", "genre:", "online:"};

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
    void addToPlaylist(QList<QMap<int, QString>> mapList, bool notRepeated = false);
    void putPixmap(const QByteArray &array);
    void populateResultsTable(QList<QMap<int, QString>> mapList);
    QList<QMap<int, QString>> searchFor(QStringList queries);
    void dummy();

private slots:
    void on_hide_sidebar_btn_clicked();
    void on_shuffle_btn_clicked();
    void on_open_btn_clicked();
    void on_mainList_clicked(const QList<QMap<int, QString>> &list);
    void update();
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
    bool addToCollectionDB(QStringList url, QString babe = "0");
    void scanNewDir(QString url, QString babe = "0");
    void setToolbarIconSize(const int &iconSize);
    void collectionDBFinishedAdding(bool state);
    void on_fav_btn_clicked();
    void hideControls();
    void showControls();
    void on_search_returnPressed();
    void on_search_textChanged(const QString &arg1);

    //void on_resultsPLaylist_clicked();
    void setCoverArt(const QString &artist, const QString &album, const QString &title);
    void orderTables();
    void on_rowInserted(QModelIndex model, int x, int y);

    void AlbumsViewOrder(const QString &order);
    void refreshTables();
    void addToPlayed(QString url);
    void on_tracks_view_2_clicked();
    void on_addAll_clicked();
    void saveResultsTo(QAction *action);
    void on_saveResults_clicked();

    void removeSong(const int &index);
    void putAlbumOnPlay(QMap<int, QString> info);
    void putOnPlay(const QList<QMap<int, QString>> &mapList);
    void changedArt(QMap<int, QString> info);
    void babeAlbum(QMap<int, QString> info);
    bool loadCover(const QString &artist, const QString &album, const QString &title);
    bool babeIt(const QMap<int, QString> &track);
    bool unbabeIt(QMap<int, QString> track);
    void loadMood();
    bool removeQueuedTrack(QMap<int, QString> track);
    void addToQueue(QMap<int, QString> track);
    void on_filterBtn_clicked();
    void on_filter_textChanged(const QString &arg1);
    void infoIt(const QString &title, const QString &artist, const QString &album);
    void albumDoubleClicked(QMap<int, QString> info);
    void on_playAll_clicked();

private:
    Database *m_database;
    Ui::MainWindow *ui;
    CollectionDB connection;
    const QString stylePath = BaeUtils::getSettingPath() + "style.qss";
    int viewMode = FULLMODE;
    int prevIndex;

    Qt::WindowFlags defaultWindowFlags;
    Notify nof;
    Mpris *mpris;

    QSize prevSize;

    QWidget *mainWidget;
    QGridLayout *mainLayout;
    QStackedWidget *views;

    QDockWidget *leftDock;
    QFrame *leftFrame;
    QGridLayout *leftFrame_layout;

    QDockWidget *rightDock;
    QFrame *rightFrame;
    QFrame *line;
    QFrame *lineV;

    QToolBar *utilsBar;
    Album *album_art;
    QWidget *playlistWidget;

    /*the views*/
    BabeTable *mainList;
    BabeTable *collectionTable;
    BabeTable *resultsTable;
    AlbumsView *albumsTable;
    AlbumsView *artistsTable;
    PlaylistsView *playlistTable;
    InfoView *infoTable;
    settings *settings_widget;
    RabbitView *rabbitTable;
    web_jgm90 *onlineFetcher;

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

    void clearMainList();
    void calibrateMainList();
    void updateList();
    void populateMainList();
    void clearCurrentList();
    bool isBabed(QMap<int, QString> track);
    void feedRabbit();

    /*the streaming */
    QMediaPlayer *player = new QMediaPlayer();
    QTimer *updater = new QTimer(this);
    QTimer *timer;

    QSlider *seekBar;
    QMenu *saveResults_menu;
    QMenu *calibrateBtn_menu;

    QMap<QString, QMap<int, QString>> queued_songs;
    int queued_song_pos = -1;

    QList<QMap<int, QString>> currentList;
    QMap<int, QString> current_song;
    int current_song_pos;

    QMap<int, QString> prev_song;
    int prev_song_pos;

    int lCounter = 0;
    int  shuffle_state = REGULAR;

    bool repeat = false;
    bool muted = false;
    bool shuffle = false;

    vector<unsigned short int> shuffledPlaylist;

signals:
    void collectionChecked();
    void finishedPlayingSong(const QString &url);
    void fetchCover(const QString &artist, const QString &album, const QString &title);
};
#endif // MAINWINDOW_H
