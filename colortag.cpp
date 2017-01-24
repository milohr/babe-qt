#include "colortag.h"

ColorTag::ColorTag(QWidget *parent) : QWidget(parent)
{
    this->update();
    this->setStyleSheet("background-color: blue;");
    this->show();

}

void ColorTag::paintEvent(QPaintEvent *event)
{
     QPainter painter(this);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    QBrush brush(Qt::yellow);

    painter.setPen (pen);
    painter.setBrush(brush);
    painter.drawEllipse(0, 0, 10,10);

}
