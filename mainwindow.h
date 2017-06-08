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
    explicit MainWindow(const QStringList &files={}, QWidget *parent = 0);
    ~MainWindow();

    enum views
    {
        COLLECTION,ALBUMS,ARTISTS,PLAYLISTS,RABBIT,INFO,SETTINGS,RESULTS
    };
    enum playlistViews
    {
        MAINPLAYLIST, FILTERLIST
    };

    enum utilsBar
    {
        INFO_UB,PLAYLISTS_UB,SEARCH_UB,ALBUMS_UB,ARTISTS_UB,COLLECTION_UB,FAVORITES_UB,RABBIT_UB
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

    void appendFiles(const QStringList &paths, const appendPos &pos = APPENDBOTTOM);
    void loadTrack();
    void loadTrackAt(const int &pos);
    bool babeTrack(const QMap<int, QString> &track);
    QList<QMap<int,QString>> searchFor(const QStringList &queries);

    QStringList searchKeys = {"location:","artist:","album:","title:","genre:","online:","playlist:", "stars:"};

protected:

    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject * watched, QEvent * event) Q_DECL_OVERRIDE;

public slots:

    void addToPlaylist(const QList<QMap<int,QString>> &mapList, const bool &notRepeated=false, const appendPos &pos = APPENDBOTTOM);
    void putPixmap(const QByteArray &array);
    void populateResultsTable(const QList<QMap<int,QString>> &mapList);
    void addToQueue(const QList<QMap<int, QString> > &tracks);

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
    void on_mainList_clicked(const QList<QMap<int, QString>> &list);
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
    void addToCollectionDB(const QStringList &url,const QString &babe="0");
    void scanNewDir(const QString &url,const QString &babe="0");
    void setToolbarIconSize(const int &iconSize);
    void collectionDBFinishedAdding();
    void on_fav_btn_clicked();
    void hideControls();
    void showControls();
    void on_search_returnPressed();
    void on_search_textChanged(const QString &arg1);

    //void on_resultsPLaylist_clicked();
    void setCoverArt(const QString &artist, const QString &album, const QString &title);
    void on_rowInserted(QModelIndex model ,int x,int y);

    void refreshTables();
    void addToPlayed(const QString &url);
    void on_tracks_view_2_clicked();
    void on_addAll_clicked();
    void saveResultsTo();

    void removeSong(const int &index);
    void putAlbumOnPlay(const QMap<int,QString> &info);
    void putOnPlay(const QList<QMap<int,QString>> &mapList);
    void playItNow(const QList<QMap<int,QString>> &list);
    void changedArt(const QMap<int, QString> &info);
    void babeAlbum(const QMap<int, QString> &info);
    bool loadCover(const QString &artist, const QString &album, const QString &title);
    void babeIt(const QList<QMap<int, QString>> &tracks);
    bool unbabeIt(const QMap<int, QString> &track);
    void loadMood();
    bool removeQueuedTrack(const QMap<int, QString> &track, const int &pos);
    void removequeuedTracks();
    void on_filterBtn_clicked();
    void on_filter_textChanged(const QString &arg1);
    void infoIt(const QString &title, const QString &artist, const QString &album);
    void albumDoubleClicked(const QMap<int, QString> &info);
    void on_playAll_clicked();

    void on_miniPlaybackBtn_clicked();

private:

    Ui::MainWindow *ui;

    int ALBUM_SIZE;

    CollectionDB connection;

    const QString stylePath = BaeUtils::SettingPath+"style.qss";

    int viewMode = FULLMODE;
    QRect defaultGeometry;
    int prevIndex;

    Qt::WindowFlags defaultWindowFlags;
    Notify nof;
    Mpris *mpris;

    QWidget *mainWidget;
    QGridLayout * mainLayout;
    QStackedWidget *views;
    QStackedWidget *mainListView;

    QFrame *leftFrame;
    QGridLayout *leftFrame_layout;

    QFrame *rightFrame;
    QFrame *line;
    QFrame *lineV;

    QToolBar *utilsBar;
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
    web_jgm90 *onlineFetcher;

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
    bool isBabed(const QMap<int, QString> &track);
    void feedRabbit();
    void expandAlbumList(const QString &artist, const QString &album);
    void blurWidget(Album &widget, const int &radius );
    void saveSettings(const QString &key, const QVariant &value,const QString &group);
    QVariant loadSettings(const QString &key, const QString &group, const QVariant &defaultValue = QVariant());

    /*the streaming */
    QMediaPlayer *player;
    QTimer *updater = new QTimer(this);
    QTimer *timer;

    QSlider *seekBar;
    QMenu *calibrateBtn_menu;

    QMap<QString, QMap<int, QString>> queued_songs;
    int prev_queued_song_pos = -1;

    QList<QMap<int,QString>> currentList;
    QMap<int, QString> current_song;
    int current_song_pos=0;

    QMap<int, QString> prev_song;
    int prev_song_pos=0;

    int  shuffle_state = REGULAR;

    bool repeat = false;
    bool muted = false;
    bool shuffle = false;
    bool stopped = false;
    bool wasPlaying = true;
    bool miniPlayback =false;

signals:
    void finishedPlayingSong(QString url);
    void collectionChecked();
    void fetchCover(QString artist, QString album,QString title);

};




#endif // MAINWINDOW_H
