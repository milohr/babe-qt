#pragma once

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
    void populateTableView(QSqlQuery query);
    void populateTableViewHeads(QSqlQuery query);
    void populateExtraList(QSqlQuery query);
    void flushGrid();
    int getAlbumSize() { return this->albumSize; }
    QSlider *slider;
    QComboBox *order;
    QFrame *utilsFrame;
    BabeTable *albumTable;
    QListWidget *grid;


    enum ALBUMSVIEW_H{ TITLE, ARTIST, ART };

private:

    int gridSize;
    int albumSize=120;
    bool extraList=false;
    QList<Album*> albumsList;
    QList<QListWidgetItem*> itemsList;
    QStringList albums;
    QStringList artists;
    QWidget *albumBox_frame;
    QFrame *line_h;
    Album *cover;
    CollectionDB *connection;
    Playlist *playlist;
    QListWidget *artistList;
    QToolButton *closeBtn;

public slots:

    void hideAlbumFrame();
    void changedArt_cover(QMap<int,QString> info);
    void changedArt_head(QMap<int,QString> info);
    void babeAlbum(QMap<int,QString> info);
    void setAlbumsSize(int value);


private slots:

    void getAlbumInfo(QMap<int,QString> info);
    void getArtistInfo(QMap<int,QString> info);
    void albumTable_clicked(QStringList list);
    void albumTable_rated(QStringList list);
    void albumTable_babeIt(QStringList list);
    void albumHover();    
    void orderChanged(QString order);
    void filterAlbum(QModelIndex index);

signals:

    void albumDoubleClicked(QMap<int,QString> info);
    void songClicked(QStringList url);
    void songRated(QStringList url);
    void songBabeIt(QStringList url);
    void albumOrderChanged(QString order);
    void playAlbum(QMap<int,QString> info);
    void babeAlbum_clicked(QMap<int,QString> info);
    void populateCoversFinished();
    void populateHeadsFinished();

protected:
    virtual bool eventFilter(QObject * watched, QEvent * event) Q_DECL_OVERRIDE;


};

