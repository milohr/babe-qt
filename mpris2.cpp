
#include "mpris2.h"
#include<QDBusConnection>
#include <QDBusMessage>


static inline qlonglong convertTime(int t)
{
    return t*1000000;
}

static QString mprisPath;

Mpris::Mpris(QObject *p)
    : QObject(p)
    , pos(-1)
{
    QDBusConnection::sessionBus().registerService("org.mpris.MediaPlayer2.Babe");


    QDBusConnection::sessionBus().registerObject("/org/mpris/MediaPlayer2", this, QDBusConnection::ExportAdaptors);
    /*connect(this, SIGNAL(setRandom(bool)), MPDConnection::self(), SLOT(setRandom(bool)));
    connect(this, SIGNAL(setRepeat(bool)), MPDConnection::self(), SLOT(setRepeat(bool)));
    connect(this, SIGNAL(setSeekId(qint32, quint32)), MPDConnection::self(), SLOT(setSeekId(qint32, quint32)));
    connect(this, SIGNAL(setVolume(int)), MPDConnection::self(), SLOT(setVolume(int)));*/

    //    connect(MPDConnection::self(), SIGNAL(currentSongUpdated(const Song &)), this, SLOT(updateCurrentSong(const Song &)));
    //connect(MPDStatus::self(), SIGNAL(updated()), this, SLOT(updateStatus()));
    /* if (mprisPath.isEmpty()) {
        mprisPath=QLatin1String(CANTATA_REV_URL);
        mprisPath.replace(".", "/");
        mprisPath="/"+mprisPath+"/Track/%1";
    }*/
    //connect(CurrentCover::self(), SIGNAL(coverFile(const QString &)), this, SLOT(updateCurrentCover(const QString &)));
}

Mpris::~Mpris()
{
    QDBusConnection::sessionBus().unregisterService("org.mpris.MediaPlayer2.cantata");
}

void Mpris::Pause()
{
    qDebug()<<"pause";

}

void Mpris::Play()
{
    qDebug()<<"play";
}

QString Mpris::PlaybackStatus() const
{
    qDebug()<<"PlaybackStatus";
    return "PlaybackStatus";
}

qlonglong Mpris::Position() const
{
    // Cant use MPDStatus, as we dont poll for track position, but use a timer instead!
    //return MPDStatus::self()->timeElapsed();
    return 1000000000;
}

void Mpris::updateStatus()
{
    QVariantMap map;
    qDebug()<<"updateStatus";


}

void Mpris::updateCurrentCover(const QString &fileName)
{
    if (fileName!=currentCover) {
        currentCover=fileName;
        signalUpdate("Metadata", Metadata());
    }
}

void Mpris::updateCurrentSong()
{
    qDebug()<<"updateCurrentSong";

}

QVariantMap Mpris::Metadata() const {
    QVariantMap metadataMap;

    return metadataMap;
}

void Mpris::Raise()
{
    emit showMainWindow();
}

void Mpris::signalUpdate(const QString &property, const QVariant &value)
{
    QVariantMap map;
    map.insert(property, value);
    signalUpdate(map);
}

void Mpris::signalUpdate(const QVariantMap &map)
{
    if (map.isEmpty()) {
        return;
    }
    QDBusMessage signal = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
                                                     "org.freedesktop.DBus.Properties",
                                                     "PropertiesChanged");
    QVariantList args = QVariantList()
            << "org.mpris.MediaPlayer2.Player"
            << map
            << QStringList();
    signal.setArguments(args);
    QDBusConnection::sessionBus().send(signal);
}
