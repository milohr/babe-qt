#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QtMultimedia/QMediaPlayer>
#include "playlist.h"
#include "QFileDialog"
#include <QLabel>
#include "QTimer"

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
    void updateList();

private slots:
    void on_toolButton_4_clicked();

    void on_toolButton_3_clicked();

    void on_hide_up_btn_clicked();

    void on_hide_sidebar_btn_clicked();

    void on_shuffle_btn_clicked();

    void on_open_btn_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void update();

    void on_seekBar_sliderMoved(int position);

    void on_play_btn_clicked();

    void on_backward_btn_clicked();

    void on_foward_btn_clicked();



private:
    void loadTrack();
    int getIndex();
    void next();
    void back();
    void shufflePlaylist();

    Ui::MainWindow *ui;
    QToolBar *playback;
    int mini_mode;
    QWidget *main_widget;
    QGridLayout * layout;
    QLabel *info;

    Playlist playlist;
    QMediaPlayer *player = new QMediaPlayer();
    QTimer *updater = new QTimer(this);

    int lCounter = 0;
    bool repeat = false;
    bool muted = false;
    bool playing = false;
    vector<unsigned short int> shuffledPlaylist;bool shuffle = false;

};

#endif // MAINWINDOW_H
