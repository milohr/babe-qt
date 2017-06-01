#ifndef ALBUMSVIEW_H
#define ALBUMSVIEW_H

#include <QWidget>
#include <QComboBox>

#include "album.h"
#include "database/albumsdb.h"
#include "database/tracksdb.h"
#include "database/artistsdb.h"
#include "babetable.h"
#include "collectionDB.h"
#include "scrolltext.h"

class QFrame;
class QGridLayout;
class QHeaderView;
class QListWidgetItem;
class QListWidget;
class QShortcut;
class QSplitter;
class QToolTip;

namespace Ui {
class AlbumsView;
}

class AlbumsView : public QWidget
{
    Q_OBJECT
public:
    explicit AlbumsView(bool extraList = false, QWidget *parent = 0);
    ~AlbumsView();
    void populateTableView(const QString &order = "");
    void populateTableViewHeads();
    void populateExtraList(const QString &artist = "");
    void flushGrid();

    int getAlbumSize();
    QComboBox *order;
    QFrame *utilsFrame;
    QListWidget *grid;
    QSlider *slider;
    BabeTable *albumTable;

    enum ALBUMSVIEW_H {TITLE, ARTIST, ART};

private:
    int gridSize;
    int albumSize = 120;
    bool extraList = false;
    QList<Album*> albumsList;
    QList<QListWidgetItem*> itemsList;

    QStringList albums;
    QStringList artists;

    Album *cover;
    QFrame *line_h;
    Playlist *playlist;
    QToolButton *closeBtn;
    QListWidget *artistList;
    QWidget *albumBox_frame;
    CollectionDB m_collectionDB;
    AlbumsDB *m_albumsDB;
    TracksDB *m_tracksDB;
    ArtistsDB *m_artistsDB;

public slots:
    void hideAlbumFrame();
    void changedArt_cover(const QMap<int, QString> &info);
    void changedArt_head(const QMap<int, QString> &info);
    void babeAlbum(const QMap<int, QString> &info);
    void setAlbumsSize(int value);

private slots:
    void getAlbumInfo(QMap<int, QString> info);
    void getArtistInfo(const QMap<int, QString> &info);
    void albumTable_clicked(const QStringList &list);
    void albumTable_rated(const QStringList &list);
    void albumTable_babeIt(const QStringList &list);
    void albumHover();    
    void orderChanged(const QString &order);
    void filterAlbum(const QModelIndex &index);

signals:
    void populateCoversFinished();
    void populateHeadsFinished();
    void albumDoubleClicked(const QMap<int, QString> &info);
    void albumOrderChanged(const QString &order);
    void babeAlbum_clicked(const QMap<int, QString> &info);
    void playAlbum(const QMap<int, QString> &info);
    void songClicked(const QStringList &url);
    void songRated(const QStringList &url);
    void songBabeIt(const QStringList &url);
};

#endif // ALBUMSVIEW_H
