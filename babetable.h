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

#include "settings.h"
#include "notify.h"
#include "album.h"
#include "collectionDB.h"
#include "form.h"


namespace Ui {
class BabeTable;
}

class BabeTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit BabeTable(QWidget *parent = 0);
    ~BabeTable();

    enum columns
    {
        TRACK,TITLE,ARTIST,ALBUM,GENRE,LOCATION,STARS,BABE,ART,PLAYED,PLAYLIST,columnsCOUNT
    };
    enum order
    {
        DESCENDING, ASCENDING
    };
    CollectionDB *connection;
    const QMap<int, QString> columnsNames{{TRACK, "track"}, {TITLE, "title"}, {ARTIST, "artist"},{ALBUM,"album"},{GENRE,"genre"},{LOCATION,"location"},{STARS,"stars"},{BABE,"babe"},{ART,"art"},{PLAYED,"played"},{PLAYLIST,"playlist"}};
    void populateTableView(QString indication, bool descriptiveTitle=false);
    void setRating(int rate);
    void passCollectionConnection(CollectionDB *con);
    void setTableOrder(int column, int order);
    void setVisibleColumn(int column);
    void addRow(QMap<int, QString> map, bool descriptiveTooltip=false);
    QMap<int,QString> getRowData(int row);
    void allowDrag();
    //void removeRow(int row);

    void passStyle(QString style);
    QStringList getTableContent(int column);
    QList<QMap<int, QString>> getAllTableContent();
    void passPlaylists();
    void  populatePlaylist(QStringList urls, QString playlist);

    QStringList playlistsMenus;
    QStringList colors = {"#F0FF01","#01FF5B","#3DAEFD","#B401FF","#E91E63"};

    QMap<QString, QString> getKdeConnectDevices();
    QLabel *addMusicTxt;
    QString addMusicMsg = "oops... :(\nnothing here";
    void setAddMusicMsg(QString msg);


protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    //  virtual void mouseReleaseEvent(QMouseEvent* evt);
    virtual void mousePressEvent(QMouseEvent* evt);
    virtual void keyPressEvent(QKeyEvent *event);


private slots:

    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void rateGroup(int id);
    void setUpContextMenu(QPoint pos);
    void addToPlaylist(QAction* action);
    void babeIt_action();
    void sendIt_action(QAction *device);
    void infoIt_action();
    void editIt_action();
    void removeIt_action();
    void moodIt_action(QString color);
    void queueIt_action();
    void moodTrack(int color);
    void update();

public slots:
    QStringList getPlaylistMenus();
    void itemEdited(QMap<int, QString> map);
    void flushTable();

private:

    Notify nof;

    QToolButton *fav1;
    QToolButton *fav2;
    QToolButton *fav3;
    QToolButton *fav4;
    QToolButton *fav5;

    int rRow;
    int rColumn;
    QMenu *contextMenu;
    QMenu* playlistsMenu;
    QMenu* sendToMenu;
    QMenu* moodMenu;
    QMap<QString,QString> devices;
    QTimer *updater = new QTimer(this);



signals:
    void tableWidget_doubleClicked(QList<QMap<int,QString>> mapList);
    void songRated(QStringList list);
    void enteredTable();
    void leftTable();
    void finishedPopulating();
    void rightClicked(QPoint evt);
    void babeIt_clicked(QList<QMap<int,QString>> mapList);
    void removeIt_clicked(int index);
    void createPlaylist_clicked();
    void refreshPlaylistsMenu(QStringList list);
    void moodIt_clicked(QString color);
    void queueIt_clicked(QList<QMap<int,QString>> mapList);
    void infoIt_clicked(QString title, QString artist, QString album);

};

#endif // BABETABLE_H
