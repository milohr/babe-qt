#ifndef RABITVIEW_H
#define RABITVIEW_H

#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QFrame>
#include <QSplitter>

#include "collectionDB.h"
#include "album.h"
#include "babetable.h"

class RabbitView : public QWidget
{
    Q_OBJECT

public:

    explicit RabbitView(QWidget *parent = 0);
    void flushSuggestions();
    BabeTable * getTable() { return this->generalSuggestion; }

private:

    CollectionDB *connection;

    QListWidget *artistSuggestion;
    BabeTable *generalSuggestion;

public slots:

    void populateArtistSuggestion(QMap<QString, QByteArray> info);
    void populateGeneralSuggestion(QList<QMap<int,QString>> mapList);

signals:

};

#endif // RABITVIEW_H
