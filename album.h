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
#include "baeUtils.h"
#include "scrolltext.h"

class Album : public QLabel
{
    Q_OBJECT
public:

    explicit Album(QString imagePath, int widgetSize, int widgetRadius=0, bool isDraggable=false, bool plain=false, QWidget *parent = 0);

    void setArtist(QString artist);
    void setAlbum(QString album);
    void setTitle();
    void setBGcolor(QString bgColor);
    void titleVisible(bool state);
    void setTitleGeometry(int x, int y, int w, int h);
    void setSize(int value);

    int border_radius;
    int size;
    bool borderColor=false;
    QWidget *widget;
    QString getTitle();
    QString getArtist();
    QString getAlbum();
    QString getBGcolor();
    QPixmap image;
    QPixmap getPixmap();
    QToolButton *playBtn;



private:

    bool draggable;
     bool isPlain=false;
QString imagePath;
    QString artist="";
    QString album="";
    QString bgColor="";
    ScrollText *title;

    QPoint oldPos;


signals:
    void albumCoverClicked(QStringList info);
    void albumCoverEnter();
     void albumCoverLeft();
     void playAlbum(QString artist, QString album);
     void changedArt(QString path,QString artist, QString album);

public slots:
      void putPixmap(QByteArray pix);
      void putPixmap(QString path);
      void playBtn_clicked();
      void babeIt_action();
      void artIt_action();
      void removeIt_action();

protected:

    virtual void mousePressEvent ( QMouseEvent * evt);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

    virtual void paintEvent(QPaintEvent *e);

    // virtual void  mouseMoveEvent(QMouseEvent *evt);
};

#endif // ALBUM_H
