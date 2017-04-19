#ifndef ONLINEVIEW_H
#define ONLINEVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>

#include "babetable.h"

namespace Ui {
class OnlineView;
}

class OnlineView : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineView(QWidget *parent = 0);
    ~OnlineView();
    BabeTable *table;

private:
    Ui::OnlineView *ui;
    QLineEdit *searchField;

};

#endif // ONLINEVIEW_H
