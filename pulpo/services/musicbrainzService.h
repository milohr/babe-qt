#ifndef MUSICBRAINZSERVICE_H
#define MUSICBRAINZSERVICE_H
#include <QObject>
#include <QString>
#include "../pulpo.h"


class musicbrainzService : public Pulpo
{
    Q_OBJECT

private:
    QString API = "http://musicbrainz.org/ws/2/recording/?query=artist:lana%20del%20rey";
public:
    explicit musicbrainzService();

signals:

public slots:
};

#endif // MUSICBRAINZSERVICE_H
