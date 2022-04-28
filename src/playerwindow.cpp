#include "playerwindow.h"
#define CHANGE_TO_PLAYICON ui->playButton->changeState("开始",PLAY_ICON)
#define CHANGE_TO_PAUSEICON ui->playButton->changeState("暂停",PAUSE_ICON)
const QString PlayerWindow::CONFIG_FILE = "player.config";
static const QLatin1Char zero('0');
static const QString LAST_PATH = "/CONFIG/PATH",LAST_VOL = "/CONFIG/VOLUME",LAST_MODE = "/CONFIG/MODE";
PlayerWindow::PlayerWindow(QWidget *parent):
    PLAY_ICON(":/Icons/images/play.png"),PAUSE_ICON(":/Icons/images/pause.png"),QMainWindow(parent), ui(new Ui::PlayerWindow) {
    ui->setupUi(this);
    player = new PlayerCore(this);
    initUi();
    initConfiguration();
    connectSlots();
}

inline void PlayerWindow::initUi() {
    ui->playButton->setReplyClick(false);
    ui->stopButton->setReplyClick(false);
    ui->delButton->setEnabled(false);
    ui->listView->setOpacity(0.6);
    initSystemtray();
    setBackground(QPixmap(":/Icons/images/back.jpg"));
    ui->waitingLabel->hide();
    ui->cancelButton->hide();
    initPlayList();
}

inline void PlayerWindow::initPlayList() {
    playListModel = new QStringListModel(this);
    ui->listView->setModel(playListModel);
}

inline void PlayerWindow::initSystemtray() {
    tray = new QSystemTrayIcon(QIcon(":/Icons/images/icon.ico"),this);
    tray->setToolTip("TinyPlayer");
    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction("打开窗口",this,&QMainWindow::showNormal);
    trayMenu->addAction("退出",qApp,&QApplication::quit);
    tray->setContextMenu(trayMenu);
    tray->show();
}

inline void PlayerWindow::setBackground(const QPixmap &img) {
    QPalette pl;
    pl.setBrush(backgroundRole(),QBrush(img));
    setPalette(pl);
    setAutoFillBackground(true);
}

inline void PlayerWindow::initConfiguration() {
    QSettings setting(CONFIG_FILE,QSettings::IniFormat);
    if(!QFile::exists(CONFIG_FILE)) {
        setting.setValue(LAST_PATH,QCoreApplication::applicationDirPath());
        setting.setValue(LAST_VOL,50);
        setting.setValue(LAST_MODE,0);
    }
    lastPath = setting.value(LAST_PATH).toString();
    ui->volumeSlider->setValue(setting.value(LAST_VOL).toInt());
    changeMode((PlayerCore::PlayMode)setting.value(LAST_MODE).toInt());
}

inline void PlayerWindow::changeMode(PlayerCore::PlayMode m) {
    const QString TIPS[]={"单曲播放","顺序播放","单曲循环","列表循环"};
    QString name = ":/Icons/images/" + QString::number((int)m) + ".png";
    ui->modeButton->setPixmap(QPixmap(name));
    player->mode = m;
    ui->modeButton->setToolTip(TIPS[(int)m]);
}

inline void PlayerWindow::connectSlots() {
    connectUiSlots();
    connect(ui->playButton,&PlayerButton::clicked,this,[this](){
         Vlc::State state = player->state();
         if(state != Vlc::Error) {
             ui->stopButton->setReplyClick(true);
             if(player->state() != Vlc::Playing) {
                 player->play();
                 CHANGE_TO_PAUSEICON;
             }
             else {
                 player->togglePause();
                 CHANGE_TO_PLAYICON;
             }
         }
         else {
             QMessageBox::critical(this,"播放失败","文件可能已损坏");
         }
    });
    connect(ui->stopButton,&PlayerButton::clicked,this,[this]() {
        if(player->state() != Vlc::Stopped) {
           player->stop();
           CHANGE_TO_PLAYICON;
           ui->stopButton->setReplyClick(false);
           ui->progressSlider->setValue(0);
        }
    });
    connect(player,&PlayerCore::lengthChanged,this,[&,this](int totTime) {
        ui->playButton->setReplyClick(true);
        totTime = qRound(totTime / 1000.0);
        //改变总时间
        ui->timeLable->setText(QString("/%1:%2").arg(totTime / 60,2,10,zero).arg(totTime % 60,2,10,zero));
        ui->progressSlider->setMaximum(totTime);
        QFileInfo tmp(player->getMedia());
        ui->mediaLabel->setText(tmp.fileName());
        //重置进度条（好像不加也可以？）
        ui->progressSlider->setValue(0);
    });
    //定时器发出信号，开始改变时间
    connect(player,&PlayerCore::timeChanged,this,[this]() {
        ui->progressSlider->setValue(player->getPosInSecond());
    });
    //停止后重置进度条
    connect(player,&PlayerCore::finished,this,[&,this]() {
        ui->progressSlider->setValue(0);
        CHANGE_TO_PLAYICON;
        ui->stopButton->setReplyClick(false);
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
    connect(ui->actionAbout,&QAction::triggered,this,[this] {
        QMessageBox box(QMessageBox::Information,"关于","TinyPlayer播放器\n"
                                                        "基于Qt的简易音频播放器\n\n"
                                                        "环境:QT5.12+QT Creator5+CMake3.21+MinGW8.1\n"
                                                        "作者邮箱:latexreal@163.com\n"
                                                        "版本号:2.0 Alpha  2.0.220426");
        box.addButton("确定",QMessageBox::AcceptRole);
        QPushButton *b = box.addButton("项目地址",QMessageBox::NoRole);
        connect(b,&QPushButton::clicked,this,[]{
            QDesktopServices::openUrl(QUrl("https://github.com/Remaker01/TinyPlayer"));
        });
        box.exec();
    });
    connect(ui->actionopenFile,&QAction::triggered,this,[this]() {
        QString filiter = "音频文件(";
        for(const QString &f : PlayerCore::Formats)
            filiter.append('*' + f + ';');
        filiter.append(')');
        QStringList medias(QFileDialog::getOpenFileNames(this,"选择文件",lastPath,filiter));
        if(!medias.isEmpty())
            doAddMedia(medias);
    });
    connect(ui->modeButton,&PlayerButton::clicked,this,[this]() {
        int now = (int)player->mode;
        changeMode((PlayerCore::PlayMode)((now + 1) % PlayerCore::MODE_COUNT));
    });
    connect(ui->actionOpenHelp,&QAction::triggered,this,[]() {
        return QDesktopServices::openUrl(QUrl::fromLocalFile("README.htm"));
    });
    connect(ui->actionLoadImg,&QAction::triggered,this,[this]() {
        QString back = QFileDialog::getOpenFileName(this,"选择文件","","图片文件(*.jpg;*.jpeg;*.png;*.jfif)");
        setBackground(QPixmap(back));
    });
    connect(ui->actionToDefault,&QAction::triggered,this,[this](){
        setBackground(QPixmap(":/Icons/images/back.jpg"));
    });
    connect(ui->volumeButton,&PlayerButton::clicked,this,[this]() {
        static const QPixmap muted(":/Icons/images/muted.png");
        static const QPixmap unMuted(":/Icons/images/volume.png");
        if(player->audio()->volume() > 0) {
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
        QFileInfo tmp(player->getMedia(row));
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
SLOTS
void PlayerWindow::doAddMedia(QStringList medias) {
    bool f = true;
    long st = clock();
    ui->waitingLabel->show();
    ui->cancelButton->show();
    connect(ui->cancelButton,&QPushButton::clicked,[&]{f = false;});
    lastPath = QFileInfo(medias.last()).absolutePath();
    for(QString &fullName:medias) {
        if(!f)
            break;
        QFileInfo a(fullName);
        ui->waitingLabel->setText("正在打开" + a.fileName());
        if(player->addToList(fullName))
             playList.append(a.fileName() + '\n' + Music::getMediaDetail(fullName).formatTime());
    }
    playListModel->setStringList(playList);
    if(playList.size() > 0)
        ui->delButton->setEnabled(true);
    player->setCurrentMediaIndex(0u);
    //这里一定要隐藏按钮，防止函数退出后继续调用上面的lambda表达式
    ui->cancelButton->hide();
    ui->waitingLabel->hide();
    qDebug() << clock() - st;
}

void PlayerWindow::on_volumeSlider_valueChanged(int value) {
    if(value > 0)
        ui->volumeButton->setPixmap(QPixmap(":/Icons/images/volume.png"));
    else
        ui->volumeButton->setPixmap(QPixmap(":/Icons/images/muted.png"));
    player->audio()->setVolume(value);
    ui->volLabel->setText(QString("音量：%1").arg(value));
}

void PlayerWindow::on_progressSlider_valueChanged(int value) {
    ui->currentLabel->setText(QString("%1:%2").arg(value / 60,2,10,zero).arg(value % 60,2,10,zero));
}

void PlayerWindow::on_progressSlider_sliderMoved(int position) {
    player->setPos(position);
    on_progressSlider_valueChanged(player->getPosInSecond());
}

void PlayerWindow::on_listView_doubleClicked(const QModelIndex &index) {
    player->setCurrentMediaIndex(index.row());
}
#define LIST_DEL_ACTION(RPYCLICK_CONDITION) ui->playButton->setReplyClick(RPYCLICK_CONDITION);\
    ui->delButton->setEnabled(RPYCLICK_CONDITION);\
    if(!(RPYCLICK_CONDITION)) {\
        ui->timeLable->setText("/00:00");\
        ui->mediaLabel->clear();\
}
void PlayerWindow::doDelMedia() {
    QModelIndexList tmp = ui->listView->getSelections();
    QList<int> selections;
    for (QModelIndex &i:tmp)
        selections.append(i.row());
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
    ui->stopButton->setReplyClick(a);
    LIST_DEL_ACTION(a)
}

void PlayerWindow::on_clearButton_clicked() {
    playList.clear();
    playListModel->setStringList(playList);
    player->clear();
    LIST_DEL_ACTION(false)
}
#undef LIST_DEL_ACTION
void PlayerWindow::on_addButton_clicked() {
    ui->actionopenFile->trigger();
}

PlayerWindow::~PlayerWindow() {
    QSettings setting(CONFIG_FILE,QSettings::IniFormat);
    setting.setValue(LAST_PATH,lastPath);
    setting.setValue(LAST_VOL,ui->volumeSlider->value());
    setting.setValue(LAST_MODE,(int)player->mode);
    delete ui;
}
