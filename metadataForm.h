#ifndef METADATAFORM_H
#define METADATAFORM_H

#include <QWidget>
#include <QMap>

#include "babetable.h"

namespace Ui {
class metadataForm;
}

class metadataForm : public QWidget
{
    Q_OBJECT

public:
    explicit metadataForm(QMap<int, QString> info, QWidget *parent = 0);
    ~metadataForm();

private slots:
    void on_pushButton_2_clicked();
    void on_changebtn_clicked();

private:
    Ui::metadataForm *ui;
    QString track;
    QString title;
    QString artist;
    QString album;
    QString genre;

signals:
    void infoModified(QMap<int, QString> map);

};

#endif // FORM_H
