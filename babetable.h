#ifndef BABETABLE_H
#define BABETABLE_H

#include <QWidget>
#include <QStringList>
#include <collectionDB.h>
#include <QTableWidget>
#include <QToolButton>
#include <QMouseEvent>
#include <QMenu>

namespace Ui {
class BabeTable;
}

class BabeTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit BabeTable(QTableWidget *parent = 0);
    ~BabeTable();

    enum columns
    {
        TRACK,TITLE,ARTIST,ALBUM,GENRE,LOCATION,STARS,BABE,ART,PLAYED,PLAYLIST
    };
    enum order
    {
        DESCENDING, ASCENDING
    };

    void populateTableView(QString indication);
    void setRating(int rate);
    void passCollectionConnection(CollectionDB *con);
    void setTableOrder(int column, int order);
    void setVisibleColumn(int column);
    void addRow(QString title, QString artist, QString album,QString location, QString stars,QString babe);
    void removeRow(int row);
    void flushTable();
    void passStyle(QString style);
    QStringList getTableContent(int column);

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
  //  virtual void mouseReleaseEvent(QMouseEvent* evt);
    virtual void mousePressEvent(QMouseEvent* evt);


private slots:

    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void rateGroup(int id);
    void setUpContextMenu(QPoint pos);

    void babeIt_action();


private:

    CollectionDB *connection;
    QToolButton *fav1;
    QToolButton *fav2;
    QToolButton *fav3;
    QToolButton *fav4;
    QToolButton *fav5;
    int row;
    QMenu *contextMenu;

signals:
    void tableWidget_doubleClicked(QStringList url);
    void songRated(QStringList list);
    void enteredTable();
    void leftTable();
    void finishedPopulating();
    void rightClicked(QPoint evt);
    void babeIt_clicked(QStringList list);

};

#endif // BABETABLE_H
