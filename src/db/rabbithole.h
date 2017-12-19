#ifndef RABBITHOLE_H
#define RABBITHOLE_H

#endif // RABBITHOLE_H
#include "../utils/bae.h"

using namespace BAE;

namespace Query
{
typedef QMap<TABLE, QMap<W, QString>> Q;

static const Q QUERY =
{
    {
        TABLE::TRACKS,
        {
            {W::SIMILAR, "SELECT DISTINCT t.* FROM tracks t "
             "INNER JOIN tracks_tags tg ON t.url = tg.url "
             "WHERE tg.tag COLLATE NOCASE IN "

             "(SELECT tag FROM tracks_tags "
             "WHERE url = '?' "
             "AND (context = 'track_team' or context = 'tag')) "

             "AND ((tg.tag COLLATE NOCASE IN "
             "(SELECT tag FROM albums_tags "
             "WHERE album IN "
             "(SELECT album FROM tracks "
             "WHERE url = '?') "
             "AND artist IN "
             "(SELECT artist FROM tracks WHERE url = '?') ) ) "

             "OR (tg.tag COLLATE NOCASE IN "
             "(SELECT tag FROM artists_tags "
             "WHERE artist IN "
             "(SELECT artist FROM tracks "
             "WHERE url = '?') ) ))"
            },
            {W::TAG, "SELECT * FROM TRACKS WHERE GENRE = '?' " }
        }
    },
    {
        TABLE::ARTISTS,
        {
            {W::SIMILAR, "SELECT DISTINCT a.* "
             "FROM artists_tags at "
             "INNER JOIN artists a ON a.artist = at.artist  "
             "WHERE at.artist COLLATE NOCASE "

             "IN (SELECT at.tag FROM artists_tags at "
             "INNER JOIN tracks t ON t.url = '?' "
             "AND t.artist = at.artist "
             "AND (at.context = 'artist_team' "
             "OR at.context = 'artist_similar' "
             "OR at.context = 'tag') ) "

//             "OR at.tag COLLATE NOCASE "
//             "IN (select at.tag "
//             "FROM artists_tags at "
//             "INNER join tracks t ON t.url = \"?\" "
//             "AND t.artist = at.artist AND  at.context = 'tag') "
//             "ORDER BY at.context "
             "LIMIT 20;"
            },
        }
    }
};



}
