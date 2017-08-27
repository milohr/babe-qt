#ifndef ALBUMSVIEW_H
#define ALBUMSVIEW_H

#include <QWidget>
#include <QSqlQuery>
#include <QGridLayout>
#include <QFrame>
#include <QListWidgetItem>
#include <QListWidget>
#include <QComboBox>
#include <QHeaderView>
#include <QToolTip>
#include <QDebug>
#include <QShortcut>
#include <QSplitter>
#include <QScrollBar>
//#include <QGraphicsBlurEffect>
//#include <QGraphicsScene>
//#include <QGraphicsPixmapItem>

#include "scrolltext.h"
#include "album.h"
#include "babetable.h"
#include "collectionDB.h"

namespace Ui {
class AlbumsView;
}

class AlbumsView : public QWidget
{
    Q_OBJECT

public:

    explicit AlbumsView(bool extraList=false, QWidget *parent = 0);
    ~AlbumsView();
    void populateTableView();
    void populateTableViewHeads();
    void populateExtraList(QSqlQuery query);
    void flushGrid();
    int getAlbumSize() { return this->albumSize; }
    void hide_all(bool state);
    void filter(const Bae::TRACKMAP_LIST &filter, const Bae::TracksCols &type);
    QSlider *slider;
    QAction *order;
    QFrame *utilsFrame;
    BabeTable *albumTable;
    QListWidget *grid;
    QToolButton *expandBtn;


    enum ALBUMSVIEW_H{ TITLE, ARTIST, ARTWORK };

private:

    int albumSize;
    bool extraList=false;
    bool ascending=true;
    bool hiddenLabels=false;
    void adjustGrid();
    QList<Album*> albumsList;
    QList<QListWidgetItem*> itemsList;
    QStringList albums;
    QStringList artists;
    QWidget *albumBox_frame;
    QFrame *line_h;
    Album *cover;
    CollectionDB connection;
    Playlist *playlist;
    QListWidget *artistList;
    QToolButton *closeBtn;

public slots:

    void hideAlbumFrame();
    void expandList();
    void changedArt_cover(QMap<int,QString> info);
    void changedArt_head(QMap<int,QString> info);
    void babeAlbum(QMap<int,QString> info);
    void setAlbumsSize(int value);
    void getAlbumInfo(QMap<int,QString> info);
    void getArtistInfo(QMap<int,QString> info);

protected:

    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

private slots:

    void albumHover();
    void orderChanged();
    void filterAlbum(QModelIndex index);

signals:

    void albumDoubleClicked(QMap<int,QString> info);
    void albumOrderChanged(QString order);
    void playAlbum(QMap<int,QString> info);
    void babeAlbum_clicked( QMap<int,QString> info);
    void populateFinished();
    void expandTo(QString artist, QString album);
    void createdAlbum(Album *album);
};

#endif // ALBUMSVIEW_H
