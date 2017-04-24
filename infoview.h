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
    void setArtistArt(QString url);
    void setLyrics(QString lyrics);
    void setArtistInfo(QString info);
    void setArtistTagInfo(QStringList tags);
    void setTagsInfo(QStringList);
    void hideArtistInfo();
    void playAlbum_clicked(QMap<int,QString> info);
    void getTrackInfo(QString _title, QString _artist, QString _album);
    void getTrackArt(QString _artist, QString _album);
    void on_searchBtn_clicked();

signals:

    void playAlbum(QMap<int,QString> info);
    void tagClicked(QString tag);
    void similarArtistTagClicked(QString tag);
    void similarBtnClicked(QStringList similar);
    void tagsBtnClicked(QStringList similar);


private slots:

    void on_toolButton_clicked();
    void on_tagsInfo_anchorClicked(const QUrl &arg1);
    void on_similarArtistInfo_anchorClicked(const QUrl &arg1);
};

#endif // INFOVIEW_H
