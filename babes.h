#ifndef BABES_H
#define BABES_H

#include <QWidget>

namespace Ui {
class babes;
}

class babes : public QWidget
{
    Q_OBJECT

public:
    explicit babes(QWidget *parent = 0);
    ~babes();

private:
    Ui::babes *ui;
};

#endif // BABES_H
