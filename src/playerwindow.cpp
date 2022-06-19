/*
 * TODO: 利用QStackedWidget添加播放列表
 * 在退出时，需要先保存播放列表的列表(listsView)，再分别保存所有播放列表
 * 可先创建playLists文件夹，将播放列表都保存进文件夹中
 */
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
    openList("default.lst");
}

inline void PlayerWindow::initUi() {
    ui->volumeButton->setReplyClick(true);
    ui->modeButton->setReplyClick(true);
    ui->delButton->setEnabled(false);
    ui->playView->setOpacity(0.6);
    initSystemtray();
    setBackground(QPixmap(":/Icons/images/back.jpg"));
    ui->waitingLabel->hide();
    ui->cancelButton->hide();
}

inline void PlayerWindow::initSystemtray() {
    tray = new QSystemTrayIcon(QIcon(":/Icons/images/icon.ico"),this);
    tray->setToolTip("TinyPlayer");
    trayMenu = new QMenu(this);
    trayMenu->addAction(QIcon(":/Icons/images/icon.ico"),"打开窗口",this,&QMainWindow::showNormal);
    trayMenu->addAction(QIcon(":/Icons/images/exit.png"),"退出",qApp,&QApplication::quit);
    tray->setContextMenu(trayMenu);
    tray->show();
}

inline void PlayerWindow::setBackground(const QPixmap &img) {
    constexpr int MIN_HEIGHT = 100,MIN_WIDTH = 200;
    if(img.height() < MIN_HEIGHT||img.width() < MIN_WIDTH) {
        QMessageBox::warning(this,"警告",QString("图片尺寸不能小于%1x%2像素").arg(MIN_WIDTH).arg(MIN_HEIGHT));
        return;
    }
    QPalette pl;
    int w = std::max(width(),img.width()),h = std::max(height(),img.height());
    pl.setBrush(backgroundRole(),QBrush(img.scaled(w,h)));
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
    QString name = ":/Icons/images/" + QString::number((int)m) + ".png";
    ui->modeButton->setPixmap(QPixmap(name));
    player->mode = m;
    ui->modeButton->setToolTip(PlayerCore::MODE_TIPS[(int)m]);
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
                 player->pause();
                 CHANGE_TO_PLAYICON;
             }
         }
         else
             QMessageBox::critical(this,"播放失败","文件可能已损坏");
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
        ui->nextButton->setReplyClick(true);
        ui->prevButton->setReplyClick(true);
        totTime = qRound(totTime / 1000.0);
        //改变总时间
        ui->timeLable->setText(QString("/%1:%2").arg(totTime / 60,2,10,zero).arg(totTime % 60,2,10,zero));
        ui->progressSlider->setMaximum(totTime);
        ui->mediaLabel->setText(QString::number(1+player->getCurrentMediaIndex()) + " - " + player->getMedia().fileName());
        //重置进度条（好像不加也可以？）
        ui->progressSlider->setValue(0);
        //改变专辑图片
        QString albumPic = player->getMediaDetail().getAlbumImage().toLocalFile();
        if(albumPic.isEmpty())
            ui->albumLabel->setPixmap(QPixmap(":/Icons/images/non-music.png").scaled(150,150));
        else
            ui->albumLabel->setPixmap(QPixmap(albumPic).scaled(150,150));
    });
    connect(player,&PlayerCore::timeChanged,this,[this](int t) {
        ui->progressSlider->setValue(qRound(t / 1000.0));
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
    connect(ui->playView,&PlayListView::mediaDropin,this,&PlayerWindow::doAddMedia);
    connect(ui->delButton,&QPushButton::clicked,this,&PlayerWindow::doDelMedia);
    connect(ui->playView,&PlayListView::itemDelRequirement,this,&PlayerWindow::doDelMedia);
    connect(ui->progressSlider,&PlayerSlider::playerSliderClicked,player,&PlayerCore::setPos);
    connect(ui->actionAbout,&QAction::triggered,this,[this] {
        QMessageBox box(QMessageBox::Information,"关于","TinyPlayer播放器\n"
                                                        "基于Qt的简易音频播放器\n\n"
                                                        "环境:QT5.12+QT Creator5+CMake3.21+MinGW8.1\n"
                                                        "作者邮箱:latexreal@163.com\n"
                                                        "版本号:2.10 Beta  2.10.220619");
        box.addButton("确定",QMessageBox::AcceptRole);
        box.addButton("项目地址",QMessageBox::RejectRole);
        connect(&box,&QMessageBox::rejected,this,[]{
            QDesktopServices::openUrl(QUrl("https://github.com/Remaker01/TinyPlayer"));
        });
        box.exec();
    });
    connect(ui->actionopenFile,&QAction::triggered,this,[this]() {
        QStringList medias(QFileDialog::getOpenFileNames(this,"选择文件(将自动筛选合法格式，支持格式请查看帮助)",lastPath));
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
        if(!back.isEmpty())
            setBackground(QPixmap(back));
    });
    connect(ui->actionToDefault,&QAction::triggered,this,[this](){
        setBackground(QPixmap(":/Icons/images/back.jpg"));
    });
    connect(ui->actionSaveList,&QAction::triggered,this,[this]() {
        QString tmp = QFileDialog::getSaveFileName(this,"选择保存目录",lastPath,"播放列表(*.lst)");
        if(tmp.isEmpty())   return;
        if(!saveList(tmp))
            QMessageBox::warning(this,"警告","无法保存，请检查文件路径和文件名");
    });
    connect(ui->actionLoadList,&QAction::triggered,this,[this]() {
        QString tmp = QFileDialog::getOpenFileName(this,"选择播放列表",lastPath,"播放列表(*.lst)");
        if(!tmp.isEmpty())
            openList(tmp);
    });
    connect(ui->volumeButton,&PlayerButton::clicked,this,[this]() {
        static const QPixmap muted(":/Icons/images/muted.png"),unMuted(":/Icons/images/volume.png");
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
    connect(ui->playView,&PlayListView::openDirRequirement,this,[this](int row){
        QFileInfo tmp(player->getMedia().toLocalFile());
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
    connect(ui->playView,&PlayListView::showDetailRequirement,this,[this](int row) {
        QMessageBox::information(this,"信息",player->getMediaDetail(row).toString());
    });
    connect(ui->nextButton,&PlayerButton::clicked,player,&PlayerCore::goNext);
    connect(ui->prevButton,&PlayerButton::clicked,player,&PlayerCore::goPrevious);
}

inline void PlayerWindow::ensureExit() {
    QMessageBox box(QMessageBox::Question,"退出确认","确认退出?");
    box.addButton("最小化",QMessageBox::AcceptRole);
    QPushButton *yes = box.addButton("是",QMessageBox::NoRole);
    box.addButton("否",QMessageBox::RejectRole);
    connect(yes,&QPushButton::clicked,qApp,&QApplication::quit);
    connect(&box,&QMessageBox::accepted,this,&QMainWindow::hide);
    box.exec();
}

void PlayerWindow::keyReleaseEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Space) {
        e->accept();
        ui->playButton->click();
    }
}
SLOTS
void PlayerWindow::doAddMedia(QStringList medias) {
    if(medias.isEmpty())
        return;
    static bool f = true;
    ui->waitingLabel->show();
    ui->cancelButton->show();
    connect(ui->cancelButton,&QPushButton::clicked,[&]{f = false;});
    QStringList &playList = ui->playView->currentList();
    for(QString &fullName:medias) {
        if(!f)
            break;
        QFileInfo a(fullName);
        ui->waitingLabel->setText("正在打开" + a.fileName());
        if(player->addToList(fullName))
            playList.append(a.fileName() + '\n' + Music::getMediaDetail(fullName).formatTime());
    }
    lastPath = QFileInfo(medias.last()).absolutePath();
    ui->playView->commitChange();
    if(playList.size() > 0)
        ui->delButton->setEnabled(true);
    if(player->getCurrentMediaIndex() < 0)
        player->setCurrentMediaIndex(0);
    f = true;
    ui->cancelButton->hide();
    ui->waitingLabel->hide();

}

void PlayerWindow::on_volumeSlider_valueChanged(int value) {
    if(value > 0)
        ui->volumeButton->setPixmap(QPixmap(":/Icons/images/volume.png"));
    else
        ui->volumeButton->setPixmap(QPixmap(":/Icons/images/muted.png"));
    player->audio()->setVolume(value);
    ui->volLabel->setText(QString("音量：%1").arg(value,2) + ' ');
}

void PlayerWindow::on_progressSlider_valueChanged(int value) {
    ui->currentLabel->setText(QString("%1:%2").arg(value / 60,2,10,zero).arg(value % 60,2,10,zero));
}

void PlayerWindow::on_progressSlider_sliderMoved(int position) {
    player->setPos(position);
}

void PlayerWindow::on_playView_doubleClicked(const QModelIndex &index) {
    player->setCurrentMediaIndex(index.row());
}
#define LIST_DEL_ACTION(RPYCLICK_CONDITION) ui->playButton->setReplyClick(RPYCLICK_CONDITION);\
    ui->delButton->setEnabled(RPYCLICK_CONDITION);\
    ui->prevButton->setReplyClick(RPYCLICK_CONDITION);\
    ui->nextButton->setReplyClick(RPYCLICK_CONDITION);\
    if(!(RPYCLICK_CONDITION)) {\
        ui->timeLable->setText("/00:00");\
        ui->mediaLabel->clear();\
}
void PlayerWindow::doDelMedia() {
    QModelIndexList tmp = ui->playView->getSelections();
    QList<int> selections;
    QStringList &playList = ui->playView->currentList();
    for (QModelIndex &i:tmp)
        selections.append(i.row());
    std::sort(selections.begin(),selections.end(),std::greater<int>());
    for (int i:selections) {
        if(!player->removeFromList(i))
            QMessageBox::critical(this,"错误","删除失败。");
        else
            playList.removeAt(i);
    }
    ui->playView->commitChange();
    ui->mediaLabel->setText(QString::number(1+player->getCurrentMediaIndex()) + " - " + player->getMedia().fileName());
    bool f = (playList.size() > 0);
    LIST_DEL_ACTION(f)
}

void PlayerWindow::on_clearButton_clicked() {
    ui->playView->currentList().clear();
    ui->playView->commitChange();
    player->clear();
    LIST_DEL_ACTION(false)
}
#undef LIST_DEL_ACTION
void PlayerWindow::on_addButton_clicked() {
    ui->actionopenFile->trigger();
}
static constexpr uint16_t MAGIC = (uint16_t)0x0102;
bool PlayerWindow::saveList(const QString &file) {
    QFile lstFile(file);
    if(!lstFile.open(QIODevice::ReadWrite|QIODevice::Truncate))
        return false;
    int tot = ui->playView->currentList().size();
    QDataStream ds(&lstFile);
    ds.setVersion(QDataStream::Qt_5_0);
    ds << MAGIC; //magic number
    for(int i = 0; i < tot; i++)
        ds << player->getMedia(i).toLocalFile();
    lstFile.close();
    return true;
}

bool PlayerWindow::openList(const QString &file) {
    QFile lstFile(file);
    if(!lstFile.open(QIODevice::ReadOnly))
        return false;
    QDataStream ds(&lstFile);
    ds.setVersion(QDataStream::Qt_5_0);
    uint16_t magic;
    ds >> magic;
    if(magic != MAGIC)
        return false;
    QStringList tmp;
    QString str;
    while (!ds.atEnd()) {
        ds >> str;
        tmp.append(str);
    }
    doAddMedia(tmp);
    lstFile.close();
    return true;
}

PlayerWindow::~PlayerWindow() {
    QSettings setting(CONFIG_FILE,QSettings::IniFormat);
    setting.setValue(LAST_PATH,lastPath);
    setting.setValue(LAST_VOL,ui->volumeSlider->value());
    setting.setValue(LAST_MODE,(int)player->mode);
    saveList("default.lst");
    delete ui;
}
