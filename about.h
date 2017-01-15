#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = 0);
    ~About();

private slots:
    void on_about_linkActivated(const QString &link);
    void labelClicked();

private:
    Ui::About *ui;
};

#endif // ABOUT_H
