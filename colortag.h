#ifndef COLORTAG_H
#define COLORTAG_H
#include <QWidget>
#include <QPainter>

class ColorTag : public QWidget
{
    Q_OBJECT
public:
    explicit ColorTag(QWidget *parent = 0);

signals:

public slots:
   virtual  void paintEvent(QPaintEvent *event);
};

#endif // COLORTAG_H
