#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#include "albumsview.h"
#include "babetable.h"
#include "playlistsview.h"
#include "pulpo/pulpo.h"
#include "infoview.h"
#include "rabbitview.h"

#include "album.h"
#include "mpris2.h"
#include "notify.h"
#include "playlist.h"
#include "settings.h"
#include "collectionDB.h"
#include "web_jgm90.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QStringList &files={}, QWidget *parent = nullptr);
    ~MainWindow();

    enum views
    {
        COLLECTION,ALBUMS,ARTISTS,PLAYLISTS,INFO,SETTINGS,RESULTS
    };
    enum playlistViews
    {
        MAINPLAYLIST, FILTERLIST
    };

    enum utilsBar
    {
        VIEWS_UB
    };
    enum viewModes
    {
        FULLMODE, PLAYLISTMODE, MINIMODE
    };
    enum playModes
    {
        REGULAR, SHUFFLE, REPEAT
    };
    enum appendPos
    {
        APPENDTOP,APPENDBOTTOM, APPENDAFTER, APPENDBEFORE, APPENDINDEX
    };

    enum position
    {
        RIGHT,LEFT, IN, OUT
    };


    void start();
    void appendFiles(const QStringList &paths, const appendPos &pos = APPENDBOTTOM);
    void loadTrack();
    bool babeTrack(const Bae::DB &track);
    void loadInfo(const Bae::DB &track);
    Bae::DB_LIST searchFor(const QStringList &queries);


protected:

    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject * watched, QEvent * event) Q_DECL_OVERRIDE;

public slots:

    void addToPlaylist(const Bae::DB_LIST &mapList, const bool &notRepeated=false, const appendPos &pos = APPENDBOTTOM);
    void putPixmap(const QByteArray &array);
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
    void dummy();

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
    void setCoverArt(const Bae::DB &song);
    void on_rowInserted(QModelIndex model ,int x,int y);

    void refreshAlbumsView(const TABLE &type);
    void refreshTables(const Bae::TABLE &reset);
    void addToPlayed(const QString &url);
    void on_tracks_view_2_clicked();
    void on_addAll_clicked();
    void saveResultsTo();

    void removeSong(const int &index);
    void putAlbumOnPlay(const Bae::DB &info);
    void putOnPlay(const Bae::DB_LIST &mapList);
    void playItNow(const Bae::DB_LIST &list);
    void babeAlbum(const Bae::DB &info);
    bool loadCover(const QString &artist, const QString &album, const QString &title);
    void babeIt(const Bae::DB_LIST &tracks);
    bool unbabeIt(const Bae::DB &track);
    void loadMood();
    bool removeQueuedTrack(const Bae::DB &track, const int &pos);
    void removequeuedTracks();
    void on_filterBtn_clicked();
    void on_filter_textChanged(const QString &arg1);
    void infoIt(const  Bae::DB &track);
    void albumDoubleClicked(const Bae::DB &info);
    void on_playAll_clicked();

    void on_miniPlaybackBtn_clicked();

private:

    Ui::MainWindow *ui;
    uint ALBUM_SIZE;
    uint iconSize = 22;

    int playlistPos = RIGHT;
    int playlistSta = IN;

    CollectionDB connection;

    const QString stylePath = Bae::SettingPath+"style.qss";

    int viewMode = FULLMODE;
    QRect defaultGeometry;
    int prevIndex;

    Qt::WindowFlags defaultWindowFlags;
    Notify *nof;
    Mpris *mpris;

    QWidget *mainWidget;
    QHBoxLayout * mainLayout;
    QStackedWidget *views;
    QStackedWidget *mainListView;

    QFrame *leftFrame;
    QGridLayout *leftFrame_layout;

    QFrame *rightFrame;
    QFrame *line;
    QFrame *lineV;

    Album *album_art;
    QWidget *playlistWidget;

    /*the views*/
    BabeTable *mainList;
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
    void setUpWidgets();
    void setUpSidebar();
    void setUpCollectionViewer();
    void setUpPlaylist();
    void setUpRightFrame();

    int shuffleNumber();

    void clearMainList();
    void clearCurrentList();
    void calibrateMainList();

    void updateList();
    void populateMainList();
    int isBabed(const Bae::DB &track);
    void feedRabbit();
    void expandAlbumList(const QString &artist);
    void blurWidget(Album &widget, const int &radius );
    void babedIcon(const bool &state);
    void saveSettings(const QString &key, const QVariant &value,const QString &group);
    void movePanel(const position &pos);
    void loadStyle();
    QVariant loadSettings(const QString &key, const QString &group, const QVariant &defaultValue = QVariant());

    /*the streaming */
    QMediaPlayer *player;
    QTimer *updater = new QTimer(this);

    QSlider *seekBar;
    QMenu *calibrateBtn_menu;  
    QTimer *searchTimer;

    QMap<QString,Bae::DB> queued_songs;
    int prev_queued_song_pos = -1;

    Bae::DB_LIST currentList;

    Bae::DB current_song;
    int current_song_pos=0;
    QString current_artwork;

    Bae::DB prev_song;
    int prev_song_pos=0;

    int  shuffle_state = REGULAR;

    bool repeat = false;
    bool shuffle = false;
    bool stopped = false;
    bool miniPlayback =false;

signals:
    void finishedPlayingSong(QString url);
    void collectionChecked(const Bae::TABLE &reset);
    void fetchCover(const Bae::DB &song);
    void finishRefresh();
};




#endif // MAINWINDOW_H
