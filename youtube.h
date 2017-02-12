#ifndef YOUTUBE_H
#define YOUTUBE_H

#include <QWidget>
#include <QStandardPaths>
#include <QProcess>
#include<QByteArray>
#include <QMovie>

namespace Ui {
class YouTube;
}

class YouTube : public QWidget
{
    Q_OBJECT

public:
    explicit YouTube(QWidget *parent = 0);
    void fetch(QStringList ids);
    ~YouTube();

private slots:
    void on_goBtn_clicked();
    void processFinished();
    void processFinished_totally(int state);

private:
    Ui::YouTube *ui;
    QProcess *process;
    QMovie *movie;
    const QString ydl="youtube-dl --metadata-from-title \"%(artist)s - %(title)s\"  --format m4a  --add-metadata";
    const QString cachePath=QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)+"/babe/youtube";

signals:
   void  youtubeTrackReady(bool state);
};

#endif // YOUTUBE_H
