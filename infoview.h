#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QDebug>
#include <QHBoxLayout>
#include <QToolButton>
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

    QMap<int, QString> track;

private:

    Ui::InfoView *ui;
    int ALBUM_SIZE_BIG = BaeUtils::getWidgetSizeHint(BaeUtils::BIG_ALBUM_FACTOR,BaeUtils::BIG_ALBUM);
    int ALBUM_SIZE_MEDIUM = BaeUtils::getWidgetSizeHint(BaeUtils::MEDIUM_ALBUM_FACTOR,BaeUtils::MEDIUM_ALBUM);
    Pulpo info;
    QToolButton *hideBtn;
    bool hide= false;
    bool customsearch = false;

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
    void getTrackInfo(const  QMap<int, QString> &song, const bool &album, const bool &artist, const bool &lyrics, const bool &tags);
    void on_searchBtn_clicked();

signals:

    void playAlbum(QMap<int,QString> info);
    void tagClicked(QString tag);
    void similarArtistTagClicked(QString tag);
    void similarBtnClicked(QStringList similar);
    void tagsBtnClicked(QStringList similar);

    void artistSimilarReady(const QMap<QString,QByteArray> &info);
    void albumTagsReady(const QStringList &tags);
    void lyricsReady(const QString &lyrics);
    void albumWikiReady(const QString &wiki);
    void artistWikiReady(const QString &wiki);

private slots:

    void on_toolButton_clicked();
    void on_tagsInfo_anchorClicked(const QUrl &arg1);
    void on_similarArtistInfo_anchorClicked(const QUrl &arg1);
};

#endif // INFOVIEW_H
