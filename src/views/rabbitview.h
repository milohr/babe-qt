#ifndef RABITVIEW_H
#define RABITVIEW_H

#include "../utils/bae.h"
#include "../utils/albumloader.h"

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

    void seed (const BAE::DB &track);
    void flushSuggestions(suggestionsTables = ALL);
    BabeGrid *artistSuggestion;
    BabeTable *generalSuggestion;
    BabeTable *filterList;
    void addArtistSuggestion(const BAE::DB &albumMap);

private:
    uint ALBUM_SIZE_MEDIUM = BAE::getWidgetSizeHint(BAE::MEDIUM_ALBUM_FACTOR, BAE::AlbumSizeHint::MEDIUM_ALBUM);
    AlbumLoader albumLoader;

};

#endif // RABITVIEW_H
