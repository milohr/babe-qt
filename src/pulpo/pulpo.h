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
#include "enums.h"

using namespace PULPO;

class Pulpo : public QObject
{
    Q_OBJECT

public:
    explicit Pulpo(const BAE::DB &song, QObject *parent = nullptr);
    explicit Pulpo(QObject *parent = nullptr);
    ~Pulpo();

    void feed(const BAE::DB &song, const PULPO::RECURSIVE &recursive = PULPO::RECURSIVE::ON );
    void registerServices(const QList<PULPO::SERVICES> &services);
    void setInfo(const PULPO::INFO &info);
    void setOntology(const PULPO::ONTOLOGY &ontology);
    PULPO::ONTOLOGY getOntology();
    void setRecursive(const PULPO::RECURSIVE &state);

private:
    bool initServices();
    PULPO::RECURSIVE recursive = PULPO::RECURSIVE::ON;
    QList<SERVICES> registeredServices = {SERVICES::ALL};

    void passSignal(const BAE::DB &track, const PULPO::RESPONSE &response);

protected:
    QByteArray array;
    BAE::DB track;
    PULPO::INFO info = INFO::NONE;
    PULPO::ONTOLOGY ontology = ONTOLOGY::NONE;
    PULPO::AVAILABLE availableInfo;

    PULPO::RESPONSE packResponse(const PULPO::ONTOLOGY ontology, const PULPO::INFO &infoKey, const PULPO::CONTEXT &contextName, const QVariant &value);
    PULPO::RESPONSE packResponse(const PULPO::ONTOLOGY ontology, const PULPO::INFO &infoKey, const PULPO::VALUE &map);

    QByteArray startConnection(const QString &url, const QMap<QString, QString> &headers = {});
    bool parseArray();

    /* expected methods to be overrided by services */
    bool setUpService(const PULPO::ONTOLOGY &ontology, const PULPO::INFO &info);
    virtual bool parseArtist() {return false;}
    virtual bool parseAlbum() {return false;}
    virtual bool parseTrack() {return false;}

signals:
    void infoReady(BAE::DB track, PULPO::RESPONSE response);
    void serviceFail(const QString &message);
};

#endif // ARTWORK_H
