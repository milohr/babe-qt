#ifndef BAEWINDOW_H
#define BAEWINDOW_H

#include <random>
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
#include <QSettings>
#include <QVariant>
#include <QDesktopWidget>
#include <QPoint>
#include <QSize>
#include <QPropertyAnimation>
#include <QMapIterator>
#include <QPixmap>
#include <QDir>
#include <QDirIterator>
#include <QSqlQuery>
#include <QFileInfo>
#include <QObject>
#include <QMenuBar>
#include <QModelIndex>

#include "../utils/bae.h"

class BabeTable;
class Pulpo;
class BabeAlbum;
class PlaylistsView;
class InfoView;
class RabbitView;
class AlbumsView;
class settings;
class Notify;
class CollectionDB;
class Tracklist;

namespace BABEWINDOW
{
    enum VIEWS
    {
        COLLECTION = 0,
        ALBUMS = 1,
        ARTISTS = 2,
        PLAYLISTS = 3,
        INFO = 4,
        RABBIT = 5,
        SETTINGS = 6,
        RESULTS =7
    };

    enum VIEW_MODE
    {
        FULLMODE,
        PLAYLISTMODE,
        MINIMODE
    };

    enum PLAY_MODE
    {
        REGULAR,
        SHUFFLE,
        REPEAT
    };

    enum APPEND
    {
        APPENDTOP,
        APPENDBOTTOM,
        APPENDAFTER,
        APPENDBEFORE,
        APPENDINDEX
    };

    enum POSITION
    {
        RIGHT,
        LEFT,
        IN,
        OUT
    };
}

using namespace BAE;
using namespace BABEWINDOW;

namespace Ui { class BabeWindow;}

class BabeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BabeWindow(const QStringList &files = {}, QWidget *parent = nullptr);
    ~BabeWindow() override;
    static Notify *nof;

    void start();
    void appendFiles(const QStringList &paths, const APPEND &pos = APPENDBOTTOM);
    void loadTrack();
    bool babeTrack(const BAE::DB &track);
    void loadInfo(const BAE::DB &track);

    BAE::DB_LIST searchFor(const QStringList &queries);

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject * watched, QEvent * event) Q_DECL_OVERRIDE;

public slots:
    void addToPlaylist(const BAE::DB_LIST &mapList, const bool &notRepeated=false, const APPEND &pos = APPENDBOTTOM);
    void populateResultsTable(const BAE::DB_LIST &mapList);
    void addToQueue(const BAE::DB_LIST &tracks);

    void pause();
    void play();
    void stop();
    void next();
    void back();

    void expand();
    void go_mini();
    void go_playlistMode();

private slots:
    void on_hide_sidebar_btn_clicked();
    void on_shuffle_btn_clicked();
    void on_open_btn_clicked();
    void on_mainList_clicked(const BAE::DB_LIST &list);
    void update();
    void on_seekBar_sliderMoved(const int &position);
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
    void setToolbarIconSize(const uint &iconSize);

    void on_fav_btn_clicked();
    void showControls(const bool &state);
    void runSearch();

    //void on_resultsPLaylist_clicked();
    void fetchCoverArt(DB &song);
    void on_rowInserted(QModelIndex model ,int x,int y);

    void refreshTables(const QMap<BAE::TABLE, bool> &tableReset);
    void addToPlayed(const QString &url);

    void putAlbumOnPlay(const BAE::DB &info);
    void putOnPlay(const BAE::DB_LIST &mapList);
    void playItNow(const BAE::DB_LIST &list);
    void babeAlbum(const BAE::DB &info);
    bool loadCover(DB &track);
    void babeIt(const BAE::DB_LIST &tracks);
    bool unbabeIt(const BAE::DB &track);
    void loadMood();
    void removeQueuedTrack(const int &pos);
    void removeQueuedTracks();
    void playQueuedTrack(const int &pos);
    void infoIt(const  BAE::DB &track);
    void albumDoubleClicked(const BAE::DB &info);

    void on_miniPlaybackBtn_clicked();

private:

    Ui::BabeWindow *ui;
    uint ALBUM_SIZE;
    uint iconSize = 22;

    CollectionDB *connection;

    POSITION playlistPos = RIGHT;
    POSITION playlistSta = IN;

    const QString stylePath = BAE::SettingPath+"style.qss";

    VIEW_MODE viewMode = VIEW_MODE::FULLMODE;
    QRect defaultGeometry;
    int prevIndex;

    Qt::WindowFlags defaultWindowFlags;

    QToolBar *mainToolbar;
    QWidget *mainWidget;
    QHBoxLayout * mainLayout;

    QWidget *viewsWidget;
    QStackedWidget *views;
    QStackedWidget *mainListView;

    QFrame *rightFrame;
    BabeAlbum *album_art;
    QWidget *playlistWidget;

    /*the views*/
    BabeTable *mainList;
    BabeTable *queueList;
    BabeTable *collectionTable;
    BabeTable *resultsTable;

    AlbumsView* albumsTable;
    AlbumsView* artistsTable;

    PlaylistsView *playlistTable;
    InfoView *infoTable;
    settings *settings_widget;
    RabbitView *rabbitTable;

    QAction *popPanel;

    void keepOnTop(bool state);

    void setUpViews();
    void setUpCollectionViewer();
    void setUpPlaylist();
    void setUpRightFrame();
    void setUpMenuBar();

    int shuffleNumber();

    void clearMainList();
    void clearCurrentList();
    void calibrateMainList();

    void populateMainList();
    int isBabed(const BAE::DB &track);
    void blurWidget(BabeAlbum &widget, const int &radius );
    void babedIcon(const bool &state);
    void movePanel(const POSITION &pos);
    void loadStyle();

    /*the streaming */
    QMediaPlayer *player;
    QTimer *updater;

    QSlider *seekBar;
    QTimer *searchTimer;

    BAE::DB current_song;
    int current_song_pos = 0;

    BAE::DB prev_song;
    int prev_song_pos = 0;

    PLAY_MODE shuffle_state = PLAY_MODE::REGULAR;

    bool repeat = false;
    bool stopped = true;
    bool miniPlayback =false;

};
#endif // BAEWINDOW_H
