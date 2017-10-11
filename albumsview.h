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
//#include <QGraphicsBlurEffect>
//#include <QGraphicsScene>
//#include <QGraphicsPixmapItem>

#include "scrolltext.h"
#include "album.h"
#include "babetable.h"
#include "collectionDB.h"
#include "gridview.h"
#include "notify.h"



namespace Ui {
class AlbumsView;
}

class AlbumLoader : public QObject
{
    Q_OBJECT

public:

    AlbumLoader() : QObject()
    {
        qRegisterMetaType<Bae::DB>("Bae::DB");
        qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");

        moveToThread(&t);
        t.start();
    }

    ~AlbumLoader()
    {
        go=false;
        t.quit();
        t.wait();
    }

    void requestAlbums(Bae::DBTables type, QString query)
    {
        if(type==Bae::DBTables::ALBUMS)
            QMetaObject::invokeMethod(this, "getAlbums", Q_ARG(QString, query));
        else if(type==Bae::DBTables::ARTISTS)
            QMetaObject::invokeMethod(this, "getArtists", Q_ARG(QString, query));
    }

    void next() { this->nextAlbum = false;  }

public slots:

    void getAlbums(QString query)
    {
        qDebug()<<"GETTING TRACKS FROM ALBUMSVIEW";

        QSqlQuery mquery(query);
        auto albums = this->connection.getAlbumData(mquery);
        if(albums.size()>0)
        {
            for(auto albumMap : albums)
            {   if(go)
                {
                    emit albumReady(albumMap);
                    while(this->nextAlbum && go){t.msleep(100);}
                    this->nextAlbum=!this->nextAlbum;

                }else break;
            }
        }
        t.msleep(100);
        emit finished();
    }

    void getArtists(QString query)
    {
        QSqlQuery mquery(query);
        auto artists = this->connection.getArtistData(mquery);
        if(artists.size()>0)
        {
            for(auto albumMap : artists)
            {
                if(go)
                {
                    emit albumReady(albumMap);
                    while(this->nextAlbum && go){t.msleep(100); }
                    this->nextAlbum=true;
                }else break;
            }
        }
        t.msleep(100);
        emit finished();
    }

signals:
    void albumReady(const Bae::DB &albumMap);
    void finished();

private:
    QThread t;
    CollectionDB connection;
    bool go=true;
    bool nextAlbum=true;
};

class AlbumsView : public QWidget
{
    Q_OBJECT

public:

    explicit AlbumsView(bool extraList=false, QWidget *parent = nullptr);

    void populateAlbumsView(const Bae::DBTables &type, QSqlQuery &query);
    void addAlbum(const Bae::DB &albumMap);
    void populateExtraList(const QStringList &albums);
    void flushView();
    int getAlbumSize() { return this->albumSize; }
    void hide_all(bool state);

    void filter(const Bae::DB_LIST &filter, const Bae::DBCols &type);
    QSlider *slider;

    QFrame *utilsFrame;
    BabeTable *albumTable;
    GridView *grid;
    QToolButton *expandBtn;


private:

    AlbumLoader albumLoader;
    int albumSize;
    bool extraList=false;

    QWidget *albumBox_frame;
    QFrame *line_h;
    Album *cover;
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
    void createdAlbum(Album *album);
};





#endif // ALBUMSVIEW_H
