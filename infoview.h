#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QWidget>

namespace Ui {
class InfoView;
}

class InfoView : public QWidget
{
    Q_OBJECT

public:
    explicit InfoView(QWidget *parent = 0);
    ~InfoView();

private:
    Ui::InfoView *ui;
};

#endif // INFOVIEW_H
