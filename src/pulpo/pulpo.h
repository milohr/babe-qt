#ifndef PULPO_H
#define PULPO_H

#include <QPixmap>
#include <QList>
#include <QDebug>
#include <QImage>
#include <QtCore>
#include <QtNetwork>
#include <QUrl>
#include <QWidget>
#include <QObject>
#include <QNetworkAccessManager>
#include <QDomDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QVariantMap>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QSqlQuery>

#include "../utils/bae.h"
#include "webengine.h"
#include "enums.h"

using namespace PULPO;

class Pulpo : public QObject
{
    Q_OBJECT

public:
    explicit Pulpo(const Bae::DB &song, QObject *parent = nullptr);
    explicit Pulpo(QObject *parent = nullptr);
    ~Pulpo();

    void feed(const Bae::DB &song, const RECURSIVE &recursive = RECURSIVE::ON );
    void registerServices(const QList<SERVICES> &services);
    void setInfo(const INFO &info);
    void setOntology(const ONTOLOGY &ontology);
    void setRecursive(const RECURSIVE &state);

private:
    bool initServices();
    RECURSIVE recursive = RECURSIVE::ON;
    QList<SERVICES> registeredServices = {SERVICES::ALL};

    void passSignal(const Bae::DB &track, const PULPO::RESPONSE &response);

protected:
    QByteArray array;
    Bae::DB track;
    INFO info = INFO::NONE;
    ONTOLOGY ontology = ONTOLOGY::NONE;
    AVAILABLE availableInfo;

    RESPONSE packResponse(const INFO &infoKey, const CONTEXT &contextName, const QVariant &value);
    RESPONSE packResponse(const PULPO::INFO &infoKey, const CONTEXT_K &map);

    QByteArray startConnection(const QString &url, const QMap<QString, QString> &headers = {});
    bool parseArray();

    /* expected methods to be overrided by services */
    bool setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info);
    virtual bool parseArtist() {return false;}
    virtual bool parseAlbum() {return false;}
    virtual bool parseTrack() {return false;}

public slots:    
    void saveArt(const QByteArray &array, const QString &path);

signals:
    void infoReady(const Bae::DB &track, const PULPO::RESPONSE &response);
    void artSaved(const Bae::DB &track);
    void serviceFail(const QString &message);
};

#endif // ARTWORK_H
