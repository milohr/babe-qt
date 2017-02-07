#include "scrolltext.h"
#include <QPainter>
ScrollText::ScrollText(QWidget *parent) {

    Q_UNUSED(parent);
    px = 0;
    py = 10;
    speed = 0;
    direction = RightToLeft;
    connect(&timer, SIGNAL(timeout()), this, SLOT(refreshLabel()));
    timer.start(10);
}
void ScrollText::refreshLabel() { repaint(); }

void ScrollText::start() {}

void ScrollText::enterEvent(QEvent *event) {
    Q_UNUSED(event);
    if (maxSize < this->sizeHint().width())
        speed = 1;
}

void ScrollText::leaveEvent(QEvent *event) {
    Q_UNUSED(event);
    speed = 0;
    reset();
}
void ScrollText::setMaxSize(int size) {
    this->setMaximumWidth(size);
    maxSize = size;
}

void ScrollText::stop() {
    // connect(&timer, SIGNAL(timeout()), this, SLOT(refreshLabel()));
    // timer.stop();
}

void ScrollText::reset() {
    px = 0;
    repaint();
}

ScrollText::~ScrollText() {}

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
