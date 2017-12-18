#ifndef METADATAFORM_H
#define METADATAFORM_H

#include <QWidget>
#include <QMap>

#include "../utils/bae.h"

namespace Ui {
class metadataForm;
}

class metadataForm : public QWidget
{
    Q_OBJECT

public:
    explicit metadataForm(const BAE::DB &info, QWidget *parent = nullptr);
    ~metadataForm();

private slots:
    void on_pushButton_2_clicked();
    void on_changebtn_clicked();

private:
    Ui::metadataForm *ui;
    BAE::DB trackMap;

signals:
    void infoModified(const BAE::DB &map);

};

#endif // METADATAFORM_H
