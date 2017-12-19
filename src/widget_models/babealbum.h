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

#include "../utils/bae.h"


class ScrollText;
class CollectionDB;

using namespace BAE;

class BabeAlbum : public QLabel
{
    Q_OBJECT

public:
    explicit BabeAlbum(const DB &info, const AlbumSizeHint &widgetSize, const uint8_t &widgetRadius=0, const bool &isDraggable=false,QWidget *parent = nullptr);
    ~BabeAlbum() override;

    void setUpMenu();

    void setTitle(const QString &artistTitle = "", const QString &albumTitle = "");
    void setTitleGeometry(const int &x, const int &y, const int &w, const int &h);
    void setSize(const uint &value);

    void saturatePixmap(const int &value);
    void restoreSaturation();

    uint getSize();
    uint size;
    double subSize;
    bool borderColor = false;
    bool visibleTitle = true;

    QWidget *widget;
    QString getTitle();
    QString getArtist();
    QString getAlbum();
    DB getAlbumMap();

    void showTitle(const bool &state);
    bool showPlayBtn = true;

private:
    QMenu *contextMenu;
CollectionDB *connection;
    DB albumMap;
    bool draggable;

    QString imagePath;
    QString artist = "";
    QString album = "";

    ScrollText *title;
    QPoint oldPos;
    QPoint startPos;
    QToolButton *playBtn;

    uint border_radius=0;

    void performDrag();

signals:
    void albumCoverClicked(const DB & albumMap);
    void albumCoverDoubleClicked(const DB &albumMap);

    void playAlbum(const DB &albumMap);
    void changedArt(const DB &albumMap);
    void babeAlbum(const DB &albumMap);

    void albumDragged();
    void albumDragStarted();

    void albumCoverEnter();
    void albumCoverLeave();

public slots:
    void putPixmap(const QString &path);
    void putDefaultPixmap();
    void babeIt_action();
    void artIt_action();

protected:
    virtual void mousePressEvent (QMouseEvent * event) Q_DECL_OVERRIDE;
    virtual void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject * watched, QEvent * event) Q_DECL_OVERRIDE;
};

#endif // ALBUM_H
