/*
   Babe - tiny music player
   Copyright (C) 2017  Camilo Higuita
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

   */


#include "scrolltext.h"
#include <QDebug>

ScrollText::ScrollText(QWidget *parent) {

    Q_UNUSED(parent);
    px = 0;
    py = 10;
    speed = 0;
    direction = RightToLeft;
    connect(&timer, SIGNAL(timeout()), this, SLOT(refreshLabel()));
    //
}
void ScrollText::refreshLabel() { repaint(); }

void ScrollText::start() {}

void ScrollText::enterEvent(QEvent *event) {
    Q_UNUSED(event);
    if (static_cast<int>(maxSize) < this->sizeHint().width())
        speed = 1;
    timer.start(30);

}

void ScrollText::leaveEvent(QEvent *event) {
    Q_UNUSED(event);
    speed = 0;
    reset();
}
void ScrollText::setMaxSize(uint size)
{
    this->maxSize = size;
    this->setMaximumWidth(static_cast<int>(maxSize));

}

void ScrollText::stop() {
    // connect(&timer, SIGNAL(timeout()), this, SLOT(refreshLabel()));
    // timer.stop();
}

void ScrollText::reset() {
    px = 0;
    repaint();
    timer.stop();
}

ScrollText::~ScrollText()
{
    qDebug()<<"DELETING SCROLLTEXT";
}

void ScrollText::show() { QLabel::show(); }

void ScrollText::setAlignment(Qt::Alignment al) {
    m_align = al;
    updateCoordinates();
    QLabel::setAlignment(al);
}

void ScrollText::paintEvent(QPaintEvent *evt) {
    Q_UNUSED(evt);
    QPainter p(this);
    if (direction == RightToLeft) {
        px -= speed;
        if (px <= (-textLength))
            px = width();
    } else {
        px += speed;
        if (px >= width())
            px = -textLength;
    }
    p.drawText(px, py + fontPointSize, text());
    p.translate(px, 0);
}

void ScrollText::resizeEvent(QResizeEvent *evt) {
    updateCoordinates();
    QLabel::resizeEvent(evt);
}

void ScrollText::updateCoordinates() {
    switch (m_align) {
    case Qt::AlignTop:
        py = 10;
        break;
    case Qt::AlignBottom:
        py = height() - 10;
        break;
    case Qt::AlignVCenter:
        py = height() / 2;
        break;
    }
    fontPointSize = font().pointSize() / 2;
    textLength = fontMetrics().width(text());
}

void ScrollText::setSpeed(int s) { speed = s; }

int ScrollText::getSpeed() { return speed; }

void ScrollText::setDirection(int d) {
    direction = d;
    if (direction == RightToLeft)
        px = width() - textLength;
    else
        px = 0;
    refreshLabel();
}
