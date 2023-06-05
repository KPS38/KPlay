#define _HAS_AUTO_PTR_ETC 1

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <algorithm>
#include <iostream>
#include <string>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this);

    audioOutput = new QAudioOutput(this);

    player->setAudioOutput(audioOutput);

    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(on_positionChanged(qint64)));

    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(on_durationChanged(qint64)));

    connect(player, SIGNAL(valueChanged(qint64)), this,  SLOT(on_volumeSlider_valueChanged(int)));

    connect(audioOutput, SIGNAL(volumeChanged(float)), this, SLOT(on_volumeChanged(qint64)));

    audioOutput->setVolume(100);

    this->setFixedSize(this->geometry().width(),this->geometry().height());

    updateList();

    connect(updater, SIGNAL(timeout()), this, SLOT(update()));

    ui->listWidget->setCurrentRow(0);

    if(ui->listWidget->count() != 0){
        loadTrack();
        player->pause();
        updater->start();

    }

    this->setWindowTitle("KPlay");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playButton_clicked()
{
    if(ui->listWidget->count() != 0){
        if(player->playbackState() == QMediaPlayer::PlayingState)
        {
            player->pause();
            ui->playButton->setText(">");
        }
        else
        {
            player->play();
            ui->playButton->setText("||");
            updater->start();
        }
   }
}

void MainWindow::on_nextButton_clicked()
{
    if(ui->listWidget->count() != 0)
    {
       if(repeat)
       {
           repeat = !repeat;
           next();
           repeat = !repeat;
       }
       else
       {
           next();
       }
    }
}


void MainWindow::on_backButton_clicked()
{
    if(ui->listWidget->count() != 0)
    {
       if(player->position() > 3000)
       {
          player->setPosition(0);
       }
       else
       {
           back();
       }
    }
}


void MainWindow::on_volumeSlider_valueChanged(int value)
{
    audioOutput->setVolume(float(value)/100);
}


void MainWindow::on_progressSlider_sliderMoved(int position)
{
    player->setPosition(position);
}


void MainWindow::on_positionChanged(qint64 position)
{
    ui->progressSlider->setValue(position);
}


void MainWindow::on_durationChanged(qint64 position)
{
    ui->progressSlider->setMaximum(position);
}


void MainWindow::on_volumeChanged(qint64 position)
{
    ui->volumeSlider->setMaximum(position);
}

void MainWindow::on_shuffleButton_clicked()
{
    shuffle = !shuffle;
    if(shuffle)
        shufflePlaylist();
}


void MainWindow::on_repeatButton_clicked()
{
    repeat = !repeat;
}

void MainWindow::on_listWidget_doubleClicked()
{
    lCounter = getIndex();

    ui->playButton->setChecked(false);
    ui->searchBar->clear();

    loadTrack();
    player->play();
    ui->playButton->setText("||");
}

void MainWindow::update()
{   if(!ui->progressSlider->isSliderDown())
        ui->progressSlider->setValue((double)player->position());

    if(player->playbackState() == QMediaPlayer::StoppedState)
    {
        next();
    }
}


void MainWindow::updateList()
{
    ui->listWidget->clear();
    ui->listWidget->addItems(playlist.getTracksNameList());
}


int MainWindow::getIndex()
{
    return ui->listWidget->currentIndex().row();
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Return :
    {
        lCounter = getIndex();
        if(lCounter != -1)
        {
            ui->playButton->setChecked(false);
            ui->searchBar->clear();

           loadTrack();
           player->play();
           ui->playButton->setText("||");
        }
        break;
    }
    case Qt::Key_Up :
    {
        int ind = getIndex() - 1;if(ind < 0)ind = ui->listWidget->count() - 1;
        ui->listWidget->setCurrentRow(ind);
        break;
    }
    case Qt::Key_Down :
    {
        int ind = getIndex() + 1;if(ind >= ui->listWidget->count())ind = 0;
        ui->listWidget->setCurrentRow(ind);
        break;
    }
    case Qt::Key_Space :
        player->play();
        ui->playButton->setText("||");
    default :
    {
        ui->searchBar->setFocus();

        break;
    }
    }
}


void MainWindow::next()
{
    lCounter++;

    if(repeat)
    {
        lCounter--;
    }

    if(lCounter >= ui->listWidget->count())
        lCounter = 0;

    (!shuffle or repeat) ? ui->listWidget->setCurrentRow(lCounter) : ui->listWidget->setCurrentRow(shuffledPlaylist[lCounter]);

    ui->playButton->setChecked(false);
    ui->searchBar->clear();

    loadTrack();
    player->play();
    ui->playButton->setText("||");

}


void MainWindow::back()
{
     lCounter--;

     if(lCounter < 0)
        lCounter = ui->listWidget->count() - 1;


     (!shuffle) ? ui->listWidget->setCurrentRow(lCounter) : ui->listWidget->setCurrentRow(shuffledPlaylist[lCounter]);

     ui->playButton->setChecked(false);
     ui->searchBar->clear();

     loadTrack();
     player->play();
     ui->playButton->setText("||");
}


void MainWindow::shufflePlaylist()
{
    shuffledPlaylist.resize(0);

    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        shuffledPlaylist.push_back(i);
    }

    random_shuffle(shuffledPlaylist.begin(), shuffledPlaylist.end());
}


void MainWindow::loadTrack()
{
     QString qstr = QString::fromStdString(playlist.tracks[getIndex()].getLocation());
     player->setSource(QUrl::fromLocalFile(qstr));
     qstr = QString::fromStdString(playlist.tracks[getIndex()].getName());
     ui->songName->setText(qstr);
}


void MainWindow::on_searchBar_textChanged(const QString &arg1)
{
    if(ui->searchBar->text().toStdString() != "")
    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        if(ui->listWidget->item(i)->text().toLower().toStdString().find(arg1.toLower().toStdString()) != string::npos )
        {
            ui->listWidget->setCurrentRow(i);
            break;
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    playlist.save();
    ui->actionSave->setChecked(true);
}


void MainWindow::on_actionRemove_triggered()
{
    int index = getIndex();
    if(index != -1)
    {
       playlist.remove(index);
       updateList();
       ui->listWidget->setCurrentRow(index);
       ui->actionSave->setChecked(false);
       if(shuffle) shufflePlaylist();
    }
}


void MainWindow::on_actionAdd_2_triggered()
{
    bool startUpdater = false;if(ui->listWidget->count() == 0) startUpdater = true;
      QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"));
      if(!files.empty())
      {
          playlist.add(files);
          updateList();
          ui->actionSave->setChecked(false);
          if(shuffle) shufflePlaylist();
          if(startUpdater) updater->start();
      }
}

