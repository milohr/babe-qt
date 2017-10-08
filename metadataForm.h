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
    explicit metadataForm(const Bae::DB &info, QWidget *parent = nullptr);
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
    void infoModified(const Bae::DB &map);

};

#endif // FORM_H
