#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QObject>
#include <QListWidget>
#include <QEvent>
#include <QScrollBar>
#include <QAction>
#include <QGraphicsDropShadowEffect>

#include "babealbum.h"
#include "../utils/bae.h"

class BabeGrid : public QListWidget
{
    Q_OBJECT

public:
    explicit BabeGrid(const Bae::ALBUM_FACTOR &factor, const Bae::AlbumSizeHint &deafultValue, const uint8_t &albumRadius = 4, QWidget *parent= nullptr);
    ~BabeGrid() override;
    void addAlbum(const Bae::DB &albumMap);
    void flushGrid();
    void setAlbumsSize(const uint &value);
    void setAlbumsSpacing(const uint &space);
    void showLabels(const bool &state);
    QHash<Bae::DB,BabeAlbum*> albumsMap;
    QList<QListWidgetItem*> itemsList;

    bool autoAdjust = true;
    bool albumShadows = true;
    bool hiddenLabels = false;
    uint albumSize;

private:
    QAction *order;

    double albumFactor;
    Bae::AlbumSizeHint defaultAlbumValue;
    bool ascending=true;
    uint albumSpacing = 25;
    uint8_t albumRadius = 4;

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
