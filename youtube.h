#ifndef YOUTUBE_H
#define YOUTUBE_H

#include <QByteArray>
#include <QDebug>
#include <QDirIterator>
#include <QMovie>
#include <QObject>
#include <QProcess>
#include <QWidget>

#include "baeUtils.h"

class YouTube : public QObject
{
    Q_OBJECT
public:
    explicit YouTube(QObject *parent = 0);
    ~YouTube();
    void fetch(QStringList ids_, QStringList urls_ = {});
    void searchPendingFiles();
    QStringList ids;
    QStringList urls;

private slots:
    void processFinished();
    void processFinished_totally(int state, QString id, QProcess::ExitStatus exitStatus);

private:
    const QString ydl = "youtube-dl --metadata-from-title \"%(artist)s - %(title)s\"  --format m4a  --add-metadata";
    const QString cachePath = BaeUtils::getYoutubeCachePath();
    const QString extensionFetchingPath = BaeUtils::getExtensionFetchingPath();

signals:
   void  youtubeTrackReady(bool state);
};

#endif // YOUTUBE_H
