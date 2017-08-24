#ifndef BABETABLE_H
#define BABETABLE_H

#include <QWidget>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidgetAction>
#include <QToolButton>
#include <QMouseEvent>
#include <QEvent>
#include <QMap>
#include <QMenu>
#include <QFileInfo>
#include <QVector>
#include <QButtonGroup>
#include <QFontDatabase>
#include <QFont>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QProcess>
#include <QLabel>
#include <QTimer>
#include <QGraphicsEffect>
#include <QGraphicsOpacityEffect>
#include <QItemSelectionModel>
#include <QtMultimedia/QMediaPlayer>

#include "settings.h"
#include "notify.h"
#include "album.h"
#include "collectionDB.h"
#include "metadataForm.h"


namespace Ui { class BabeTable; }

class BabeTable : public QTableWidget
{
    Q_OBJECT

public:

    explicit BabeTable(QWidget *parent = 0);
    ~BabeTable();

    enum order
    {
        DESCENDING, ASCENDING
    };

    enum menuActions //this order must be followed
    {
        BABEIT, QUEUEIT, INFOIT, EDITIT, SAVETO, REMOVEIT, RATEIT, MOODIT, SENDIT, ADDTO
    };


    //    const QMap<int, QString> columnsNames{{TRACK, "track"}, {TITLE, "title"}, {ARTIST, "artist"},{ALBUM,"album"},{GENRE,"genre"},{LOCATION,"location"},{STARS,"stars"},{BABE,"babe"},{ART,"art"},{PLAYED,"played"},{PLAYLIST,"playlist"}};
    QStringList colors = BaeUtils::MoodColors;
    void populateTableView(const BaeUtils::TRACKMAP_LIST &mapList, const bool &descriptiveTitle);
    void populateTableView(const QString &indication, const bool &descriptiveTitle);
    void removeMissing(QStringList missingFiles);
    void setRating(int rate);
    void setTableOrder(int column, int order);
    void setVisibleColumn(int column);
    void addRow(const BaeUtils::TRACKMAP &map, const bool &descriptiveTooltip);
    void addRowAt(const int &row, const BaeUtils::TRACKMAP &map, const bool &descriptiveTooltip);
    void passStyle(QString style);
    void setAddMusicMsg(const QString &msg, const QString &icon);
    int getIndex();
    void enablePreview(const bool state);
    void startPreview(const QString &url);
    void stopPreview();
    QString getHearts(const int &value);
    QString getStars(const int &value);
    void removeRepeated();
    QList<int> getSelectedRows(const bool &onRightClick=true);
    void enableRowColoring(const bool &state=false);
    void enableRowDragging(const bool &state=false);
    //void removeRow(int row);

    BaeUtils::TRACKMAP getRowData(const int &row);
    QMap<QString, QString> getKdeConnectDevices();
    QStringList getTableColumnContent(const BaeUtils::TracksCols &column);
    BaeUtils::TRACKMAP_LIST getAllTableContent();


protected:

    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void dropEvent(QDropEvent *event);
    //  virtual void mouseReleaseEvent(QMouseEvent* evt);
    virtual void mousePressEvent(QMouseEvent* evt);
    virtual void keyPressEvent(QKeyEvent *event);

private slots:

    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void rateGroup(const int &id, const bool &rightClick=true);
    void setUpContextMenu(const int row, const int column);
    void addToPlaylist();
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
    void itemEdited(BaeUtils::TRACKMAP map);
    void flushTable();
    void colorizeRow(const QList<int> &rows, const QString &color, const bool &dark=false);

private:

    CollectionDB connection;

    QMediaPlayer *preview;
    int previewRow=-1;
    Notify nof;
    QToolButton *fav1;
    QToolButton *fav2;
    QToolButton *fav3;
    QToolButton *fav4;
    QToolButton *fav5;

    int rRow=0;
    int rColumn=0;
    bool rowColoring=false;
    bool rowDragging=false;
    bool rowPreview=true;
    QMenu *contextMenu;
    QMenu* sendToMenu;
    QMenu* moodMenu;
    QMap<QString,QString> devices;
    QTimer *updater = new QTimer(this);

    QWidget *addMusicMsgWidget;
    QLabel *addMusicTxt;
    QLabel *addMusicImg;
    QString addMusicMsg = "oops...\n";
    QString addMusicIcon= "face-sleeping";

signals:

    void tableWidget_doubleClicked(BaeUtils::TRACKMAP_LIST mapList);
    void songRated(QStringList list);
    void enterTable();
    void leaveTable();
    void finishedPopulating();
    void rightClicked(const int row, const int column);
    void babeIt_clicked(BaeUtils::TRACKMAP_LIST tracks);
    void removeIt_clicked(int index);
    void moodIt_clicked(QList<int> rows, QString color, bool dark);
    void queueIt_clicked(BaeUtils::TRACKMAP_LIST track);
    void infoIt_clicked(const BaeUtils::TRACKMAP &track);
    void indexesMoved(int track,int newRow);
    void previewStarted();
    void previewFinished();
    void indexRemoved(int row);
    void playItNow(BaeUtils::TRACKMAP_LIST tracks);
    void appendIt(BaeUtils::TRACKMAP_LIST tracks);
    void saveToPlaylist(BaeUtils::TRACKMAP_LIST tracks);

};

#endif // BABETABLE_H
