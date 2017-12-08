#ifndef MOODFORM_H
#define MOODFORM_H

#include <QWidget>

namespace Ui {
class MoodForm;
}

class MoodForm : public QWidget
{
    Q_OBJECT

public:
    explicit MoodForm(QWidget *parent = nullptr);
    ~MoodForm();

private:
    Ui::MoodForm *ui;

signals:
    void saved();
};

#endif // MOODFORM_H
