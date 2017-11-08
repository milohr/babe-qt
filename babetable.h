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


class TrackLoader : public QObject
{
    Q_OBJECT

public:
    TrackLoader() : QObject()
    {
        qRegisterMetaType<Bae::DB>("Bae::DB");
        qRegisterMetaType<Qt::SortOrder>("Qt::SortOrder");
        qRegisterMetaType<QVector<int>>("<QVector<int>");
        qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
        qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
        moveToThread(&t);
        t.start();
    }

    ~TrackLoader()
    {
        go=false;
        t.quit();
        t.wait();
    }

    void requestTracks(QString query)
    {
        QMetaObject::invokeMethod(this, "getTracks", Q_ARG(QString, query));
    }

public slots:
    void getTracks(QString query)
    {
        qDebug()<<"GETTING TRACKS FROM BABETABLE";

        QSqlQuery mquery(query);
        auto tracks = this->connection.getTrackData(mquery);
        if(tracks.size()>0)
        {
            for(auto trackMap : tracks)
            {   if(go)
                {
                    emit trackReady(trackMap);
                }else break;
            }
        }
        emit finished();
    }

signals:
    void trackReady(Bae::DB &trackMap);
    void finished();

private:
    QThread t;
    CollectionDB connection;
    bool go=true;
};


class BabeTable : public QTableWidget
{
    Q_OBJECT

private:

    TrackLoader trackLoader;
    CollectionDB connection;

    QMediaPlayer *preview;
    int previewRow=-1;
    Notify *nof;

    int rRow=0;
    int rColumn=0;
    bool rowColoring=false;
    bool rowDragging=false;
    bool rowPreview=true;

    QMenu *contextMenu;
    QMenu* sendToMenu;

    QMap<QString,QString> devices;
    QButtonGroup *stars;

    QWidget *addMusicMsgWidget;
    QLabel *addMusicTxt;
    QLabel *addMusicImg;
    QString addMusicMsg = "oops...\n";
    QString addMusicIcon= "face-sleeping";

     const QStringList colors = Bae::MoodColors;

public:

    explicit BabeTable(QWidget *parent = nullptr);

    enum menuActions //this order must be followed
    {
        BABEIT, QUEUEIT, INFOIT, EDITIT, SAVETO, REMOVEIT, RATEIT, MOODIT, SENDIT, ADDTO
    };

    void insertTrack(const Bae::DB &track);
    void populateTableView(const Bae::DB_LIST &mapList);
    void populateTableView(QSqlQuery &indication);
    void removeMissing(const QString &url);
    void setRating(const int &rate);
    void setTableOrder(int column, Bae::Order order);
    void setVisibleColumn(const Bae::DBCols &column);
    void addRow(const Bae::DB &map);
    void addRowAt(const int &row, const Bae::DB &map);
    void passStyle(QString style);
    void setAddMusicMsg(const QString &msg, const QString &icon= "face-sleeping");
    int getIndex();
    QTableWidgetItem *getItem(const int &row, const Bae::DBCols &column);
    void putItem(const int &row, const Bae::DBCols &col, QTableWidgetItem *item);
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

    Bae::DB getRowData(const int &row);
    QMap<QString, QString> getKdeConnectDevices();
    QStringList getTableColumnContent(const Bae::DBCols &column);
    Bae::DB_LIST getAllTableContent();



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
    void itemEdited(const Bae::DB &map);
    void flushTable();
    void colorizeRow(const QList<int> &rows, const QString &color, const bool &dark=false);

signals:

    void tableWidget_doubleClicked(const Bae::DB_LIST &mapList);
    void songRated(const QStringList &list);
    void enterTable();
    void leaveTable();
    void finishedPopulating();
    void rightClicked(const int &row, const int &column);
    void babeIt_clicked(const Bae::DB_LIST &tracks);
    void removeIt_clicked(const int &index);
    void moodIt_clicked(const QList<int> &rows,const QString &color,const bool &dark);
    void queueIt_clicked(const Bae::DB_LIST &track);
    void infoIt_clicked(const Bae::DB &track);
    void indexesMoved(const int &track,const int &newRow);
    void previewStarted();
    void previewFinished();
    void indexRemoved(const int &row);
    void playItNow(const Bae::DB_LIST &tracks);
    void appendIt(const Bae::DB_LIST &tracks);
    void saveToPlaylist(const Bae::DB_LIST &tracks);

};

#endif // BABETABLE_H
