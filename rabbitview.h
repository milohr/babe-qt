#ifndef RABITVIEW_H
#define RABITVIEW_H

#include <QDebug>
#include <QFrame>
#include <QListWidget>
#include <QListWidgetItem>
#include <QObject>
#include <QSplitter>
#include <QString>
#include <QStringList>

#include "album.h"
#include "babetable.h"
#include "collectionDB.h"

class RabbitView : public QWidget
{
    Q_OBJECT
public:
    explicit RabbitView(QWidget *parent = 0);   

    enum suggestionsTables
    {
        SIMILAR, GENERAL, ALL
    };

    BabeTable* getTable();
    void flushSuggestions(suggestionsTables = ALL);

public slots:
    void populateArtistSuggestion(const QMap<QString, QByteArray> &info);
    void populateGeneralSuggestion(const QList<QMap<int, QString> > &mapList);
    void filterByArtist(const QMap<int, QString> &albumMap);

signals:
    void playAlbum(const QMap<int,QString> &info);

private:
    CollectionDB *connection;
    BabeTable *generalSuggestion;
    QListWidget *artistSuggestion;
};

#endif // RABITVIEW_H
