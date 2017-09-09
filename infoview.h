#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QDebug>
#include <QHBoxLayout>
#include <QToolButton>
#include <QWebEngineView>
#include "album.h"
#include "pulpo/pulpo.h"

namespace Ui {
class InfoView;
}

class InfoView : public QWidget
{
    Q_OBJECT


public:
    explicit InfoView(QWidget *parent = 0);
    ~InfoView();

    Album *artist;
    QWidget *infoUtils;

   Bae::TRACKMAP track;

    void setTrack(const Bae::TRACKMAP &track);
private:

    Ui::InfoView *ui;
    int ALBUM_SIZE_BIG = Bae::getWidgetSizeHint(Bae::BIG_ALBUM_FACTOR,Bae::BIG_ALBUM);
    int ALBUM_SIZE_MEDIUM = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::MEDIUM_ALBUM);
    QToolButton *hideBtn;
    bool hide= false;
    bool customsearch = false;
    QWebEngineView *page;

public slots:

    void clearInfoViews();
    void setAlbumArt(QByteArray array);
    void setAlbumInfo(QString info);
    void setArtistArt(const QByteArray &array);
    void setArtistArt(const QString &url);
    void setLyrics(const QString &lyrics);
    void setArtistInfo(const QString &info);
    void setArtistTagInfo(const QStringList &tags);
    void setTagsInfo(QStringList);
    QStringList getTags();
    QStringList getSimilarArtistTags();
    void hideArtistInfo();
    void getTrackInfo(const bool &album, const bool &artist, const bool &lyrics, const bool &tags);
    void on_searchBtn_clicked();

signals:

    void playAlbum(QMap<int,QString> info);
    void tagClicked(QString tag);
    void similarArtistTagClicked(QString tag);
    void similarBtnClicked(QStringList similar);
    void tagsBtnClicked(QStringList similar);

    void artistSimilarReady(const QMap<QString,QByteArray> &info,const Bae::TRACKMAP &track);
    void albumTagsReady(const QStringList &tags,const Bae::TRACKMAP &track);
    void lyricsReady(const QString &lyrics,const Bae::TRACKMAP &track);
    void albumWikiReady(const QString &wiki,const Bae::TRACKMAP &track);
    void artistWikiReady(const QString &wiki,const Bae::TRACKMAP &track);

private slots:

    void on_toolButton_clicked();
    void on_tagsInfo_anchorClicked(const QUrl &arg1);
    void on_similarArtistInfo_anchorClicked(const QUrl &arg1);
};

#endif // INFOVIEW_H
