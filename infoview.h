#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QWidget>
#include <album.h>
namespace Ui {
class InfoView;
}

class InfoView : public QWidget
{
    Q_OBJECT


public:
    explicit InfoView(QWidget *parent = 0);
    ~InfoView();

    void setAlbumArt(QByteArray array);
void setAlbumInfo(QString info);


private:
    Ui::InfoView *ui;
    Album *album;
    Album *artist;

private slots:
    void setArtistArt(QByteArray array);
void setLyrics(QString lyrics);
     void setArtistInfo(QString info);
};

#endif // INFOVIEW_H
