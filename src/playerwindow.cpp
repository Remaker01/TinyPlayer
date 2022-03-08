#include "playerwindow.h"
#include "./ui_playerwindow.h"
#include <QThread>
#ifndef SLOTS
#define SLOTS
#endif
PlayerWindow::PlayerWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::PlayerWindow) {
    ui->setupUi(this);
    playButton = new PlayerButton(50,50,this);
    stopButton = new PlayerButton(45,45,this);
    setButton(playButton,QPixmap(":/Icons/images/play.png"),QPoint(10,155));
    setButton(stopButton,QPixmap(":/Icons/images/stop.png"),QPoint(70,160));
    playButton->setToolTip("开始");
    stopButton->setToolTip("停止");
    player = new PlayerCore;
    setBackground();
    registerSlots();
}

inline void PlayerWindow::setBackground() {
    QPixmap img(":/Icons/images/back.jpg");
    QPalette pl;
    pl.setBrush(backgroundRole(),QBrush(img));
    setPalette(pl);
    setAutoFillBackground(true);
}

inline void PlayerWindow::setButton(PlayerButton *button, const QPixmap &pic, const QPoint &loc) {
    button->move(loc);
    button->setPixmap(pic);
    button->setReplyClick(false);
    button->show();
}

inline void PlayerWindow::registerSlots() {
    connect(ui->actionExit,&QAction::triggered,this,&PlayerWindow::ensureExit);
    connect(ui->actionAbout,&QAction::triggered,this,[this]() {
        QMessageBox::information(this,"关于","TinyPlayer播放器\n"
                                                        "基于Qt的简易音频播放器\n\n"
                                                        "界面框架:Qt5.12\n"
                                                        "环境:QT Creator5+CMake3.21+MinGW8.1\n"
                                                        "作者邮箱:latexreal@163.com\n"
                                                        "版本号:1.0 Beta1  1.0.220306");
    });
    connect(ui->actionopenFile,&QAction::triggered,this,[this]() {
        QFile media(QFileDialog::getOpenFileName(this,"选择文件","","音频文件(*.mp3;*.wav;*.wma;*.aiff)"));
        QString name = media.fileName();
        if(name.isEmpty()||QUrl::fromLocalFile(name) == player->getMedia())
            return;
        player->setMedia(&media);
        ui->mediaLabel->setText(player->getMedia().fileName());
    });
    connect(ui->actionOpenHelp,&QAction::triggered,this,[]() {
        return QDesktopServices::openUrl(QUrl::fromLocalFile("README.htm"));
    });
    //按下按钮，开始播放
    connect(playButton,&PlayerButton::clicked,this,[this](){
         if(player->isAudioAvailable()) {
            setIcon();
         }
    });
    connect(stopButton,&PlayerButton::clicked,this,[this]() {
        if(player->isAudioAvailable()&&player->state() != QMediaPlayer::StoppedState) {
           player->stop();
           player->changeState(playButton,"开始",QPixmap(":/Icons/images/play.png"),PlayerCore::STOP);
           ui->progressSlider->setValue(0);
        }
    });
    //音频改变，触发durationChanged信号，改变标签
    connect(player,&PlayerCore::durationChanged,playButton,[&,this](qint64 totTime) {
        playButton->setReplyClick(player->isAudioAvailable());
        //注意这里只能改为开始图标，不要调用setIcon();
        player->changeState(playButton,"开始",QPixmap(":/Icons/images/play.png"),PlayerCore::STOP);
        totTime = qRound(totTime / 1000.0);
        //改变总时间
        static const QLatin1Char zero('0');
        ui->timeLable->setText(QString("/%1:%2").arg(totTime / 60,2,10,zero).arg(totTime % 60,2,10,zero));
        ui->progressSlider->setMaximum(totTime);
        //重置进度条（好像不加也可以？）
        ui->progressSlider->setValue(0);
    });
    //定时器发出信号，开始改变时间
    connect(player,&PlayerCore::timedOut,this,[this]() {
        ui->progressSlider->setValue(player->getPosInSecond());
    });
    //停止后重置进度条，不自动暂停（因为已经停止了）
    connect(player,&QMediaPlayer::stateChanged,this,[&,this](QMediaPlayer::State nState) {
        if(nState == QMediaPlayer::StoppedState&&player->position() > 0ll) {
            ui->progressSlider->setValue(0);
            setIcon(false);
        }
        stopButton->setReplyClick(nState != QMediaPlayer::StoppedState);
    });
    connect(ui->progressSlider,&PlayerSlider::playerSliderClicked,this,[&,this](int loc) {
        player->setPos(loc);
    });
}

void PlayerWindow::setIcon(bool needOperation) {
    static const QPixmap playIcon(":/Icons/images/play.png");
    static const QPixmap pauseIcon(":/Icons/images/pause.png");
    if(*(playButton->pixmap()) == playIcon) {
        if(needOperation) {
            player->play();
            stopButton->setReplyClick(true);
        }
        player->changeState(playButton,"暂停",pauseIcon,PlayerCore::START);
    }
    else {
        if(needOperation) player->pause();
        player->changeState(playButton,"开始",playIcon,PlayerCore::STOP);
    }
}

inline void PlayerWindow::ensureExit() {
    QMessageBox box(QMessageBox::Question,"退出确认","确认退出?");
    QPushButton *Yes = box.addButton("是",QMessageBox::AcceptRole);
    box.addButton("否",QMessageBox::RejectRole);
    connect(Yes,&QPushButton::clicked,this,&QMainWindow::close);
    box.exec();
}
SLOTS
void PlayerWindow::on_closeButton_clicked() {
    if(player->state() != QMediaPlayer::StoppedState)
        player->stop();
    static const QFile emptyFile("");
    player->setMedia(&emptyFile);
    ui->mediaLabel->clear();
    player->stop();
}

void PlayerWindow::on_volumeSlider_valueChanged(int value) {
    player->setVolume(value);
    QString str = QString("音量：%1").arg(value);
    ui->volLabel->setText(str);
    if(player->state() == QMediaPlayer::PlayingState) {
        player->pause();
        player->play();
    }
}

void PlayerWindow::on_progressSlider_valueChanged(int value) {
    static const QLatin1Char zero('0');
    ui->currentLabel->setText(QString("%1:%2").arg(value / 60,2,10,zero).arg(value % 60,2,10,zero));
}

void PlayerWindow::on_progressSlider_sliderMoved(int position) {
    player->setPos(position);
    on_progressSlider_valueChanged(player->getPosInSecond());
}

PlayerWindow::~PlayerWindow() {delete ui;}
