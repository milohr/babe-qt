#ifndef ALBUMSVIEW_H
#define ALBUMSVIEW_H

#include <QWidget>
#include <QSqlQuery>
#include <QGridLayout>
#include <album.h>
#include <QFrame>
#include <collectionDB.h>
#include <playlist.h>

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

private:
    Ui::AlbumsView *ui;
    QGridLayout *grid;
    QFrame *albumBox_frame;
    Album *cover;
    CollectionDB *connection;
    Playlist *playlist;

private slots:
    void getAlbumInfo(QStringList info);
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void albumHover();

signals:
    void songClicked(QStringList url);
};

#endif // ALBUMSVIEW_H
