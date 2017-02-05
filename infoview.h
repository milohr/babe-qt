#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QWidget>
#include <album.h>
#include <QToolButton>

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

    QToolButton *hideBtn;
    bool hide= true;

private slots:
    void setAlbumArt(QByteArray array);
    void setAlbumInfo(QString info);
    void setArtistArt(QByteArray array);
    void setLyrics(QString lyrics);
    void setArtistInfo(QString info);
    void hideArtistInfo();
    void playAlbum_clicked(QString artist, QString album);

signals:
    void playAlbum(QString artist, QString album);

};

#endif // INFOVIEW_H
