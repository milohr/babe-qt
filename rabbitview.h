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

    enum suggestionsTables
    {
        SIMILAR,GENERAL,ALL
    };

    void flushSuggestions(suggestionsTables = ALL);
    BabeTable * getTable() { return this->generalSuggestion; }


private:

    int ALBUM_SIZE_MEDIUM = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::MEDIUM_ALBUM);
    int ALBUM_SIZE_SMALL = Bae::getWidgetSizeHint(Bae::SMALL_ALBUM_FACTOR,Bae::SMALL_ALBUM);

    QListWidget *artistSuggestion;
    BabeTable *generalSuggestion;

public slots:

    void populateArtistSuggestion(QMap<QString, QByteArray> info);
    void populateGeneralSuggestion(const Bae::TRACKMAP_LIST &mapList);

    void filterByArtist(QMap<int, QString> albumMap);

signals:
    void playAlbum(QMap<int,QString> info);


};

#endif // RABITVIEW_H
