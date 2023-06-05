#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "playlist.h"
#include <QTimer>
#include <QPalette>
#include <vector>
#include <QKeyEvent>
#include <QLineEdit>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_playButton_clicked();

    void on_nextButton_clicked();

    void on_progressSlider_sliderMoved(int position);

    void on_positionChanged(qint64 position);

    void on_durationChanged(qint64 position);

    void on_volumeChanged(qint64 position);

    void update();

    void on_shuffleButton_clicked();

    void on_repeatButton_clicked();

    void on_listWidget_doubleClicked();

    void on_volumeSlider_valueChanged(int value);

    void on_backButton_clicked();

    void on_searchBar_textChanged(const QString &arg1);

    void on_actionSave_triggered();

    void on_actionRemove_triggered();

    void on_actionAdd_2_triggered();

private:

    void updateList();

    void loadTrack();

    void next();

    void back();

    void shufflePlaylist();

    int getIndex();

    bool repeat = false;

    bool shuffle = false;

    int lCounter = 0;

    Ui::MainWindow *ui;

    QMediaPlayer* player;

    QAudioOutput* audioOutput;

    Playlist playlist;

    QTimer *updater = new QTimer(this);

    vector<unsigned short int> shuffledPlaylist;

protected:
    void keyPressEvent(QKeyEvent *event);

};
#endif // MAINWINDOW_H
