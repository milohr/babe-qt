#ifndef ALBUM_H
#define ALBUM_H
#include <QLabel>
#include <QString>
#include <scrolltext.h>
#include <QPixmap>

class Album : public QLabel
{
    Q_OBJECT
public:

    explicit Album(QString imagePath,int widgetSize, int widgetRadius=0, bool isDraggable=false, QWidget *parent = 0);

    void setCoverArt(QString path);
    void setArtist(QString artist);
    void setAlbum(QString album);
    void setTitle();
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
    QPixmap image;
    QPixmap getPixmap();

private:

    bool draggable;

    QString artist="";
    QString album="";
    ScrollText *title;

    QPoint oldPos;


signals:
    void albumCoverClicked(QStringList info);
    void albumCoverEnter();
     void albumCoverLeft();

public slots:

protected:

    virtual void mousePressEvent ( QMouseEvent * evt);
    virtual void mouseReleaseEvent ( QMouseEvent * evt);
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

    virtual void paintEvent(QPaintEvent *e);
    // virtual void  mouseMoveEvent(QMouseEvent *evt);
};

#endif // ALBUM_H
