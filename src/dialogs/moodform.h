#ifndef MOODFORM_H
#define MOODFORM_H

#include <QWidget>

class MoodForm : public QWidget
{
    Q_OBJECT

public:
    explicit MoodForm(QWidget *parent = nullptr);
    ~MoodForm();

signals:
    void saved();
};

#endif // MOODFORM_H
