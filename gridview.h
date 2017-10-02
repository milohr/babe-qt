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

    explicit GridView(QWidget *parent= nullptr);
    void addAlbum(const Bae::DB &albumMap);
    void flushGrid();
    void setAlbumsSize(const int &value);
    QHash<Bae::DB,Album*> albumsMap;
    QList<QListWidgetItem*> itemsList;

private:

    QAction *order;
    int albumSize;
    int albumSpacing = 25;
    bool ascending=true;
    bool hiddenLabels=false;

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
    return qHash(key[Bae::DBCols::ALBUM]+" "+key[Bae::DBCols::ARTIST], seed) ^ key[Bae::DBCols::ARTIST].length();
}

#endif // GRIDVIEW_H
