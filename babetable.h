#ifndef BABETABLE_H
#define BABETABLE_H

#include <QWidget>
#include <QStringList>
#include <collectionDB.h>
#include <QTableWidget>
#include <QToolButton>
#include <QMouseEvent>
#include <QMenu>
#include <QFileInfo>
#include <QVector>
#include "album.h"

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
        TRACK,TITLE,ARTIST,ALBUM,GENRE,LOCATION,STARS,BABE,ART,PLAYED,PLAYLIST
    };
    enum order
    {
        DESCENDING, ASCENDING
    };
    CollectionDB *connection;

    void populateTableView(QString indication, bool descriptiveTitle=false);
    void setRating(int rate);
    void passCollectionConnection(CollectionDB *con);
    void setTableOrder(int column, int order);
    void setVisibleColumn(int column);
    void addRow(QStringList list);
    QStringList getRowData(int row);
    void allowDrag();
    //void removeRow(int row);
    void flushTable();
    void passStyle(QString style);
    QStringList getTableContent(int column);
    QList<QStringList> getAllTableContent();
    void passPlaylists();
    void  populatePlaylist(QStringList urls, QString playlist);
    QMenu* playlistsMenu;
    QMenu* moodMenu;
    QStringList playlistsMenus;
    QStringList colors = {"#F44336","#E91E63","#9C27B0","#2196F3","#CDDC39"};


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
    void removeIt_action();
    void moodIt_action(QString color);
    void queueIt_action();
    void moodTrack(int color);

public slots:
    QStringList getPlaylistMenus();

private:


    QToolButton *fav1;
    QToolButton *fav2;
    QToolButton *fav3;
    QToolButton *fav4;
    QToolButton *fav5;
    int row;
    QMenu *contextMenu;


signals:
    void tableWidget_doubleClicked(QList<QStringList> list);
    void songRated(QStringList list);
    void enteredTable();
    void leftTable();
    void finishedPopulating();
    void rightClicked(QPoint evt);
    void babeIt_clicked(QList<QStringList> list);
    void removeIt_clicked(int index);
    void createPlaylist_clicked();
    void refreshPlaylistsMenu(QStringList list);
    void moodIt_clicked(QString color);
    void queueIt_clicked(QString url);

};

#endif // BABETABLE_H
