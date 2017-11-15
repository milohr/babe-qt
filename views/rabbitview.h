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
#include "gridview.h"

#include "collectionDB.h"
#include "album.h"
#include "babetable.h"

class RabbitView : public QWidget
{
    Q_OBJECT

public:

    explicit RabbitView(QWidget *parent = nullptr);

    enum suggestionsTables
    {
        SIMILAR,GENERAL,ALL
    };

    void flushSuggestions(suggestionsTables = ALL);
    BabeTable * getTable() { return this->generalSuggestion; }


private:

    uint ALBUM_SIZE_MEDIUM = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::AlbumSizeHint::MEDIUM_ALBUM);

    GridView *artistSuggestion;
    BabeTable *generalSuggestion;

public slots:

    void populateArtistSuggestion(QMap<QString, QByteArray> info);
    void populateGeneralSuggestion(const Bae::DB_LIST &mapList);

    void filterByArtist(const Bae::DB &albumMap);

signals:
    void playAlbum(const Bae::DB &info);


};

#endif // RABITVIEW_H
