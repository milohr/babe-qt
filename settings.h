#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

namespace Ui {
class settings;
}

class settings : public QWidget
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = 0);
    ~settings();
    int toolbarIconSize()  {return iconSize;}

private slots:
    void on_comboBox_activated(const QString &arg1);


public slots:


private:
    Ui::settings *ui;
    int iconSize = 16;

signals:
    void toolbarIconSizeChanged(int newSize);

};

#endif // SETTINGS_H
