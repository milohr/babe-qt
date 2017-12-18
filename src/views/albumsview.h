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
#include <QMetaType>
#include <QToolButton>
//#include <QGraphicsBlurEffect>
//#include <QGraphicsScene>
//#include <QGraphicsPixmapItem>

#include "../utils/albumloader.h"
#include "../utils/bae.h"

class BabeAlbum;
class BabeTable;
class BabeGrid;

namespace Ui { class AlbumsView; }

class AlbumsView : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumsView(const bool &extraList=false, QWidget *parent = nullptr);
    ~AlbumsView();

    void populate(const QString &query);
    void populate(const BAE::DB_LIST &albums);

    void addAlbum(const BAE::DB &albumMap);
    void populateExtraList(const QStringList &albums);
    void flushView();
    void hide_all(bool state);

    void filter(const BAE::DB_LIST &filter, const BAE::KEY &type);

    BabeTable *albumTable;
    BabeGrid *grid;
    QToolButton *expandBtn;

private:
    AlbumLoader albumLoader;
    bool extraList = false;

    QFrame *albumBox_frame;
    BabeAlbum *cover;
    QListWidget *artistList;
    QToolButton *closeBtn;


public slots:
    void hideAlbumFrame();
    void expandList();
    void showAlbumInfo(const BAE::DB &albumMap);

private slots:
    void albumHover();
    void filterAlbum(QModelIndex index);

signals:
    void albumDoubleClicked(const BAE::DB info);
    void albumOrderChanged(QString order);
    void playAlbum(const BAE::DB &info);
    void babeAlbum(const BAE::DB info);
    void populateFinished();
    void expandTo(const QString &artist);
    void createdAlbum(BabeAlbum *album);
};

#endif // ALBUMSVIEW_H
