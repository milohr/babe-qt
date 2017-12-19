#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QDebug>
#include <QHBoxLayout>
#include <QToolButton>
#include <QWebEngineView>

#include "../utils/bae.h"

class Pulpo;
class BabeAlbum;
class CollectionDB;

namespace Ui { class InfoView; }

class InfoView : public QWidget
{
    Q_OBJECT

public:
    explicit InfoView(QWidget *parent = nullptr);
    ~InfoView();

    BabeAlbum *artist;
    QWidget *infoUtils;

    BAE::DB track;

    void setTrack(const BAE::DB &track);

private:
    Ui::InfoView *ui;
    uint ALBUM_SIZE_BIG = BAE::getWidgetSizeHint(BAE::BIG_ALBUM_FACTOR, BAE::AlbumSizeHint::BIG_ALBUM);
    uint ALBUM_SIZE_MEDIUM = BAE::getWidgetSizeHint(BAE::MEDIUM_ALBUM_FACTOR, BAE::AlbumSizeHint::MEDIUM_ALBUM);
    QToolButton *hideBtn;
    bool hide= false;
    bool customsearch = false;
    CollectionDB *connection;

public slots:
    void clearInfoViews();
    void setAlbumArt(QByteArray array);
    void setAlbumInfo(const QString &info);
    void setArtistArt(const QByteArray &array);
    void setArtistArt(const QString &url);
    void setLyrics(const QString &lyrics);
    void setArtistInfo(const QString &info);
    void setArtistTags(const QStringList &tags);
    void setAlbumTags(const QStringList &tags);
    QStringList getTags();
    QStringList getSimilarArtistTags();
    void hideArtistInfo();
    void getTrackInfo(const BAE::DB &track);
    void on_searchBtn_clicked();

signals:
    void playAlbum(const BAE::DB &info);
    void tagClicked(QString tag);
    void similarArtistTagClicked(QString tag);
    void similarBtnClicked(QStringList similar);
    void tagsBtnClicked(QStringList similar);

    void artistSimilarReady(const QMap<QString,QByteArray> &info,const BAE::DB &track);
    void albumTagsReady(const QStringList &tags,const BAE::DB &track);
    void lyricsReady(const QString &lyrics,const BAE::DB &track);
    void albumWikiReady(const QString &wiki,const BAE::DB &track);
    void artistWikiReady(const QString &wiki,const BAE::DB &track);

private slots:
    void on_toolButton_clicked();
    void on_tagsInfo_anchorClicked(const QUrl &arg1);
    void on_similarArtistInfo_anchorClicked(const QUrl &arg1);
    void on_save_clicked();
};

#endif // INFOVIEW_H
