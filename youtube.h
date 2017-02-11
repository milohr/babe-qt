#ifndef YOUTUBE_H
#define YOUTUBE_H

#include <QWidget>
#include <QStandardPaths>
namespace Ui {
class YouTube;
}

class YouTube : public QWidget
{
    Q_OBJECT

public:
    explicit YouTube(QWidget *parent = 0);
    ~YouTube();

private slots:
    void on_goBtn_clicked();
    void processFinished();

private:
    Ui::YouTube *ui;
    const QString ydl="youtube-dl --metadata-from-title \"%(artist)s - %(title)s\"  --format m4a  --add-metadata";
    const QString cachePath=QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)+"/babe/youtube";
};

#endif // YOUTUBE_H
