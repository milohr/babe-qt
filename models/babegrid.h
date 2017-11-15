#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QObject>
#include <QListWidget>
#include <QEvent>
#include <QScrollBar>
#include <QAction>
#include <QGraphicsDropShadowEffect>
#include <album.h>

#include "baeUtils.h"

class GridView : public QListWidget
{
    Q_OBJECT

public:

    explicit GridView(const double &factor, const Bae::AlbumSizeHint &deafultValue, QWidget *parent= nullptr);
    void addAlbum(const Bae::DB &albumMap);
    void flushGrid();
    void setAlbumsSize(const uint &value);
    void setAlbumsSpacing(const uint &space);
    QHash<Bae::DB,Album*> albumsMap;
    QList<QListWidgetItem*> itemsList;


private:

    QAction *order;
    uint albumSize;
    double albumFactor;
    Bae::AlbumSizeHint defaultAlbumValue;
    bool ascending=true;
    bool hiddenLabels=false;
    uint albumSpacing = 25;

    void adjustGrid();
    void setUpActions();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

signals:
    void albumClicked(const Bae::DB &albumMap);
    void albumDoubleClicked(const Bae::DB &albumMap);
    void playAlbum(const Bae::DB &albumMap);
    void babeAlbum(const Bae::DB &albumMap);
    void dragAlbum();
    void albumReady();
};

inline uint qHash(const Bae::DB &key, uint seed)
{
    return qHash(key[Bae::KEY::ALBUM]+" "+key[Bae::KEY::ARTIST], seed) ^ key[Bae::KEY::ARTIST].length();
}

#endif // GRIDVIEW_H
