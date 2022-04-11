#include "playerwindow.h"
#define CHANGE_TO_PLAYICON player->changeState(playButton,"开始",PLAY_ICON,PlayerCore::STOP)
#define CHANGE_TO_PAUSEICON player->changeState(playButton,"暂停",PAUSE_ICON,PlayerCore::START)
static const QString CONFIG_FILE = "player.config";
static const QLatin1Char zero('0');
static const QString LAST_PATH = "/CONFIG/PATH",LAST_VOL = "/CONFIG/VOLUME";
PlayerWindow::PlayerWindow(QWidget *parent):
    PLAY_ICON(":/Icons/images/play.png"),PAUSE_ICON(":/Icons/images/pause.png"),QMainWindow(parent), ui(new Ui::PlayerWindow) {
    ui->setupUi(this);
    player = new PlayerCore;
    initUi();
    initConfiguration();
    connectSlots();
}

inline void PlayerWindow::initUi() {
    playButton = new PlayerButton(50,50,this);
    stopButton = new PlayerButton(45,45,this);
    setButton(playButton,PLAY_ICON,QPoint(10,150));
    setButton(stopButton,QPixmap(":/Icons/images/stop.png"),QPoint(70,155));
    playButton->setToolTip("开始");
    stopButton->setToolTip("停止");
    ui->delButton->setEnabled(false);
    initSystemtray();
    setBackground(QPixmap(":/Icons/images/back.jpg"));
    ui->waitingLabel->hide();
    initPlayList();
}

inline void PlayerWindow::initPlayList() {
    playListModel = new QStringListModel(this);
    ui->listView->setModel(playListModel);
}

inline void PlayerWindow::initSystemtray() {
    tray = new QSystemTrayIcon(QIcon(":/Icons/images/icon.ico"),this);
    tray->setToolTip("TinyPlayer");
    QMenu *trayMenu = new QMenu;
    QAction *e = trayMenu->addAction("退出");
    tray->setContextMenu(trayMenu);
    connect(e,&QAction::triggered,qApp,&QApplication::quit);
    tray->show();
}

inline void PlayerWindow::setBackground(const QPixmap &img) {
    static QPalette pl;
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

inline void PlayerWindow::initConfiguration() {
    QSettings setting(CONFIG_FILE,QSettings::IniFormat);
    if(!QFile::exists(CONFIG_FILE)) {
        setting.setValue(LAST_PATH,QCoreApplication::applicationDirPath());
        setting.setValue(LAST_VOL,50);
    }
    else {
        lastPath = setting.value(LAST_PATH).toString();
        int v = setting.value(LAST_VOL).toInt();
        ui->volumeSlider->setValue(v);
    }
}

inline void PlayerWindow::connectSlots() {
    connectUiSlots();
    connect(playButton,&PlayerButton::clicked,this,[this](){
         if(player->isAudioAvailable()) {
             stopButton->setReplyClick(true);
             if(player->state() != QMediaPlayer::PlayingState) {
                 player->play();
                 stopButton->setReplyClick(true);
                 CHANGE_TO_PAUSEICON;
             }
             else {
                 player->pause();
                 CHANGE_TO_PLAYICON;
             }
         }
         else {
             QMessageBox err(QMessageBox::Critical,"播放失败","文件可能已损坏");
             err.addButton("确定",QMessageBox::AcceptRole);
             err.exec();
         }
    });
    connect(stopButton,&PlayerButton::clicked,this,[this]() {
        if(player->isAudioAvailable()&&player->state() != QMediaPlayer::StoppedState) {
           player->stop();
           CHANGE_TO_PLAYICON;
           stopButton->setReplyClick(false);
           ui->progressSlider->setValue(0);
        }
    });
    connect(player,&PlayerCore::durationChanged,playButton,[&,this](qint64 totTime) {
        playButton->setReplyClick(true);
        totTime = qRound(totTime / 1000.0);
        //改变总时间
        ui->timeLable->setText(QString("/%1:%2").arg(totTime / 60,2,10,zero).arg(totTime % 60,2,10,zero));
        ui->progressSlider->setMaximum(totTime);
        ui->mediaLabel->setText(player->getMedia().fileName());
        //重置进度条（好像不加也可以？）
        ui->progressSlider->setValue(0);
    });
    //定时器发出信号，开始改变时间
    connect(player,&PlayerCore::timedOut,this,[this]() {
        ui->progressSlider->setValue(player->getPosInSecond());
    });
    //停止后重置进度条
    connect(player,&PlayerCore::finished,this,[&,this]() {
        ui->progressSlider->setValue(0);
        CHANGE_TO_PLAYICON;
        stopButton->setReplyClick(false);
    });
    connect(tray,&QSystemTrayIcon::activated,this,[this](QSystemTrayIcon::ActivationReason r){
        if(r == QSystemTrayIcon::Trigger)
            QMainWindow::showNormal();
    });
}

inline void PlayerWindow::connectUiSlots() {
    connect(ui->actionExit,&QAction::triggered,this,&PlayerWindow::ensureExit);
    connect(ui->listView,&PlayListView::mediaDropin,this,&PlayerWindow::doAddMedia);
    connect(ui->delButton,&QPushButton::clicked,this,&PlayerWindow::doDelMedia);
    connect(ui->listView,&PlayListView::itemDelRequirement,this,&PlayerWindow::doDelMedia);
    connect(ui->progressSlider,&PlayerSlider::playerSliderClicked,player,&PlayerCore::setPos);
    connect(ui->actionAbout,&QAction::triggered,this,[this]() {
        QMessageBox box(QMessageBox::Information,"关于","TinyPlayer播放器\n"
                                                        "基于Qt的简易音频播放器\n\n"
                                                        "环境:QT5.12+QT Creator5+CMake3.21+MinGW8.1\n"
                                                        "作者邮箱:latexreal@163.com\n"
                                                        "版本号:1.5 Beta  1.5.220408");
        box.addButton("确定",QMessageBox::AcceptRole);
        QPushButton *b = box.addButton("项目地址",QMessageBox::NoRole);
        connect(b,&QPushButton::clicked,this,[]{QDesktopServices::openUrl(QUrl("https://github.com/Remaker01/TinyPlayer"));});
        box.exec();
    });
    connect(ui->actionopenFile,&QAction::triggered,this,[this]() {
        QStringList medias(QFileDialog::getOpenFileNames(this,"选择文件",lastPath,"音频文件(*.mp3;*.wav;*.wma;*.aiff)"));
        if(!medias.isEmpty())
            doAddMedia(medias);
    });
    connect(ui->modeButton,&PlayerButton::clicked,this,[this]() {
        const QString TIPS[]={"单曲播放","顺序播放","单曲循环"};
        int mode = (int)player->mode;
        mode = (mode + 1) % (sizeof(TIPS)/sizeof (TIPS[0]));
        QString nextName = ":/Icons/images/" + QString::number(mode) + ".png";
        ui->modeButton->setPixmap(QPixmap(nextName));
        player->mode = (PlayerCore::PlayMode)mode;
        ui->modeButton->setToolTip(TIPS[mode]);
    });
    connect(ui->actionOpenHelp,&QAction::triggered,this,[]() {
        return QDesktopServices::openUrl(QUrl::fromLocalFile("README.htm"));
    });
    connect(ui->actionLoadImg,&QAction::triggered,this,[this]() {
        QString back = QFileDialog::getOpenFileName(this,"选择文件","","所有图片文件(*.jpg;*.jpeg;*.png;*.jfif)");
        setBackground(QPixmap(back));
    });
    connect(ui->actionToDefault,&QAction::triggered,this,[this](){
        setBackground(QPixmap(":/Icons/images/back.jpg"));
    });
    connect(ui->volumeButton,&PlayerButton::clicked,this,[this]() {
        static const QPixmap muted(":/Icons/images/muted.png");
        static const QPixmap unMuted(":/Icons/images/volume.png");
        if(player->volume() > 0) {
            ui->volumeButton->setPixmap(muted);
            emit ui->volumeSlider->valueChanged(0);
        }
        else{
            int v = ui->volumeSlider->value();
            if(v > 0) {
                ui->volumeButton->setPixmap(unMuted);
                emit ui->volumeSlider->valueChanged(v);
            }
        }
    });
    connect(ui->listView,&PlayListView::openDirRequirement,this,[this](int row){
        QUrl url(player->getMedia(row));
        QFileInfo tmp(url.toString(QUrl::PreferLocalFile));
        QDesktopServices::openUrl(QUrl::fromLocalFile(tmp.absolutePath()));
    });
    connect(ui->actionOpenDir,&QAction::triggered,this,[this]() {
        QString s = QFileDialog::getExistingDirectory(this,"选择文件夹",lastPath);
        if(s.isEmpty())
            return;
        QDir dir(s);
        QStringList files = dir.entryList();
        for (QString &s : files)
            s = dir.absolutePath() + '/' + s;
        doAddMedia(files);
    });
    connect(ui->listView,&PlayListView::showDetailRequirement,this,[this](int row) {
        QMessageBox::information(this,"信息",player->getMediaDetail(row).toString());
    });
}

inline void PlayerWindow::ensureExit() {
    QMessageBox box(QMessageBox::Question,"退出确认","确认退出?");
    QPushButton *minimize = box.addButton("最小化",QMessageBox::AcceptRole);
    QPushButton *yes = box.addButton("是",QMessageBox::NoRole);
    box.addButton("否",QMessageBox::RejectRole);
    connect(yes,&QPushButton::clicked,qApp,&QApplication::quit);
    connect(minimize,&QPushButton::clicked,this,&QMainWindow::hide);
    box.exec();
}

inline void PlayerWindow::doAddMedia(QStringList medias) {
    ui->waitingLabel->show();
    int s = playList.size();
    lastPath = QFileInfo(medias.last()).absolutePath();
    for(QString &name:medias) {
        if(name.isEmpty())    continue;
        QFileInfo a(name);
        ui->waitingLabel->setText("正在打开" + a.fileName());
        if(player->addToList(name))
             playList.append(a.fileName());
    }
    playListModel->setStringList(playList);
    if(playList.size() > 0)
        ui->delButton->setEnabled(true);
    if(s == 0&&playList.size() > 0)
        player->setCurrentMediaIndex(0u);
    ui->waitingLabel->hide();
}
SLOTS
void PlayerWindow::on_volumeSlider_valueChanged(int value) {
    if(value > 0)
        ui->volumeButton->setPixmap(QPixmap(":/Icons/images/volume.png"));
    else
        ui->volumeButton->setPixmap(QPixmap(":/Icons/images/muted.png"));
    player->setVolume(value);
    ui->volLabel->setText(QString("音量：%1").arg(value));
    if(player->state() == QMediaPlayer::PlayingState) {
        player->pause();
        player->play();
    }
}

void PlayerWindow::on_progressSlider_valueChanged(int value) {
    ui->currentLabel->setText(QString("%1:%2").arg(value / 60,2,10,zero).arg(value % 60,2,10,zero));
}

void PlayerWindow::on_progressSlider_sliderMoved(int position) {
    player->setPos(position);
    on_progressSlider_valueChanged(player->getPosInSecond());
}

void PlayerWindow::on_listView_doubleClicked(const QModelIndex &index) {
    player->setCurrentMediaIndex((uint)index.row());
}

void PlayerWindow::doDelMedia() {
    QModelIndexList tmp = ui->listView->getSelections();
    QList<int> selections;
    for (QModelIndex &i:tmp)    selections.append(i.row());
    std::sort(selections.begin(),selections.end(),std::greater<int>());
    for (int i:selections) {
        if(!player->removeFromList((uint)i))
            QMessageBox::critical(this,"错误","删除失败。");
        else {
            playList.removeAt(i);
            playListModel->setStringList(playList);
        }
    }
    bool a = (playList.size() > 0);
    ui->delButton->setEnabled(a);
    playButton->setReplyClick(a&&player->getCurrentMediaIndex() >= 0);
    stopButton->setReplyClick(a&&player->getCurrentMediaIndex() >= 0);
}

void PlayerWindow::on_clearButton_clicked() {
    playList.clear();
    playListModel->setStringList(playList);
    player->clear();
    ui->delButton->setEnabled(false);
    playButton->setReplyClick(false);
    stopButton->setReplyClick(false);
}

PlayerWindow::~PlayerWindow() {
    QSettings setting(CONFIG_FILE,QSettings::IniFormat);
    setting.setValue(LAST_PATH,lastPath);
    setting.setValue(LAST_VOL,ui->volumeSlider->value());
    delete ui;
}
