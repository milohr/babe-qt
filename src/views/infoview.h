#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QDebug>
#include <QHBoxLayout>
#include <QToolButton>
#include <QWebEngineView>
#include "../widget_models/babealbum.h"
#include "../pulpo/pulpo.h"

namespace Ui { class InfoView; }

class InfoView : public QWidget
{
    Q_OBJECT

public:
    explicit InfoView(QWidget *parent = nullptr);
    ~InfoView();

    BabeAlbum *artist;
    QWidget *infoUtils;

    Bae::DB track;

    void setTrack(const Bae::DB &track);

private:

    Ui::InfoView *ui;
    uint ALBUM_SIZE_BIG = Bae::getWidgetSizeHint(Bae::BIG_ALBUM_FACTOR,Bae::AlbumSizeHint::BIG_ALBUM);
    uint ALBUM_SIZE_MEDIUM = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::AlbumSizeHint::MEDIUM_ALBUM);
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
    void setArtistTags(const QStringList &tags);
    void setAlbumTags(const QStringList &tags);
    QStringList getTags();
    QStringList getSimilarArtistTags();
    void hideArtistInfo();
    void getTrackInfo(const Bae::DB &track);
    void on_searchBtn_clicked();

signals:

    void playAlbum(const Bae::DB &info);
    void tagClicked(QString tag);
    void similarArtistTagClicked(QString tag);
    void similarBtnClicked(QStringList similar);
    void tagsBtnClicked(QStringList similar);

    void artistSimilarReady(const QMap<QString,QByteArray> &info,const Bae::DB &track);
    void albumTagsReady(const QStringList &tags,const Bae::DB &track);
    void lyricsReady(const QString &lyrics,const Bae::DB &track);
    void albumWikiReady(const QString &wiki,const Bae::DB &track);
    void artistWikiReady(const QString &wiki,const Bae::DB &track);

private slots:

    void on_toolButton_clicked();
    void on_tagsInfo_anchorClicked(const QUrl &arg1);
    void on_similarArtistInfo_anchorClicked(const QUrl &arg1);
};

#endif // INFOVIEW_H
