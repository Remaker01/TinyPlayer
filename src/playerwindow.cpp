#include "playerwindow.h"
#include "./ui_playerwindow.h"
#ifndef SLOTS
#define SLOTS
#endif
PlayerWindow::PlayerWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::PlayerWindow) {
    ui->setupUi(this);
    QPixmap img(":/Icons/images/back.jpg");
    QPalette pl;
    playButton = new Player(50,50,this);
    playButton->move(10,155);
    playButton->show();
    pl.setBrush(backgroundRole(),QBrush(img));
    setPalette(pl);
    setAutoFillBackground(true);
    registerSlots();
}

inline void PlayerWindow::registerSlots() {
    connect(ui->actionExit,&QAction::triggered,this,&PlayerWindow::ensureExit);
    connect(ui->actionAbout,&QAction::triggered,this,[this]() {
        QMessageBox::information(this,"关于","TinyPlayer播放器\n"
                                                        "基于Qt的简易音频播放器\n\n"
                                                        "界面框架:Qt5.12\n"
                                                        "环境:QT Creator5+CMake3.21+MinGW8.1\n"
                                                        "作者邮箱:latexreal@163.com\n"
                                                        "版本号:1.0 Alpha");
    });
    connect(ui->actionopenFile,&QAction::triggered,this,[this]() {
        QFile media(QFileDialog::getOpenFileName(this,"选择文件","","音频文件(*.mp3;*.wav;*.wma;*.aiff)"));
        QString name = media.fileName();
        if(name.isEmpty())
            return;
        playButton->setMedia(&media);
        playButton->setVolum(ui->volumeSlider->value());
        ui->mediaLabel->setText(playButton->getMedia());
    });
    connect(ui->actionOpenHelp,&QAction::triggered,this,[]() {
        return QDesktopServices::openUrl(QUrl::fromLocalFile("README.htm"));
    });
    //点击关闭按钮，将Media设为空后，也会触发durationChanged信号
    connect(playButton,&Player::mediaChanged,playButton,[this](qint64 totTime) {
        totTime = qRound(totTime / 1000.0);
        static const QLatin1Char zero('0');
        ui->timeLable->setText(QString("/%1:%2").arg(totTime / 60,2,10,zero).arg(totTime % 60,2,10,zero));
        ui->progressSlider->setMaximum(totTime);
    });
    connect(playButton,&Player::timedOut,this,[this]() {
        ui->progressSlider->setValue(playButton->getPos());
    });
    connect(playButton,&Player::finished,this,[this]() {
        ui->progressSlider->setValue(0);
    });
}

void PlayerWindow::ensureExit() {
    QMessageBox box(QMessageBox::Question,"退出确认","确认退出?");
    QPushButton *Yes = box.addButton("是",QMessageBox::AcceptRole);
    box.addButton("否",QMessageBox::RejectRole);
    connect(Yes,&QPushButton::clicked,this,&QMainWindow::close);
    box.exec();
}
SLOTS
void PlayerWindow::on_closeButton_clicked() {
    static const QFile emptyFile("");
    playButton->setMedia(&emptyFile);
    ui->mediaLabel->clear();
    playButton->pause();
}

void PlayerWindow::on_volumeSlider_valueChanged(int value) {
    playButton->setVolum(value);
    QString str = QString("音量：%1").arg(value);
    ui->volLabel->setText(str);
    if(playButton->getState() == QMediaPlayer::PlayingState) {
        playButton->pause(false);
        playButton->start(false);
    }
}
void PlayerWindow::on_progressSlider_valueChanged(int value) {
    static const QLatin1Char zero('0');
    ui->currentLabel->setText(QString("%1:%2").arg(value / 60,2,10,zero).arg(value % 60,2,10,zero));
}

void PlayerWindow::on_progressSlider_sliderMoved(int position) {
    playButton->setPos(position);
    on_progressSlider_valueChanged(position);
}

PlayerWindow::~PlayerWindow() {delete ui;}
