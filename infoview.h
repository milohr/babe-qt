#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QWidget>
#include "album.h"
#include "pulpo/pulpo.h"
#include "pulpo/lyrics.h"

class QHBoxLayout;
class QToolButton;

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
    bool hide = false;
    bool m_customSearch = false;

public slots:
    void clearInfoViews();
    void setAlbumArt(const QByteArray &array);
    void setAlbumInfo(const QString &info);
    void setArtistArt(const QByteArray &array);
    void setArtistArt(const QString &url);
    void setLyrics(const QString &lyrics);
    void setArtistInfo(const QString &info);
    void setArtistTagInfo(const QStringList &tags);
    void setTagsInfo(QStringList);
    void hideArtistInfo();
    void playAlbum_clicked(const QMap<int, QString> &info);
    void getTrackInfo(const QString &title_, const QString &artist_, const QString &album_);
    void on_searchBtn_clicked();

signals:
    void playAlbum(const QMap<int, QString> &info);
    void similarArtistTagClicked(const QString &tag);
    void similarBtnClicked(const QStringList &similar);
    void tagsBtnClicked(const QStringList &similar);
    void tagClicked(const QString &tag);

private slots:
    void on_toolButton_clicked();
    void on_tagsInfo_anchorClicked(const QUrl &arg1);
    void on_similarArtistInfo_anchorClicked(const QUrl &arg1);
};

#endif // INFOVIEW_H
