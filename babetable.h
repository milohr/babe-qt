#ifndef BABETABLE_H
#define BABETABLE_H

#include <QButtonGroup>
#include <QEvent>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QGraphicsEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMap>
#include <QMenu>
#include <QMouseEvent>
#include <QProcess>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QToolButton>
#include <QWidget>
#include <QWidgetAction>
#include <QVector>

#include "album.h"
#include "collectionDB.h"
#include "metadataForm.h"
#include "notify.h"
#include "settings.h"

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

    CollectionDB *connection = new CollectionDB;

    const QMap<int, QString> columnsNames{{TRACK, "track"}, {TITLE, "title"}, {ARTIST, "artist"}, {ALBUM, "album"}, {GENRE, "genre"}, {LOCATION, "location"}, {STARS, "stars"}, {BABE, "babe"}, {ART, "art"}, {PLAYED, "played"}, {PLAYLIST, "playlist"}};
    QStringList playlistsMenus;
    QStringList colors = BaeUtils::getMoodColors();

    void populateTableView(const QList<QMap<int, QString> > &mapList, bool descriptiveTitle, bool coloring);
    void populateTableView(const QString &indication, bool descriptiveTitle, bool coloring);
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
    void update();

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
    QTimer *updater = new QTimer(this);
    QString addMusicMsg = "oops... :(\nnothing here";

signals:
    void tableWidget_doubleClicked(const QList<QMap<int, QString>> &mapList);
    void songRated(QStringList list);
    void enteredTable();
    void leftTable();
    void finishedPopulating();
    void rightClicked(const int row, const int column);
    void babeIt_clicked(const QMap<int, QString> &track);
    void removeIt_clicked(int index);
    void createPlaylist_clicked();
    void refreshPlaylistsMenu(QStringList list);
    void moodIt_clicked(const int &row, const QString &color);
    void queueIt_clicked(const QMap<int, QString> &track);
    void infoIt_clicked(const QString &title, const QString &artist, const QString &album);
};

#endif // BABETABLE_H
