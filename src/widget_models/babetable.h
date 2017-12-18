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
#include <QThread>
#include <QObject>

#include "../utils/bae.h"
#include "../utils/trackloader.h"

class BabeAlbum;
class Notify;

namespace BABETABLE
{
    enum ACTION
    {
        BABEIT = 0,
        QUEUEIT = 1,
        INFOIT = 2,
        EDITIT = 3,
        SAVETO = 4,
        REMOVEIT = 5,
        RATEIT = 6,
        MOODIT = 7,
        SENDTO = 8,
        ADDTO = 9
    };
}

class BabeTable : public QTableWidget
{
    Q_OBJECT

private:
    TrackLoader trackLoader;
    QMediaPlayer *preview;
    int previewRow=-1;

    int rRow=0;
    int rColumn=0;
    bool rowColoring=false;
    bool rowDragging=false;
    bool rowPreview=true;

    QMenu *sendToMenu;

    QMap<QString,QString> devices;
    QButtonGroup *stars;

    QWidget *addMusicMsgWidget;
    QLabel *addMusicTxt;
    QLabel *addMusicImg;
    QString addMusicMsg = "oops...\n";
    QString addMusicIcon= "face-sleeping";

public:    
    explicit BabeTable(QWidget *parent = nullptr);
    ~BabeTable();

    void insertTrack(const BAE::DB &track);
    void populateTableView(const BAE::DB_LIST &mapList);
    void populateTableView(const QString &indication);
    void removeMissing(const QString &url);
    void setRating(const int &rate);
    void setTableOrder(int column, BAE::W order);
    void setVisibleColumn(const BAE::KEY &column);
    void addRow(const BAE::DB &map);
    void addRowAt(const int &row, const BAE::DB &map);
    void passStyle(QString style);
    void setAddMusicMsg(const QString &msg, const QString &icon= "face-sleeping");
    int getIndex();
    QTableWidgetItem *getItem(const int &row, const BAE::KEY &column);
    void putItem(const int &row, const BAE::KEY &col, QTableWidgetItem *item);
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

    void addMenuItem(QAction *item);

    BAE::DB getRowData(const int &row);
    QMap<QString, QString> getKdeConnectDevices();
    QStringList getTableColumnContent(const BAE::KEY &column);
    BAE::DB_LIST getAllTableContent();

    QMenu *contextMenu;

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
    void itemEdited(const BAE::DB &map);
    void flushTable();
    void colorizeRow(const QList<int> &rows, const QString &color, const bool &dark=false);

signals:
    void tableWidget_doubleClicked(const BAE::DB_LIST &mapList);
    void songRated(const QStringList &list);
    void enterTable();
    void leaveTable();
    void finishedPopulating();
    void rightClicked(const int &row, const int &column);
    void babeIt_clicked(const BAE::DB_LIST &tracks);
    void moodIt_clicked(const QList<int> &rows,const QString &color,const bool &dark);
    void queueIt_clicked(const BAE::DB_LIST &track);
    void infoIt_clicked(const BAE::DB &track);
    void indexesMoved(const int &track,const int &newRow);
    void previewStarted();
    void previewFinished();
    void indexRemoved(const int &row);
    void playItNow(const BAE::DB_LIST &tracks);
    void appendIt(const BAE::DB_LIST &tracks);
    void saveToPlaylist(const BAE::DB_LIST &tracks);

};

#endif // BABETABLE_H
