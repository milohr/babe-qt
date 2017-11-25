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
#include <QHash>
#include <QMetaType>
#include <QThread>

//#include <QGraphicsBlurEffect>
//#include <QGraphicsScene>
//#include <QGraphicsPixmapItem>

#include "../widget_models/scrolltext.h"
#include "../widget_models/babealbum.h"
#include "../widget_models/babetable.h"
#include "../db/collectionDB.h"
#include "../widget_models/babegrid.h"
#include "../kde/notify.h"
#include "../utils/albumloader.h"


namespace Ui { class AlbumsView; }

class AlbumsView : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumsView(const bool &extraList=false, QWidget *parent = nullptr);
    ~AlbumsView();

    void populate(QSqlQuery &query);
    void populate(const DB_LIST &albums);

    void addAlbum(const Bae::DB &albumMap);
    void populateExtraList(const QStringList &albums);
    void flushView();
    void hide_all(bool state);

    void filter(const Bae::DB_LIST &filter, const Bae::KEY &type);

    BabeTable *albumTable;
    BabeGrid *grid;
    QToolButton *expandBtn;

private:
    AlbumLoader albumLoader;
    bool extraList=false;

    QFrame *albumBox_frame;
    BabeAlbum *cover;
    CollectionDB connection;
    QListWidget *artistList;
    QToolButton *closeBtn;


public slots:
    void hideAlbumFrame();
    void expandList();
    void showAlbumInfo(const Bae::DB &albumMap);

private slots:
    void albumHover();
    void filterAlbum(QModelIndex index);

signals:
    void albumDoubleClicked(const Bae::DB info);
    void albumOrderChanged(QString order);
    void playAlbum(const Bae::DB &info);
    void babeAlbum(const Bae::DB info);
    void populateFinished();
    void expandTo(const QString &artist);
    void createdAlbum(BabeAlbum *album);
};

#endif // ALBUMSVIEW_H
