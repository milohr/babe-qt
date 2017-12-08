#ifndef SCROLLTEXT_H
#define SCROLLTEXT_H

#include "scrolltext.h"
#include <QLabel>
#include <QTimer>
#include <QPainter>

class ScrollText : public QLabel
{
    Q_OBJECT

public: //Member Functions
    enum Direction{LeftToRight,RightToLeft};
    ScrollText(QWidget *parent = nullptr);
    ~ScrollText();
    void show();
    void setAlignment(Qt::Alignment);
    int getSpeed();
    void setMaxSize(const uint &size);

public slots: //Public Member Slots
    void setSpeed(const int &s);
    void setDirection(const int &d);
    void reset();

protected: //Member Functions
    virtual void paintEvent(QPaintEvent *evt);
    virtual void resizeEvent(QResizeEvent *evt);
    virtual void updateCoordinates();
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

private: //Data Members
    int px;
    int py;
    uint maxSize;
    QTimer timer;
    Qt::Alignment m_align;
    int speed;
    int direction;
    int fontPointSize;
    int textLength;

private slots: //Private Member Slots
    void refreshLabel();
};

#endif // SCROLLTEXT_H
