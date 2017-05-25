#ifndef BABETABLE_H
#define BABETABLE_H

#include <QWidget>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidgetAction>
#include <QToolButton>
#include <QMouseEvent>
#include <QEvent>
#include <QMap>
#include <QMenu>
#include <QFileInfo>
#include <QVector>
#include <QButtonGroup>
#include <QFontDatabase>
#include <QFont>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QProcess>
#include <QLabel>
#include <QTimer>
#include <QGraphicsEffect>
#include <QGraphicsOpacityEffect>
#include<QItemSelectionModel>

#include "settings.h"
#include "notify.h"
#include "album.h"
#include "collectionDB.h"
#include "metadataForm.h"


namespace Ui { class BabeTable; }

class BabeTable : public QTableWidget
{
    Q_OBJECT

public:

    explicit BabeTable(QWidget *parent = 0);
    ~BabeTable();

    enum columns
    {
        TRACK,TITLE,ARTIST,ALBUM,GENRE,LOCATION,STARS,BABE,ART,PLAYED,PLAYLIST,LYRIC,columnsCOUNT
    };

    enum order
    {
        DESCENDING, ASCENDING
    };

    enum menuActions //this order must be followed
    {
        BABEIT, QUEUEIT, INFOIT, EDITIT, SAVETO, REMOVEIT, RATEIT, MOODIT, SENDIT, ADDTO
    };


//    const QMap<int, QString> columnsNames{{TRACK, "track"}, {TITLE, "title"}, {ARTIST, "artist"},{ALBUM,"album"},{GENRE,"genre"},{LOCATION,"location"},{STARS,"stars"},{BABE,"babe"},{ART,"art"},{PLAYED,"played"},{PLAYLIST,"playlist"}};
    QStringList playlistsMenus;
    QStringList colors = BaeUtils::MoodColors;
    void populateTableView(QList<QMap<int,QString>> mapList, bool descriptiveTitle, bool coloring);
    void populateTableView(QString indication, bool descriptiveTitle, bool coloring);
    void removeMissing(QStringList missingFiles);
    void setRating(int rate);
    void setTableOrder(int column, int order);
    void setVisibleColumn(int column);
    void addRow(QMap<int, QString> map, bool descriptiveTooltip,bool coloring);
    void addRowAt(int row, QMap<int, QString> map, bool descriptiveTooltip, bool coloring);
    void passStyle(QString style);
    void passPlaylists();
    void populatePlaylist(const QStringList &urls, const QString &playlist);
    void setAddMusicMsg(QString msg);
    int getIndex();
    void removeRepeated();
    QList<int> getSelectedRows(const bool &onRightClick=true);
    //void removeRow(int row);

    QMap<int,QString> getRowData(int row);
    QMap<QString, QString> getKdeConnectDevices();
    QStringList getTableColumnContent(const columns &column);
    QList<QMap<int, QString>> getAllTableContent();


protected:

    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    //  virtual void mouseReleaseEvent(QMouseEvent* evt);
    virtual void mousePressEvent(QMouseEvent* evt);
    virtual void keyPressEvent(QKeyEvent *event);

private slots:

    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void rateGroup(int id);
    void setUpContextMenu(const int row, const int column);
    void addToPlaylist(QAction* action);
    void babeIt_action();
    void sendIt_action(QAction *device);
    void infoIt_action();
    void editIt_action();
    void removeIt_action();
    void moodIt_action(const QString &color);
    void queueIt_action();
    void moodTrack(int color);
    void update();

public slots:
    QStringList getPlaylistMenus();
    void itemEdited(QMap<int, QString> map);
    void flushTable();
    void colorizeRow(const QList<int> &rows, const QString &color);

private:

    CollectionDB connection;

    Notify nof;
    QToolButton *fav1;
    QToolButton *fav2;
    QToolButton *fav3;
    QToolButton *fav4;
    QToolButton *fav5;

    int rRow=0;
    int rColumn=0;
    QMenu *contextMenu;
    QMenu* playlistsMenu;
    QMenu* sendToMenu;
    QMenu* moodMenu;
    QMap<QString,QString> devices;
    QTimer *updater = new QTimer(this);

    QLabel *addMusicTxt;
    QString addMusicMsg = "oops... :(\n";

signals:

    void tableWidget_doubleClicked(QList<QMap<int,QString>> mapList);
    void songRated(QStringList list);
    void enteredTable();
    void leftTable();
    void finishedPopulating();
    void rightClicked(const int row, const int column);
    void babeIt_clicked(QList<QMap<int,QString>> tracks);
    void removeIt_clicked(int index);
    void createPlaylist_clicked();
    void refreshPlaylistsMenu(QStringList list);
    void moodIt_clicked(QList<int> rows, QString color);
    void queueIt_clicked(QList<QMap<int,QString>> track);
    void infoIt_clicked(QString title, QString artist, QString album);
    void finishedPopulatingPlaylist(QString playlist);

};

#endif // BABETABLE_H
