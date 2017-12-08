#ifndef RABITVIEW_H
#define RABITVIEW_H

#include "../utils/bae.h"
#include "../utils/albumloader.h"
#include "../db/collectionDB.h"

class BabeGrid;
class BabeAlbum;
class BabeTable;
class Pulpo;

class QListWidget;
class QObject;
class QListWidgetItem;
class QStringList;
class QFrame;
class QSplitter;


class RabbitView : public QWidget
{
    Q_OBJECT

public:
    explicit RabbitView(QWidget *parent = nullptr);
    ~RabbitView();

    enum suggestionsTables
    {
        SIMILAR,
        GENERAL,
        ALL
    };

    void seed (const Bae::DB &track);
    void flushSuggestions(suggestionsTables = ALL);
    BabeGrid *artistSuggestion;
    BabeTable *generalSuggestion;
    BabeTable *filterList;
    void addArtistSuggestion(const Bae::DB &albumMap);

private:
    uint ALBUM_SIZE_MEDIUM = Bae::getWidgetSizeHint(Bae::MEDIUM_ALBUM_FACTOR, Bae::AlbumSizeHint::MEDIUM_ALBUM);
    CollectionDB connection;
    AlbumLoader albumLoader;

};

#endif // RABITVIEW_H
