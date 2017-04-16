#ifndef ONLINEVIEW_H
#define ONLINEVIEW_H

#include <QWidget>

namespace Ui {
class onlineView;
}

class onlineView : public QWidget
{
    Q_OBJECT

public:
    explicit onlineView(QWidget *parent = 0);
    ~onlineView();

private:
    Ui::onlineView *ui;
};

#endif // ONLINEVIEW_H
