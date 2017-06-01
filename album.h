#ifndef ALBUM_H
#define ALBUM_H

#include <QLabel>
#include <QMap>
#include <QObject>
#include <QPixmap>

#include "baeUtils.h"
#include "scrolltext.h"

class QEvent;
class QWidget;
class ScrollText;
class QToolButton;

class Album : public QLabel
{
    Q_OBJECT
public:
    explicit Album(const QString &imagePath, int widgetSize, int widgetRadius = 0, bool isDraggable = false, QWidget *parent = 0);
    ~Album();
    void setArtist(const QString &artist);
    void setAlbum(const QString &album);
    void setTitle(const QString &artist, const QString &album = "");
    void setBGcolor(const QString &bgColor);
    void setBordercolor(bool color);
    void titleVisible(bool state);
    void setSize(int value);
    void setTitleGeometry(int x, int y, int w, int h);

    QString getTitle();
    QString getArtist();
    QString getAlbum();
    QString getBGColor();
    QPixmap getPixmap();

    QWidget *widget;
    QToolButton *playBtn;

    enum albumField
    {
        ALBUM, ARTIST, ART
    };

private:
    int m_size;
    int m_borderRadius;
    bool m_draggable;
    bool m_borderColor;
    QPixmap image;
    QPoint m_startPos;
    QString m_artist;
    QString m_album;
    QString m_bgColor;
    QString m_imagePath;
    ScrollText *m_title;
    QMap<int, QString> m_albumMap;

    void performDrag();

signals:
    void albumDragged();
    void albumCoverEnter();
    void albumCoverLeft();
    void albumCoverClicked(const QMap<int, QString> &albumMap);
    void albumCoverDoubleClicked(const QMap<int, QString> &albumMap);
    void babeAlbum_clicked(const QMap<int, QString> &albumMap);
    void changedArt(const QMap<int, QString> &albumMap);
    void playAlbum(const QMap<int, QString> &albumMap);

public slots:
    void putPixmap(const QByteArray &pix);
    void putPixmap(const QString &path);
    void putDefaultPixmap();
    void playBtn_clicked();
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
};

#endif // ALBUM_H
