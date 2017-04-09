#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "babetable.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QStringList info, QWidget *parent = 0);
    ~Form();

private slots:
    void on_pushButton_2_clicked();



    void on_changebtn_clicked();

private:
    Ui::Form *ui;
    QString track;
    QString title;
    QString artist;
    QString album;
    QString genre;

signals:
    void infoModified(QMap<int, QString> map);

};

#endif // FORM_H
