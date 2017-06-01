#ifndef SCROLLTEXT_H
#define SCROLLTEXT_H

#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QWidget>

class ScrollText : public QLabel
{
    Q_OBJECT
public:
    ScrollText(QLabel *parent = 0);
    ~ScrollText();
    int getSpeed();
    void show();
    void setAlignment(Qt::Alignment);
    void start();
    void stop();
    void setMaxSize(int size);

    enum Direction {
        LeftToRight,
        RightToLeft
    };

public slots:
    void setSpeed(int s);
    void setDirection(int d);
    void reset();

private slots:
    void refreshLabel();

protected:
    virtual void paintEvent(QPaintEvent *evt);
    virtual void resizeEvent(QResizeEvent *evt);
    virtual void updateCoordinates();
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);

private:
    int direction;
    int fontPointSize;
    int maxSize;
    int px;
    int py;
    int speed;
    int textLength;
    QTimer timer;
    Qt::Alignment m_align;
};

#endif // SCROLLTEXT_H
