#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setStyle();

private slots:
    void on_toolButton_4_clicked();

    void on_toolButton_3_clicked();

    void on_hide_up_btn_clicked();

    void on_hide_sidebar_btn_clicked();

    void on_shuffle_btn_clicked();

private:
    Ui::MainWindow *ui;
    QToolBar *playback;
    int mini_mode;
    QWidget *main_widget;
    QGridLayout * layout;
};

#endif // MAINWINDOW_H
