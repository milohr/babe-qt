#ifndef BABETABLE_H
#define BABETABLE_H

#include <QMap>
#include <QTableWidget>
#include <QVector>

#include "album.h"
#include "collectionDB.h"
#include "metadataForm.h"
#include "notify.h"
#include "settings.h"

class QButtonGroup;
class QEvent;
class QFileInfo;
class QFont;
class QFontDatabase;
class QGraphicsEffect;
class QGraphicsOpacityEffect;
class QHBoxLayout;
class QHeaderView;
class QLabel;
class QMenu;
class QMouseEvent;
class QProcess;
class QStringList;
class QTableWidgetItem;
class QTimer;
class QToolButton;
class QWidget;
class QWidgetAction;

namespace Ui {
class BabeTable;
}

class BabeTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit BabeTable(QWidget *parent = 0);
    ~BabeTable();

    enum columns
    {
        TRACK, TITLE, ARTIST, ALBUM, GENRE, LOCATION, STARS, BABE, ART, PLAYED, PLAYLIST, columnsCOUNT
    };
    enum order
    {
        DESCENDING, ASCENDING
    };
    enum menuActions //this order must be followed
    {
        BABEIT, QUEUEIT, INFOIT, EDITIT, SAVETO, REMOVEIT, RATEIT, MOODIT, SENDIT, ADDTO
    };

    QStringList playlistsMenus;
    QStringList colors = BaeUtils::getMoodColors();
    const QMap<int, QString> columnsNames{
        {TRACK, "track"},
        {TITLE, "title"},
        {ARTIST, "artist"},
        {ALBUM, "album"},
        {GENRE, "genre"},
        {LOCATION, "location"},
        {STARS, "stars"},
        {BABE, "babe"},
        {ART, "art"},
        {PLAYED, "played"},
        {PLAYLIST, "playlist"}
    };

    void populateTableView(const QList<QMap<int, QString>> &mapList, bool descriptiveTitle, bool coloring);
    void populateTableView(const QVariantList &contentList, bool descriptiveTitle, bool coloring);
    void removeMissing(const QStringList &missingFiles);
    void setRating(int rate);
    void setTableOrder(int column, int order);
    void setVisibleColumn(int column);
    void addRow(const QMap<int, QString> &map, bool descriptiveTooltip, bool coloring);
    void addRowAt(int row, const QMap<int, QString> &map, bool descriptiveTooltip, bool coloring);
    void passStyle(const QString &style);
    void passPlaylists();
    void populatePlaylist(const QStringList &urls, const QString &playlist);
    void setAddMusicMsg(const QString &msg);
    int getIndex();
    void removeRepeated();

    QMap<int, QString> getRowData(int row);
    QMap<QString, QString> getKdeConnectDevices();
    QStringList getTableColumnContent(const columns &column);
    QList<QMap<int, QString>> getAllTableContent();
    QString rowInfo(int column, int rowNumber = -1);

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void mousePressEvent(QMouseEvent* evt);
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void rateGroup(int id);
    void setUpContextMenu(const int row, const int column);
    void addToPlaylist(QAction* action);
    void babeIt_action();
    void sendIt_action(QAction *device);
    void infoIt_action();
    void editIt_action();
    void removeIt_action();
    void moodIt_action(const QString &color);
    void queueIt_action();
    void moodTrack(int color);
    void updateMusicMsg();

public slots:
    QStringList getPlaylistMenus();
    void itemEdited(const QMap<int, QString> &map);
    void flushTable();
    void colorizeRow(const int &row, const QString &color);

private:
    Notify nof;
    QToolButton *fav1;
    QToolButton *fav2;
    QToolButton *fav3;
    QToolButton *fav4;
    QToolButton *fav5;

    int rRow = 0;
    int rColumn = 0;
    QMenu *contextMenu;
    QMenu* moodMenu;
    QMenu* sendToMenu;
    QMenu* playlistsMenu;
    QMap<QString, QString> devices;
    QLabel *addMusicTxt;
    QString addMusicMsg = "oops... :(\nnothing here";
    CollectionDB m_collectionDB;

signals:
    void tableWidget_doubleClicked(const QList<QMap<int, QString>> &mapList);
    void songRated(const QStringList &list);
    void enteredTable();
    void leftTable();
    void finishedPopulating();
    void rightClicked(const int &row, const int &column);
    void babeIt_clicked(const QMap<int, QString> &track);
    void removeIt_clicked(int index);
    void createPlaylist_clicked();
    void refreshPlaylistsMenu(QStringList list);
    void moodIt_clicked(const int &row, const QString &color);
    void queueIt_clicked(const QMap<int, QString> &track);
    void infoIt_clicked(const QString &title, const QString &artist, const QString &album);
};

#endif // BABETABLE_H
