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

#include "../../utils/bae.h"
#include "../../kde/notify.h"
#include "../local/taginfo.h"


class YouTube : public QObject
{
    Q_OBJECT

public:
    explicit YouTube(QObject *parent = nullptr);
    ~YouTube();
    void fetch(const QString &json);
    QStringList ids;

private slots:
    void processFinished();
    void processFinished_totally(const int &state, const BAE::DB &info, const QProcess::ExitStatus &exitStatus);

private:
    const QString ydl="youtube-dl -f m4a --youtube-skip-dash-manifest -o \"$$$.%(ext)s\"";

signals:
    void done();
};

#endif // YOUTUBE_H
