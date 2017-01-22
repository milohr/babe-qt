#ifndef ALBUM_H
#define ALBUM_H
#include <QLabel>
#include <QString>
#include <scrolltext.h>

class Album : public QLabel
{
    Q_OBJECT
public:
    explicit Album(QLabel *parent = 0);
   // explicit Album(QLabel *parent = 0, QString artist, QString album);

    void setCoverArt(QString path);
    QString getTitle();
    QString getArtist();
    QString getAlbum();
    void setArtist(QString artist);
    void setAlbum(QString album);
    void setTitle(QString artist, QString album);

private:
    QString artist="";
    QString album="";
    ScrollText *title;
    QWidget *widget;
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
    virtual void QContextMenuEvent();
};

#endif // ALBUM_H
