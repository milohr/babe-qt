#ifndef ALBUM_H
#define ALBUM_H
#include <QLabel>
#include <QString>
#include <QPixmap>
#include <QToolButton>
#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QFileDialog>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include "collectionDB.h"
#include "baeUtils.h"
#include "scrolltext.h"


class Album : public QLabel
{
    Q_OBJECT

public:


    explicit Album(QWidget *parent = 0);
    ~Album(){}
    void createAlbum(const Bae::DB &info, const Bae::ALbumSizeHint &widgetSize, const uint &widgetRadius=0, const bool &isDraggable=false);
    void setArtist(const QString &artistTitle);
    void setAlbum(const QString &albumTitle);
    void setTitle(const QString &artistTitle, const QString &albumTitle="");
    void setBGcolor(const QString &bgColor);
    void setUpMenu();
    void setTitleGeometry(const int &x, const int &y, const int &w, const int &h);
    void setSize(const int &value);
    void saturatePixmap(const int &value);
    void restoreSaturation();
    int getSize();
    int size;
    double subSize;
    bool borderColor=false;
    QWidget *widget;
    QString getTitle();
    QString getArtist();
    QString getAlbum();
    QString getBGcolor();
    QPixmap image;
    QPixmap getPixmap();
    QPixmap unsaturated;

    void showTitle(const bool &state);
    bool showPlayBtn=true;
    enum albumField
    {
        ALBUM, ARTIST, ART
    };

private:

    Bae::DB  albumMap;
    bool draggable;
    bool visibleTitle=true;
    QString imagePath;
    QString artist="";
    QString album="";
    QString bgColor="";
    ScrollText *title;
    QColor borderQColor;
    QPoint oldPos;
    QPoint startPos;
    QToolButton *playBtn;

    int border_radius=0;

    CollectionDB connection;

    void performDrag();

signals:

    void albumCoverClicked(const Bae::DB & albumMap);
    void albumCoverDoubleClicked(const Bae::DB &albumMap);
    void playAlbum(const Bae::DB &albumMap);
    void changedArt(const Bae::DB &albumMap);
    void babeAlbum_clicked(const Bae::DB &albumMap);
    void albumDragged();
    void albumDragStarted();
    void albumCoverEnter();
    void albumCoverLeave();
    void albumCreated(Album *album);

public slots:

    void putPixmap(const QByteArray &pix);
    void putPixmap(const QPixmap &pix);
    void putPixmap(const QString &path);
    void putDefaultPixmap();
    void babeIt_action();
    void artIt_action();
    void removeIt_action();

protected:

    virtual void mousePressEvent (QMouseEvent * event) Q_DECL_OVERRIDE;
    virtual void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject * watched, QEvent * event) Q_DECL_OVERRIDE;



    // virtual void  mouseMoveEvent(QMouseEvent *evt);
};

#endif // ALBUM_H
