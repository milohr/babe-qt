#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QDebug>
#include <QHBoxLayout>
#include <QToolButton>
#include "album.h"
#include "lyrics.h"
#include "artwork.h"

namespace Ui {
class InfoView;
}

class InfoView : public QWidget
{
    Q_OBJECT


public:
    explicit InfoView(QWidget *parent = 0);
    ~InfoView();

    Album *album;
    Album *artist;

    QWidget *infoUtils;


private:
    Ui::InfoView *ui;
Lyrics *lyrics;
    QToolButton *hideBtn;
    bool hide= false;
    bool customsearch = false;

public slots:
    void setAlbumArt(QByteArray array);
    void setAlbumInfo(QString info);
    void setArtistArt(QByteArray array);
    void setLyrics(QString lyrics);
    void setArtistInfo(QString info);
    void hideArtistInfo();
    void playAlbum_clicked(QString artist, QString album);

    void getTrackInfo(QString title, QString artist, QString album);
    void on_searchBtn_clicked();

signals:
    void playAlbum(QString artist, QString album);

private slots:
    void on_toolButton_clicked();
};

#endif // INFOVIEW_H
