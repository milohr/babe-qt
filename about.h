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

    void labelClicked();

    void on_pushButton_clicked();

    void on_codeBtn_clicked();


private:
    Ui::About *ui;
    const QString url="https://github.com/milohr/babe-qt";
};

#endif // ABOUT_H
