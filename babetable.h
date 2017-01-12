#ifndef BABETABLE_H
#define BABETABLE_H

#include <QWidget>
#include <QStringList>
#include <collectionDB.h>

namespace Ui {
class BabeTable;
}

class BabeTable : public QWidget
{
    Q_OBJECT

public:
    explicit BabeTable(QWidget *parent = 0);
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

private slots:

    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void rateGroup(int id);
    void setUpContextMenu();

    void uninstallAppletClickedSlot();


private:
    Ui::BabeTable *ui;
    CollectionDB *connection;

signals:
    void tableWidget_doubleClicked(QStringList url);
    void songRated(QStringList list);

};

#endif // BABETABLE_H
