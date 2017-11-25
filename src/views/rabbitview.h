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

#include "../widget_models/babegrid.h"
#include "../db/collectionDB.h"
#include "../widget_models/babealbum.h"
#include "../widget_models/babetable.h"
#include "../pulpo/pulpo.h"
#include "../utils/albumloader.h"

class RabbitView : public QWidget
{
    Q_OBJECT

public:
    explicit RabbitView(QWidget *parent = nullptr);
    ~RabbitView();

    enum suggestionsTables
    {
        SIMILAR,GENERAL,ALL
    };

    void seed (const Bae::DB &track);
    void flushSuggestions(suggestionsTables = ALL);
    BabeTable * getTable() { return this->generalSuggestion; }
    BabeGrid *artistSuggestion;
    BabeTable *generalSuggestion;
    void addArtistSuggestion(const Bae::DB &albumMap);

private:
    uint ALBUM_SIZE_MEDIUM = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR,Bae::AlbumSizeHint::MEDIUM_ALBUM);
    CollectionDB connection;
    AlbumLoader albumLoader;

public slots:
    void filterByArtist(const Bae::DB &albumMap);

};

#endif // RABITVIEW_H
