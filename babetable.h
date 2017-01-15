#ifndef BABETABLE_H
#define BABETABLE_H

#include <QWidget>
#include <QStringList>
#include <collectionDB.h>
#include <QTableWidget>
#include <QToolButton>

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
        TITLE,ARTIST,ALBUM,LOCATION,STARS,BABE
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


private slots:

    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void rateGroup(int id);
    void setUpContextMenu();

    void uninstallAppletClickedSlot();


private:

    CollectionDB *connection;
    QToolButton *fav1;
    QToolButton *fav2;
    QToolButton *fav3;
    QToolButton *fav4;
    QToolButton *fav5;

signals:
    void tableWidget_doubleClicked(QStringList url);
    void songRated(QStringList list);
    void enteredTable();
    void leftTable();

};

#endif // BABETABLE_H
