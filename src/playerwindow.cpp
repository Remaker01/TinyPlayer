#include "playerwindow.h"
#define CHANGE_TO_PLAYICON ui->playButton->changeState("开始",PLAY_ICON)
#define CHANGE_TO_PAUSEICON ui->playButton->changeState("暂停",PAUSE_ICON)
#define RESET_LABEL ui->mediaLabel->setText(player->getMediaDetail().getTitle())
const QString PlayerWindow::CONFIG_FILE = "player.config";
static const QLatin1Char zero('0');
static const QString LAST_PATH = "/CONFIG/PATH",LAST_VOL = "/CONFIG/VOLUME",LAST_MODE = "/CONFIG/MODE";
PlayerWindow::PlayerWindow(const QString &arg,QWidget *parent):
    PLAY_ICON(":/Icons/images/play.png"),PAUSE_ICON(":/Icons/images/pause.png"),QMainWindow(parent), ui(new Ui::PlayerWindow) {
    ui->setupUi(this);
    player = new PlayerCore(this);
    settingWind = new SettingWindow(this);
    scher = new OnlineSeacher(this);
    res = new SearchResultWidget(this);
    initUi();
    initConfiguration();
    connectSlots();
    if(!arg.isEmpty()) {
        showNormal();  //提前显示，避免列表过大导致加载缓慢
        if(Music::isLegal(arg)) {
            doAddMedia(QStringList(arg));
            ui->playButton->click();
        }
        else if(!openList(arg))
            QMessageBox::warning(this,"警告","参数错误:不是合法的音频格式或播放列表");
    }
    else if(settingWind->getAutoSave()) {
        showNormal();
        openList("default.lst");
    }
}

inline void PlayerWindow::initUi() {
    ui->volumeButton->setReplyClick(true);
    ui->modeButton->setReplyClick(true);
    ui->searchLabel->setReplyClick(true);
    ui->mupbutton->setReplyClick(true);
    ui->mdownButton->setReplyClick(true);
    ui->playView->setOpacity(0.6);
    initSystemtray();
    setBackground(QPixmap(":/Icons/images/back.jpg"));
    ui->waitingLabel->hide();
    ui->cancelButton->hide();
    ui->toolBar->setMovable(false);
    setToolBar(settingWind->opacity);
    QList<QAction*> actions{ui->actionLoadList,ui->actionOpenDir,ui->actionSaveList,ui->actionToDefault,ui->actionOpenHelp};
    for (QAction *action:actions) {
        ui->toolBar->widgetForAction(action)->setStyleSheet("padding:6px 0px;");
        ui->toolBar->widgetForAction(action)->setToolTip("");
    }
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

inline void PlayerWindow::setToolBar(double opacity) {
    int val = std::min(255,qRound(255*opacity));
    QString color_st = "rgba(255,255,255," + QString::number(val) + "), ",color_end = "rgba(190,190,190," + QString::number(val) + ") ";
    ui->toolBar->setStyleSheet("QToolBar {"
                               "background-color: qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 " + color_st +
                               "stop:1 " + color_end + ");border:none;}"
                               );
}

inline void PlayerWindow::setBackground(const QPixmap &img) {
    constexpr int MIN_HEIGHT = 100,MIN_WIDTH = 200;
    if(img.height() < MIN_HEIGHT||img.width() < MIN_WIDTH) {
        QMessageBox::warning(this,"警告",QString("图片尺寸不能小于%1x%2像素").arg(MIN_WIDTH).arg(MIN_HEIGHT));
        return;
    }
    QPalette pl;
    int w = std::max(maximumWidth(),img.width()),h = std::max(maximumHeight(),img.height());
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
        setting.setValue("AUTOLOAD",settingWind->autoSave);
        setting.setValue("MINONCLOSE",settingWind->minOnClose);
    }
    lastPath = setting.value(LAST_PATH).toString();
    ui->volumeSlider->setValue(setting.value(LAST_VOL).toInt());
    settingWind->setAutoSave(setting.value("AUTOLOAD").toBool());
    settingWind->setminOnClose(setting.value("MINONCLOSE").toBool());
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
         ui->stopButton->setReplyClick(true);
         if(player->state() != Vlc::Playing) {
             player->play();
             if(player->state() == Vlc::Playing)
                CHANGE_TO_PAUSEICON;
         }
         else {
             player->pause();
             CHANGE_TO_PLAYICON;
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
        ui->nextButton->setReplyClick(true);
        ui->prevButton->setReplyClick(true);
        totTime = qRound(totTime / 1000.0);
        //改变总时间
        ui->timeLable->setText(QString("/%1:%2").arg(totTime / 60,2,10,zero).arg(totTime % 60,2,10,zero));
        ui->progressSlider->setMaximum(totTime);
        RESET_LABEL;
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
    connect(scher,&OnlineSeacher::done,this,&PlayerWindow::on_onlineSearcher_done);
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
                                                        "版本号:3.0  3.0.220816");
        box.addButton("确定",QMessageBox::AcceptRole);
        QPushButton *addr = box.addButton("项目地址",QMessageBox::NoRole);
        connect(addr,&QPushButton::clicked,this,[]{
            QDesktopServices::openUrl(QUrl("https://github.com/Remaker01/TinyPlayer"));
        });
        box.exec();
    });
    connect(ui->actionopenFile,&QAction::triggered,this,[this]() {
        QStringList medias(QFileDialog::getOpenFileNames(this,"选择文件(将自动筛选合法格式，支持格式请查看帮助)",lastPath));
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
        if(!tmp.isEmpty()) {
            if(!openList(tmp))
                QMessageBox::warning(this,"警告","播放列表加载失败");
        }
    });
    connect(ui->volumeButton,&PlayerButton::clicked,this,[this]() {
        const QPixmap muted(":/Icons/images/muted.png"),unMuted(":/Icons/images/volume.png");
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
        QFileInfo tmp(player->getMedia(row).toLocalFile());
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
    connect(ui->actionSet,&QAction::triggered,settingWind,&QWidget::show);
    connect(ui->searchLabel,&PlayerButton::clicked,this,[this]() {
        if(!QFile::exists(OnlineSeacher::PROGRAM)) {
            QMessageBox::critical(this,"错误","找不到执行搜索需要的程序");
            return;
        }
        if(res->isVisible()) {
            QMessageBox::warning(this,"警告","请关闭搜索结果窗口后进行新一次搜索");
            return;
        }
        static QMovie gif(":/Icons/images/waiting.gif");
        gif.setScaledSize(ui->searchLabel->size());
        ui->searchLabel->setMovie(&gif);
        gif.start();
        scher->setKeyWord(ui->searchEdit->text());
        scher->doSearch();
        ui->searchLabel->setReplyClick(false);
        connect(scher,&OnlineSeacher::done,&gif,&QMovie::stop);
        ui->searchEdit->clearFocus();
    });
    connect(settingWind,&SettingWindow::changeEffectRequirement,this,[this](int i) {
        static const uint map[] = {0u,1u,4u,5u,7u,11u,13u,16u};
        player->setSoundEffect(map[i]);
    });
    connect(res,&SearchResultWidget::addItemRequirement,this,[this](bool autoDel) {
        doAddOnlineMedia(res->getSelectedItems());
        if(autoDel)
            res->removeSelected();
    });
    connect(ui->mupbutton,&PlayerButton::clicked,this,[this]() {
        moveItem(true);
    });
    connect(ui->mdownButton,&PlayerButton::clicked,this,[this]() {
        moveItem(false);
    });
    connect(settingWind,&SettingWindow::changeOpacityRequirement,this,&PlayerWindow::setToolBar);
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
    if(e->key() == Qt::Key_Space&&!ui->searchEdit->hasFocus()) {
        e->accept();
        ui->playButton->click();
    }
}

void PlayerWindow::closeEvent(QCloseEvent *ev) {
    static bool first = true;
    if(settingWind->getminOnClose()) {
        ev->ignore();
        if(first)
            tray->showMessage("提示","TinyPlayer在后台运行");
        settingWind->close();
        hide();
        first = false;
    }
}
SLOTS
#define BEFORE_ADD  static bool f = true;\
    ui->waitingLabel->show();\
    ui->cancelButton->show();\
    connect(ui->cancelButton,&QPushButton::clicked,[&]{f = false;});\
    QStringList &playList = ui->playView->list();
#define AFTER_ADD   ui->playView->commitChange();\
    if(playList.size() > 0)\
        ui->delButton->setEnabled(true);\
    if(player->getCurrentMediaIndex() < 0)\
        player->setCurrentMediaIndex(0);\
    f = true;\
    ui->curlistLabel->setText("当前播放列表 共" + QString::number(playList.size()) + "项");\
    ui->cancelButton->hide();\
    ui->waitingLabel->hide();
void PlayerWindow::doAddMedia(QStringList medias) {
    if(medias.isEmpty())
        return;
    BEFORE_ADD
    for(QString &fullName:medias) {
        if(!f)
            break;
        if(!fullName.startsWith("http",Qt::CaseInsensitive)) {  //本地
            QFileInfo a(fullName);
            ui->waitingLabel->setText("正在打开" + a.fileName());
            if(player->addToList(fullName)) {
                playList.append(a.fileName() + '\n' + Music(QUrl::fromLocalFile(fullName)).formatTime());
            }
            lastPath = a.absolutePath();
        }
    }
    AFTER_ADD
}

void PlayerWindow::doAddOnlineMedia(const QList<ResultInfo> &medias) {
    if(medias.isEmpty())
        return;
    BEFORE_ADD
    for(const ResultInfo &item:medias) {
        if(!f)
            break;
        ui->waitingLabel->setText("正在插入" + item.title);
        if(player->addToList(item.url,false))
            playList.append(item.title + " - " + item.artist + "\n[线上音乐]");
    }
    AFTER_ADD
}
#undef BEFORE_ADD
#undef AFTER_ADD
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

void PlayerWindow::on_progressSlider_sliderMoved(int position) {player->setPos(position);}

#define AFTER_DEL(RPYCLICK_CONDITION) ui->playButton->setReplyClick(RPYCLICK_CONDITION);\
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
    QStringList &playList = ui->playView->list();
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
    bool f = (playList.size() > 0);
    ui->curlistLabel->setText("当前播放列表 共" + QString::number(playList.size()) + "项");
    AFTER_DEL(f)
}

void PlayerWindow::on_clearButton_clicked() {
    ui->playView->list().clear();
    ui->playView->commitChange();
    player->clear();
    ui->curlistLabel->setText("当前播放列表 共0项");
    AFTER_DEL(false)
}
#undef AFTER_DEL
void PlayerWindow::on_addButton_clicked() {
    ui->actionopenFile->trigger();
}

void PlayerWindow::on_onlineSearcher_done() {
    if(QFile("links.tmp").open(QIODevice::ReadOnly)) {
        res->setItems(scher->analyzeResult());
        res->show();
        ui->searchLabel->setPixmap(QPixmap(":/Icons/images/serach.png"));
    }
    ui->searchLabel->setReplyClick(true);
}
static constexpr uint16_t MAGIC = (uint16_t)0x0102;
bool PlayerWindow::saveList(const QString &file) {
    QFile lstFile(file);
    if(!lstFile.open(QIODevice::ReadWrite|QIODevice::Truncate))
        return false;
    const QStringList &list = ui->playView->list();
    QDataStream ds(&lstFile);
    ds.setVersion(QDataStream::Qt_5_2);
    ds << MAGIC; //magic number
    for(int i = 0; i < list.size(); i++) {
        if(!list[i].contains("[线上音乐]"))
            ds << player->getMedia(i).toLocalFile();
    }
    lstFile.close();
    return true;
}

bool PlayerWindow::openList(const QString &file) {
    QFile lstFile(file);
    if(!lstFile.open(QIODevice::ReadOnly))
        return false;
    QDataStream ds(&lstFile);
    ds.setVersion(QDataStream::Qt_5_2);
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

void PlayerWindow::on_playView_doubleClicked(const QModelIndex &index) {
    player->setCurrentMediaIndex(index.row());
}

void PlayerWindow::moveItem(bool moveUp) {
    QModelIndexList selected = ui->playView->getSelections();
    if(selected.isEmpty()||selected.size() > 1)
        return;
    int row = selected[0].row();
    if(!moveUp) {
        if(player->moveDown(row))
            ui->playView->list().move(row,row + 1);
    }
    else {
        if(player->moveUp(row))
            ui->playView->list().move(row,row - 1);
    }
    RESET_LABEL;
    ui->playView->commitChange();
}

PlayerWindow::~PlayerWindow() {
    QSettings setting(CONFIG_FILE,QSettings::IniFormat);
    setting.setValue(LAST_PATH,lastPath);
    setting.setValue(LAST_VOL,ui->volumeSlider->value());
    setting.setValue(LAST_MODE,(int)player->mode);
    setting.setValue("AUTOLOAD",settingWind->autoSave);
    setting.setValue("MINONCLOSE",settingWind->minOnClose);
    saveList("default.lst");
    delete ui;
}
