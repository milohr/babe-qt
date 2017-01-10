#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include<QString>
#include<QStringList>

namespace Ui {
class settings;
}

class settings : public QWidget
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = 0);
    ~settings();
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

public slots:


private:
    Ui::settings *ui;
    const std::string settingPath="../player/settings.conf";
    int iconSize = 16;
    QString collectionPath="";



signals:
    void toolbarIconSizeChanged(int newSize);
    void collectionPathChanged(QString newPath);

};

#endif // SETTINGS_H
