#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include<QString>
#include<QStringList>
#include "collectionDB.h"
#include<QDebug>
#include <QThread>
#include "playlist.h"
namespace Ui {
class settings;
}




class settings : public QWidget
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = 0);
    ~settings();
    bool checkCollection();
    CollectionDB &getCollectionDB();
    int getToolbarIconSize()  {return iconSize;}
    QString getCollectionPath() {return collectionPath;}
    void setSettings(QStringList setting);
    void readSettings();
    enum iconSizes
    {
        s16,s22,s24
    };
private slots:

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolbarIconSize_activated(const QString &arg1);
void finishedAddingTracks(bool state);
void on_pushButton_clicked();

public slots:

void populateDB(QString path);

private:
    Ui::settings *ui;
    const std::string settingPath="../player/settings.conf";
    int iconSize = 16;
    QString collectionPath="";
    CollectionDB collection_db;
Playlist collection;
 QThread* thread;
signals:
    void toolbarIconSizeChanged(int newSize);
    void collectionPathChanged(QString newPath);
    void collectionDBFinishedAdding(bool state);


};





#endif // SETTINGS_H
