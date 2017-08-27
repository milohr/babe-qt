#ifndef YOUTUBE_H
#define YOUTUBE_H
#include <QObject>
#include <QWidget>
#include <QProcess>
#include <QByteArray>
#include <QMovie>
#include <QDebug>
#include <QDirIterator>

#include <fstream>
#include <iostream>

#include "baeUtils.h"
#include "notify.h"
#include "taginfo.h"


class YouTube : public QObject
{
    Q_OBJECT


public:

    enum metadata
    {
        TITLE,ARTIST,ALBUM,COMMENT,ID
    };

    explicit YouTube(QObject *parent = 0);
    void fetch(QStringList files);
    ~YouTube();
    void searchPendingFiles();
    QStringList ids;
    QStringList urls;


private slots:

    void processFinished();
    void processFinished_totally(const int &state,const QMap<int,QString> &info,const QProcess::ExitStatus &exitStatus);

private:

    Notify nof;
    QMap<QString,QString> tracks;
    const  QString ydl="youtube-dl -f m4a --youtube-skip-dash-manifest -o \"$$$.%(ext)s\"";
    const QString cachePath=Bae::YoutubeCachePath;
    const QString extensionFetchingPath = Bae::ExtensionFetchingPath;

signals:
    void  youtubeTrackReady(bool state);
};

#endif // YOUTUBE_H
