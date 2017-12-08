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

#include "../widget_models/babetable.h"
#include "../widget_models/babealbum.h"

#include "../pulpo/pulpo.h"

#include "playlistsview.h"
#include "infoview.h"
#include "rabbitview.h"
#include "albumsview.h"

#include "../kde/notify.h"
#include "../settings/settings.h"
#include "../db/collectionDB.h"

#include "../data_models/tracklist.h"

namespace Ui { class BabeWindow;}

class BabeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BabeWindow(const QStringList &files={}, QWidget *parent = nullptr);
    ~BabeWindow();

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

    enum playlistViews
    {
        MAINPLAYLIST, FILTERLIST
    };

    enum VIEW_MODE
    {
        FULLMODE, PLAYLISTMODE, MINIMODE
    };

    enum PLAY_MODE
    {
        REGULAR, SHUFFLE, REPEAT
    };

    enum APPEND //TODO: move to baetable
    {
        APPENDTOP,APPENDBOTTOM, APPENDAFTER, APPENDBEFORE, APPENDINDEX
    };

    enum POSITION
    {
        RIGHT,LEFT, IN, OUT
    };


    void start();
    void appendFiles(const QStringList &paths, const APPEND &pos = APPENDBOTTOM);
    void loadTrack();
    bool babeTrack(const Bae::DB &track);
    void loadInfo(const Bae::DB &track);
    Bae::DB_LIST searchFor(const QStringList &queries);


protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject * watched, QEvent * event) Q_DECL_OVERRIDE;

public slots:

    void addToPlaylist(const Bae::DB_LIST &mapList, const bool &notRepeated=false, const APPEND &pos = APPENDBOTTOM);
    void populateResultsTable(const Bae::DB_LIST &mapList);
    void addToQueue(const Bae::DB_LIST &tracks);

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
    void on_mainList_clicked(const Bae::DB_LIST &list);
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

    void refreshTables(const QMap<Bae::TABLE, bool> &tableReset);
    void addToPlayed(const QString &url);
    void on_goBackBtn_clicked();

    void removeSong(const int &index);
    void putAlbumOnPlay(const Bae::DB &info);
    void putOnPlay(const Bae::DB_LIST &mapList);
    void playItNow(const Bae::DB_LIST &list);
    void babeAlbum(const Bae::DB &info);
    bool loadCover(DB &track);
    void babeIt(const Bae::DB_LIST &tracks);
    bool unbabeIt(const Bae::DB &track);
    void loadMood();
    void removeQueuedTrack(const int &pos);
    void removequeuedTracks();
    void playQueuedTrack(const int &pos);
    void infoIt(const  Bae::DB &track);
    void albumDoubleClicked(const Bae::DB &info);

    void on_miniPlaybackBtn_clicked();

private:

    Ui::BabeWindow *ui;
    uint ALBUM_SIZE;
    uint iconSize = 22;

    POSITION playlistPos = RIGHT;
    POSITION playlistSta = IN;

    CollectionDB connection;

    const QString stylePath = Bae::SettingPath+"style.qss";

    VIEW_MODE viewMode = VIEW_MODE::FULLMODE;
    QRect defaultGeometry;
    int prevIndex;

    Qt::WindowFlags defaultWindowFlags;
    Notify *nof;

    QToolBar *mainToolbar;
    QToolBar *secondaryToolbar;
    QWidget *mainWidget;
    QHBoxLayout * mainLayout;
    QStackedWidget *views;
    QStackedWidget *mainListView;

    QFrame *rightFrame;
    QFrame *line;
    QFrame *lineV;

    BabeAlbum *album_art;
    QWidget *playlistWidget;

    /*the views*/
    BabeTable *mainList;
    BabeTable *queueList;
    BabeTable *filterList;
    BabeTable *collectionTable;
    BabeTable *resultsTable;

    AlbumsView* albumsTable;
    AlbumsView* artistsTable;

    PlaylistsView *playlistTable;
    InfoView *infoTable;
    settings *settings_widget;
    RabbitView *rabbitTable;

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

    void updateList();
    void populateMainList();
    int isBabed(const Bae::DB &track);
    void expandAlbumList(const QString &artist);
    void blurWidget(BabeAlbum &widget, const int &radius );
    void babedIcon(const bool &state);
    void movePanel(const POSITION &pos);
    void loadStyle();

    /*the streaming */
    QMediaPlayer *player;
    QTimer *updater = new QTimer(this);

    QSlider *seekBar;
    QTimer *searchTimer;

    Bae::DB current_song;
    int current_song_pos = 0;

    Bae::DB prev_song;
    int prev_song_pos = 0;

    PLAY_MODE  shuffle_state = REGULAR;

    bool repeat = false;
    bool stopped = true;
    bool miniPlayback =false;

signals:
    void finishedPlayingSong(QString url);
    void collectionChecked(const QMap<TABLE,bool> &reset);
    void finishRefresh();
};
#endif // BAEWINDOW_H
