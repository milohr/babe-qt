#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QObject>
#include <QListWidget>
#include <QEvent>
#include <QScrollBar>
#include <QAction>
#include <QGraphicsDropShadowEffect>

#include "../utils/bae.h"

class BabeAlbum;

class BabeGrid : public QListWidget
{
    Q_OBJECT

public:
    explicit BabeGrid(const BAE::ALBUM_FACTOR &factor, const BAE::AlbumSizeHint &deafultValue, const uint8_t &albumRadius = 4, QWidget *parent= nullptr);
    ~BabeGrid() override;

    void addAlbum(const BAE::DB &albumMap);
    void flushGrid();
    void setAlbumsSize(const uint &value);
    void setAlbumsSpacing(const uint &space);
    void showLabels(const bool &state);

    QHash<BAE::DB,BabeAlbum*> albumsMap;
    QList<QListWidgetItem*> itemsList;

    bool autoAdjust = true;
    bool albumShadows = true;
    bool hiddenLabels = false;
    uint albumSize;

private:
    QAction *order;
    double albumFactor;
    BAE::AlbumSizeHint defaultAlbumValue;
    bool ascending=true;
    uint albumSpacing = 25;
    uint8_t albumRadius = 4;

    void adjustGrid();
    void setUpActions();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

signals:
    void albumClicked(const BAE::DB &albumMap);
    void albumDoubleClicked(const BAE::DB &albumMap);
    void playAlbum(const BAE::DB &albumMap);
    void babeAlbum(const BAE::DB &albumMap);
    void dragAlbum();
    void albumReady();
};

inline uint qHash(const BAE::DB &key, uint seed)
{
    return qHash(key[BAE::KEY::ALBUM]+" "+key[BAE::KEY::ARTIST], seed) ^ static_cast<uint>(key[BAE::KEY::ARTIST].length());
}

#endif // GRIDVIEW_H
