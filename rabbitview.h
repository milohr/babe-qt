#ifndef RABITVIEW_H
#define RABITVIEW_H

#include <QMap>
#include <QWidget>
#include <QListWidget>

#include "album.h"
#include "babetable.h"

class QFrame;
class QListWidgetItem;
class QSplitter;
class QString;

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
    void populateGeneralSuggestion(const QList<QMap<int, QString>> &mapList);
    void filterByArtist(const QMap<int, QString> &albumMap);

signals:
    void playAlbum(const QMap<int, QString> &info);

private:
    BabeTable *generalSuggestion;
    QListWidget *artistSuggestion;
};

#endif // RABITVIEW_H
