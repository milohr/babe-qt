#ifndef ALBUMSVIEW_H
#define ALBUMSVIEW_H

#include <QWidget>
#include <QSqlQuery>
#include <QGridLayout>
#include <album.h>
#include <QFrame>
#include <collectionDB.h>
#include <playlist.h>
#include <QListWidget>
#include <babetable.h>

namespace Ui {
class AlbumsView;
}

class AlbumsView : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumsView(QWidget *parent = 0);
    ~AlbumsView();
    void populateTableView(QSqlQuery query);
    void passConnection(CollectionDB *con);
    void flushGrid();

private:

    QListWidget *grid;

    QFrame *albumBox_frame;
    Album *cover;
    CollectionDB *connection;
    Playlist *playlist;
    BabeTable *albumTable;

private slots:
    void getAlbumInfo(QStringList info);
    void albumTable_clicked(QStringList list);
    void albumTable_rated(QStringList list);
    void albumTable_babeIt(QStringList list);
    void albumHover();

signals:
    void songClicked(QStringList url);
     void songRated(QStringList url);
      void songBabeIt(QStringList url);

};

#endif // ALBUMSVIEW_H
