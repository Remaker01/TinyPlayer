#include "playerwindow.h"
#define CHANGE_TO_PLAYICON ui->playButton->changeState("开始",PLAY_ICON)
#define CHANGE_TO_PAUSEICON ui->playButton->changeState("暂停",PAUSE_ICON)
#define RESET_LABEL ui->mediaLabel->setText(player->getMediaDetail().getTitle())
const QString PlayerWindow::CONFIG_FILE("player.config");
#define Zero QChar('0')
static const QString LAST_PATH("/CONFIG/PATH"),LAST_VOLUME("/CONFIG/VOLUME"),LAST_MODE("/CONFIG/MODE");
PlayerWindow::PlayerWindow(const QString &arg,QWidget *parent):
    PLAY_ICON(":/Icons/images/play.png"),PAUSE_ICON(":/Icons/images/pause.png"),FramelessWindow(parent), ui(new Ui::PlayerWindow) {
    ui->setupUi(this);
    player = new PlayerCore(this);
    settingWind = new SettingWindow;
    res = new SearchResultWidget(this);
    scher = new OnlineSeacher(this);
    initUi();
    initConfiguration();
    connectSlots();
    showNormal();
    //逻辑：如果打开的是音频文件，则直接播放然后打开default.lst，否则，只打开arg指定的播放列表，无论是否成功都不打开default.lst
    if(!arg.isEmpty()) {
        if(Music::isLegal(arg)) {
            doAddLocalMedias(QStringList(arg));
            ui->playButton->click();
            if(settingWind->getAutoSave())
                openList("default.lst");
        }
        else if(!openList(arg))
            QMessageBox::warning(this,"提示","您打开的文件不是合法的音频格式或播放列表");
    }
    else if(settingWind->getAutoSave()) {
        openList("default.lst");
    }
}

inline void PlayerWindow::initUi() {
    ui->volumeButton->setClickable(true);
    ui->modeButton->setClickable(true);
    ui->searchLabel->setClickable(true);
    ui->mupbutton->setClickable(true);
    ui->mdownButton->setClickable(true);
    ui->logoButton->setClickable(true);
    static QAction sep1,sep2,sep3;
    sep1.setSeparator(true),sep2.setSeparator(true),sep3.setSeparator(true);
    setMenu({ui->actionopenFile,ui->actionOpenDir,&sep1,
                      ui->actionLoadList,ui->actionSaveList,&sep2,
                      ui->actionLoadImg,ui->actionToDefault,&sep3,
                      ui->actionOpenHelp,ui->actionAbout,ui->actionExit});
    setCloseButton(ui->quitButton);
    setMinimizeButton(ui->minButton);
    initSystemtray();
    setBackground(QPixmap(":/Icons/images/back.jpg"));
    ui->waitingLabel->hide();
    ui->cancelButton->hide();
    ui->comboBox->setView(new QListView());
}

inline void PlayerWindow::initSystemtray() {
    tray = new QSystemTrayIcon(QIcon(":/Icons/images/icon.png"),this);
    tray->setToolTip("TinyPlayer");
    trayMenu = new QMenu(this);
    trayMenu->addAction(QIcon(":/Icons/images/icon.png"),"打开窗口",this,&QWidget::showNormal);
    trayMenu->addAction(QIcon(),"播放/暂停",ui->playButton,&PlayerButton::click);
    trayMenu->addAction(QIcon(":/Icons/images/exit.png"),"退出",qApp,&QApplication::quit); //不直接使用ui->actionExit使退出时无需确认
    tray->setContextMenu(trayMenu);
    tray->show();
}

inline void PlayerWindow::setTitlebar(double opacity) {
    int val = std::min(255,qRound(255*opacity));
    QString color_st = "rgba(245,245,255," + QString::number(val) + "), ",color_end = "rgba(185,185,195," + QString::number(val) + ") ";
    ui->topWidget->setStyleSheet(QStringLiteral("QWidget#topWidget {"  // 因可能频繁调用，将较长的字符串常量先初始化
                               "background-color: qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1, stop:0 ") + color_st +
                               "stop:1 " + color_end + QStringLiteral(");"
                               "border:none;"
                               "}"
                               "QWidget {font-family:\"") + font().family() + "\";}" //防止字体失效
                               );
}

inline void PlayerWindow::setBackground(const QPixmap &img) {
    constexpr int MIN_HEIGHT = 100,MIN_WIDTH = 200;
    if(img.height() < MIN_HEIGHT||img.width() < MIN_WIDTH) {
        QMessageBox::warning(this,"提示",QString("图片尺寸不能小于%1x%2像素").arg(MIN_WIDTH).arg(MIN_HEIGHT));
        return;
    }
    QPalette pl;
    int w = std::max(maximumWidth(),img.width()/2),h = std::max(maximumHeight(),img.height()/2);
    pl.setBrush(backgroundRole(),QBrush(img.scaled(w,h)));
    setPalette(pl);
}

inline void PlayerWindow::initConfiguration() {
    QSettings setting(CONFIG_FILE,QSettings::IniFormat);
    if(!QFile::exists(CONFIG_FILE)) {
		setting.setValue(LAST_PATH,QCoreApplication::applicationDirPath());
		setting.setValue(LAST_VOLUME,50);
		setting.setValue(LAST_MODE,0);
    }
    lastPath = setting.value(LAST_PATH).toString();
    ui->volumeSlider->setValue(setting.value(LAST_VOLUME).toInt());
    settingWind->loadSettings(setting);
	setTitlebar(setting.value("TOOLBAR_OPAC").toDouble()); //如设置文件正常则两行互换位置也正确，但如不正常，互换后则会异常退出
    changeMode((PlayerCore::PlayMode)setting.value(LAST_MODE).toInt());
}

inline void PlayerWindow::changeMode(PlayerCore::PlayMode m) {
    QString name = ":/Icons/images/" + QString::number((int)m) + ".png";
    player->mode = m;
    ui->modeButton->changeState(PlayerCore::MODE_TIPS[(int)m],QPixmap(name));
}

inline void PlayerWindow::connectSlots() {
    connectUiSlots();
    connect(ui->playButton,&PlayerButton::clicked,this,[this](){
         if(player->state() != Vlc::Playing) {
             player->play();
         }
         else {
             player->pause();
         }
    });
    connect(ui->stopButton,&PlayerButton::clicked,this,[this]() {
        if(player->state() != Vlc::Stopped) {
           player->stop();
           CHANGE_TO_PLAYICON;
           ui->stopButton->setClickable(false);
           ui->progressSlider->setValue(0);
        }
    });
    connect(player,&PlayerCore::lengthChanged,this,[&,this](int totTime) {
        ui->playButton->setClickable(true);
        ui->nextButton->setClickable(true);
        ui->prevButton->setClickable(true);
        totTime = qRound(totTime / 1000.0);
        //改变总时间
        ui->timeLable->setText(QString("/%1:%2").arg(totTime / 60,2,10,Zero).arg(totTime % 60,2,10,Zero));
        ui->progressSlider->setMaximum(totTime);
        RESET_LABEL;
        //改变专辑图片
        Music curMusic = player->getMediaDetail();
        QString albumPic = curMusic.getAlbumImage().toLocalFile();
        QFile picFile(albumPic);
        if(curMusic.isOnlineMusic()||!picFile.open(QIODevice::ReadOnly))
            ui->albumLabel->setPixmap(QPixmap(":/Icons/images/non-music.png").scaled(150,150));
        else {
            QPixmap pixmap;pixmap.loadFromData(picFile.read(1 << 25)); //防止1.个别图片内容与后缀不匹配2.读取过大文件导致崩溃
            if(pixmap.isNull()||pixmap.width()*pixmap.height()<100)
                ui->albumLabel->setPixmap(QPixmap(":/Icons/images/non-music.png").scaled(150,150));
            else
                ui->albumLabel->setPixmap(pixmap.scaled(150,150));
            picFile.close();
        }
    });
    connect(player,&PlayerCore::timeChanged,this,[this](int t) {
        ui->progressSlider->setValue(qRound(t / 1000.0f));
    });
    //停止后重置进度条
    connect(player,&PlayerCore::finished,this,[&,this]() {
        ui->progressSlider->setValue(0);
        CHANGE_TO_PLAYICON;
        ui->stopButton->setClickable(false);
    });
    connect(player,&VlcMediaPlayer::opening,this,[this] {
        ui->waitingLabel->setText("正在缓冲");
        ui->waitingLabel->show();
    });
    connect(player,&VlcMediaPlayer::playing,this,[this] {
        ui->stopButton->setClickable(true);
        CHANGE_TO_PAUSEICON;
        ui->waitingLabel->hide();
    });
    connect(player,&VlcMediaPlayer::paused,this,[this] {CHANGE_TO_PLAYICON;});
    connect(player,&VlcMediaPlayer::error,ui->waitingLabel,&QWidget::hide);  //播放出错也要隐藏
    connect(tray,&QSystemTrayIcon::activated,this,[this](QSystemTrayIcon::ActivationReason r){
        if(r == QSystemTrayIcon::Trigger)
            showNormal();
    });
    connect(scher,&OnlineSeacher::done,this,&PlayerWindow::on_onlineSearcher_done);
    connect(scher,&OnlineSeacher::downloaded,this,[this]{
        QDesktopServices::openUrl(QUrl::fromLocalFile(settingWind->getDownLoc()));
        ui->waitingLabel->hide();
    });
    connect(res,&SearchResultWidget::changPageRequirement,this,&PlayerWindow::searchForPage);
}

inline void PlayerWindow::connectUiSlots() {
    connect(ui->actionExit,&QAction::triggered,this,&PlayerWindow::ensureExit);
    connect(ui->playView,&PlayListView::mediaDropin,this,[this](const QStringList &medias) {doAddLocalMedias(medias);});
    connect(ui->delButton,&QPushButton::clicked,this,&PlayerWindow::doDelMedia);
    connect(ui->playView,&PlayListView::itemDelRequirement,this,&PlayerWindow::doDelMedia);
    connect(ui->progressSlider,&PlayerSlider::playerSliderClicked,player,&PlayerCore::setPos);
    connect(ui->actionAbout,&QAction::triggered,this,[this] {
        QMessageBox box(QMessageBox::Information,"关于","TinyPlayer播放器\n"
                                                        "基于Qt的简易音频播放器\n\n"
#ifndef NDEBUG
                                                        "环境:QT5.12+QT Creator5+CMake3.21+MinGW8.1\n"
#endif
                                                        "作者邮箱:latexreal@163.com\t\n"
														"版本号:3.40  3.40.231120");
        box.addButton("确定",QMessageBox::AcceptRole);
        QPushButton *addr = box.addButton("项目地址",QMessageBox::NoRole);
        connect(addr,&QPushButton::clicked,this,[]{
            QDesktopServices::openUrl(QUrl("https://github.com/Remaker01/TinyPlayer"));
        });
        box.exec();
    }); 
    connect(ui->actionopenFile,&QAction::triggered,this,[this]() {
        QStringList medias(QFileDialog::getOpenFileNames(this,"选择文件(将自动筛选合法格式，支持格式请查看帮助)",lastPath));
        doAddLocalMedias(medias);
    });
    connect(ui->modeButton,&PlayerButton::clicked,this,[this]() {
        int now = (int)player->mode;
        changeMode((PlayerCore::PlayMode)((now + 1) % PlayerCore::MODE_COUNT));
    });
    connect(ui->actionOpenHelp,&QAction::triggered,this,[]() {
        return QDesktopServices::openUrl(QUrl::fromLocalFile("README.htm"));
    });
    connect(ui->actionLoadImg,&QAction::triggered,this,[this]() {
        QString back = QFileDialog::getOpenFileName(this,"选择文件","","图片文件(*.jpg;*.jpeg;*.png;*.jfif;*.bmp;*.tif;*.tiff)");
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
            QMessageBox::warning(this,"提示","无法保存，请检查文件路径和文件名");
    });
    connect(ui->actionLoadList,&QAction::triggered,this,[this]() {
        QString tmp = QFileDialog::getOpenFileName(this,"选择播放列表",lastPath,"播放列表(*.lst)");
        if(!tmp.isEmpty()) {
            if(!openList(tmp))
                QMessageBox::warning(this,"提示","播放列表加载失败");
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
                emit ui->volumeSlider->valueChanged(v); // 虽然音量改变，但数值不变
            }
        }
    });
    connect(ui->playView,&PlayListView::openRequirement,this,[this](int row){
        QFileInfo tmp(player->getMedia(row).toLocalFile());
        QDesktopServices::openUrl(QUrl::fromLocalFile(tmp.absolutePath()));
    });
    connect(ui->playView,&PlayListView::downloadRequirement,this,[this](const QModelIndexList &indexes) {
        if(indexes.size() > 15) {
            QMessageBox::warning(this,"提示","一次最多下载15首哦~");
            return;
        }
        QStringList names,list = ui->playView->list();
        QList<QUrl> urlsToDown;
        for (const QModelIndex &i:indexes) {
            int j = i.row();
            QString name = list.at(j);
            names.append(name.left(name.indexOf('\n')));
            urlsToDown.append(player->getMedia(j));
        }
        ui->waitingLabel->setText("正在下载");
        ui->waitingLabel->show();
        scher->download(urlsToDown,settingWind->getDownLoc(),names);
    });
    connect(ui->actionOpenDir,&QAction::triggered,this,[this]() {
        QString s = QFileDialog::getExistingDirectory(this,"选择文件夹",lastPath);
        if(s.isEmpty())
            return;
        QDir dir(s);
        QStringList files = dir.entryList();
        for (QString &str : files)
            str = dir.absolutePath() + '/' + str;
        doAddLocalMedias(files);
    });
    connect(ui->playView,&PlayListView::showDetailRequirement,this,[this](int row) {
        Music detail = player->getMediaDetail(row);
        QMessageBox::information(this,detail.getTitle() + " 详细信息",detail.toString());
    });
    connect(ui->nextButton,&PlayerButton::clicked,player,&PlayerCore::goNext);
    connect(ui->prevButton,&PlayerButton::clicked,player,&PlayerCore::goPrevious);
    connect(ui->searchLabel,&PlayerButton::clicked,this,[this]() {
        if(res->isVisible()) {
            QMessageBox::warning(this,"提示","请关闭搜索结果窗口后进行新一次搜索");
            return;
        }
//        static QMovie gif(":/Icons/images/waiting.gif");  //282-285:设置gif
//        gif.setScaledSize(ui->searchLabel->size());
//        ui->searchLabel->setMovie(&gif);
//        gif.start();
        scher->setKeyWord( ui->searchEdit->text().replace(';',""));
        searchForPage(1u);
//        scher->doSearch(ui->comboBox->currentIndex());
//        ui->searchLabel->setClickable(false);
//        connect(scher,&OnlineSeacher::done,&gif,&QMovie::stop);
//        ui->searchEdit->clearFocus();
    });
    connect(settingWind,&SettingWindow::changeEffectRequirement,this,[this](int i) {
        player->setSoundEffect((uint)i);
    });
    connect(res,&SearchResultWidget::addItemRequirement,this,[this](bool autoDel) {
        doAddOnlineMedias(res->getSelectedItems());
        if(autoDel)
            res->removeSelected();
    });
    connect(ui->mupbutton,&PlayerButton::clicked,this,[this]() {
        moveItem(true);
    });
    connect(ui->mdownButton,&PlayerButton::clicked,this,[this]() {
        moveItem(false);
    });
    connect(settingWind,&SettingWindow::changeOpacityRequirement,this,&PlayerWindow::setTitlebar);
    connect(ui->logoButton,&PlayerButton::clicked,this,[this]() {
       showMenu(ui->logoButton->rect());
    });
    connect(ui->setButton,&QPushButton::clicked,settingWind,&QWidget::show);
}

inline void PlayerWindow::ensureExit() {
    QMessageBox box(QMessageBox::Question,"提示","确认退出吗?");
    box.addButton("最小化",QMessageBox::AcceptRole);
    QPushButton *yes = box.addButton("是",QMessageBox::NoRole);
    box.addButton("否",QMessageBox::RejectRole);
    connect(yes,&QPushButton::clicked,qApp,&QApplication::quit);
    connect(&box,&QMessageBox::accepted,this,&QWidget::hide);
    box.exec();
}

inline void PlayerWindow::searchForPage(uint page) {
	static QMovie gif(":/Icons/images/waiting.gif");  //设置gif
    gif.setScaledSize(ui->searchLabel->size());
    ui->searchLabel->setMovie(&gif);
    gif.start();
    scher->doSearch(ui->comboBox->currentIndex(),page);
    ui->searchLabel->setClickable(false);
    connect(scher,&OnlineSeacher::done,&gif,&QMovie::stop);
    ui->searchEdit->clearFocus();
}
static constexpr uint16_t MAGIC = (uint16_t)0x35c4,MAGIC_OLD=(uint16_t)0x0102; //L=0x4c,S=0x53,T=0x54
//文件格式：MAGIC+偏移量（仅在打开前列表为空时有意义）+数据
inline bool PlayerWindow::saveList(const QString &file) {
	QFile lstFile(file);
	if(!lstFile.open(QIODevice::ReadWrite|QIODevice::Truncate))
		return false;
	const QStringList list = ui->playView->list();
	QDataStream ds(&lstFile);
	ds.setVersion(QDataStream::Qt_5_2);
	ds.setByteOrder(QDataStream::BigEndian);
	int index = player->getCurrentMediaIndex();
	if(index>=0)
		ds << MAGIC << (uint16_t)index; //magic number&index
	else
		ds << MAGIC << (uint16_t)0;
	for(int i = 0; i < list.size(); i++) {
		const QUrl &u = player->getMedia(i);
		if(u.isLocalFile())
			ds << u.url();
		else {
			const Music &detail = player->getMediaDetail(i);
			ds << u.url() << detail.getTitle() << detail.getArtist();
		}
	}
	lstFile.close();
	return true;
}

inline bool PlayerWindow::openList(const QString &file) {
	QFile lstFile(file);
	if(!lstFile.open(QIODevice::ReadOnly))
		return false;
	QDataStream ds(&lstFile);
	ds.setVersion(QDataStream::Qt_5_2);
	uint16_t magic,index;ds >> magic;
	if(magic == MAGIC_OLD) {
		openList_old(ds);return true;
	}
	if(magic != MAGIC)
		return false;
	ds >> index;
	int oldSize = ui->playView->list().size();
	QStringList tmp;
	QString str;
	while (!ds.atEnd()) {
		ds >> str;
		QUrl u(str);
		if(!u.isLocalFile()) {
			ResultInfo info;info.url = str;
			ds >> info.title >> info.artist;
			doAddOnlineMedia(info,false);
		}
		else {
			doAddLocalMedia(str,false);
		}
	}
	doAddLocalMedias(tmp,false);
	if(oldSize == 0)
		player->setCurrentMediaIndex(index);
	lstFile.close();
	return true;
}

inline void PlayerWindow::openList_old(QDataStream &ds) {
	QStringList tmp;
	QString str;
	while (!ds.atEnd()) {
		ds >> str;tmp.append(str);
	}
	doAddLocalMedias(tmp);
	ds.device()->close();
}

void PlayerWindow::keyReleaseEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Space&&!ui->searchEdit->hasFocus()) {
        e->accept();
        ui->playButton->click();
    }
    switch (e->key()) {
    case Qt::Key_Up:
        ui->playView->clearFocus();
        ui->volumeSlider->setValue(ui->volumeSlider->value()+5);break;
    case Qt::Key_Down:
        ui->playView->clearFocus();
        ui->volumeSlider->setValue(ui->volumeSlider->value()-5);break;
    case Qt::Key_Enter:case Qt::Key_Return:
        if(ui->searchEdit->hasFocus())
            ui->searchLabel->click();
    }
}

void PlayerWindow::closeEvent(QCloseEvent *ev) {
    static bool first = true;  //仅第一次最小化时显示提示信息
    if(settingWind->getminOnClose()) {
        ev->ignore();
        if(first)
            tray->showMessage("提示","TinyPlayer在后台运行");
        settingWind->close();
        hide();
        first = false;
    }
    else
        settingWind->close();
}
SLOTS
#define BEFORE_ADD  static bool f = true;\
    ui->waitingLabel->show();\
    ui->cancelButton->show();\
    connect(ui->cancelButton,&QPushButton::clicked,[&]{f = false;});\
	QStringList playList = ui->playView->list();
#define AFTER_ADD   ui->playView->setList(playList);\
    if(playList.size() > 0)\
        ui->delButton->setEnabled(true);\
    if(reset&&player->getCurrentMediaIndex() < 0)\
        player->setCurrentMediaIndex(0);\
    f = true;\
    ui->curlistLabel->setText("当前播放列表 共" + QString::number(playList.size()) + "项");\
    ui->cancelButton->hide();\
    ui->waitingLabel->hide();
void PlayerWindow::doAddLocalMedias(QStringList medias, bool reset) {
    if(medias.isEmpty())
        return;
    BEFORE_ADD
    for(QString &fullName:medias) {
        if(!f)
            break;
        fullName.replace("file:///",""); //以防万一
//        if(!fullName.startsWith("http",Qt::CaseInsensitive)) {  //本地
            QFileInfo a(fullName);
            ui->waitingLabel->setText("正在打开" + a.fileName());
            if(player->addToList(fullName))
                playList.append(a.fileName() + '\n' + Music(QUrl::fromLocalFile(fullName)).formatTime());
            lastPath = a.absolutePath();
//        }
    }
    AFTER_ADD
}

void PlayerWindow::doAddLocalMedia(QString media,bool reset) {
    BEFORE_ADD
    media.replace("file:///","");
    QFileInfo a(media);
    if(player->addToList(media))
        playList.append(a.fileName() + '\n' + Music(QUrl::fromLocalFile(media)).formatTime());
    ui->playView->setList(playList);
    AFTER_ADD
}

void PlayerWindow::doAddOnlineMedias(const QList<ResultInfo> &medias,bool reset) {
    if(medias.isEmpty())
        return;
    BEFORE_ADD
    for(const ResultInfo &item:medias) {
        if(!f)
            break;
        ui->waitingLabel->setText("正在插入" + item.title);
        if(player->addToList(item.url,false,item.title))
			playList.append(QString("%1 - %2\n[%3]").arg(item.title,item.artist,item.getSupplier()));
    }
    AFTER_ADD
}

void PlayerWindow::doAddOnlineMedia(const ResultInfo &media,bool reset) {
    BEFORE_ADD
    if(player->addToList(media.url,false,media.title))
        playList.append(QString("%1 - %2\n[%3]").arg(media.title,media.artist,media.getSupplier()));
    ui->playView->setList(playList);
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
    ui->currentLabel->setText(QString("%1:%2").arg(value / 60,2,10,Zero).arg(value % 60,2,10,Zero));
}

void PlayerWindow::on_progressSlider_sliderMoved(int position) {player->setPos(position);}

#define AFTER_DEL(RPYCLICK_CONDITION) ui->playButton->setClickable(RPYCLICK_CONDITION);\
    ui->delButton->setEnabled(RPYCLICK_CONDITION);\
    ui->prevButton->setClickable(RPYCLICK_CONDITION);\
    ui->nextButton->setClickable(RPYCLICK_CONDITION);\
    if(!(RPYCLICK_CONDITION)) {\
        ui->timeLable->setText("/00:00");\
        ui->mediaLabel->clear();\
}
void PlayerWindow::doDelMedia() {
    QModelIndexList tmp = ui->playView->getSelections();
    QList<int> selections;
	QStringList playList = ui->playView->list();
    for (QModelIndex &i:tmp)
        selections.append(i.row());
    std::sort(selections.begin(),selections.end(),std::greater<int>());
    for (int i:selections) {
        if(!player->removeFromList(i))
            QMessageBox::critical(this,"出错了!","删除失败。");
        else
            playList.removeAt(i);
    }
	ui->playView->setList(playList);
    bool f = (playList.size() > 0);
    ui->curlistLabel->setText("当前播放列表 共" + QString::number(playList.size()) + "项");
    AFTER_DEL(f)
}

void PlayerWindow::on_clearButton_clicked() {
	ui->playView->clear();
    player->clear();
    ui->curlistLabel->setText("当前播放列表 共0项");
    ui->progressSlider->setMaximum(0);
    AFTER_DEL(false)
}
#undef AFTER_DEL
void PlayerWindow::on_addButton_clicked() {ui->actionopenFile->trigger();}

void PlayerWindow::on_onlineSearcher_done() {
    if(QFile("links.tmp").exists()) {
        res->setItems(scher->analyzeResult());
        res->show();
    }
    else
        QMessageBox::warning(this,"温馨提示","搜索时出错，请检查网络连接");
    ui->searchLabel->setPixmap(QPixmap(":/Icons/images/serach.png"));
    ui->searchLabel->setClickable(true);
}

void PlayerWindow::on_playView_doubleClicked(const QModelIndex &index) {
    player->setCurrentMediaIndex(index.row());
    player->play();
}

inline void PlayerWindow::moveItem(bool moveUp) {
    QModelIndexList selected = ui->playView->getSelections();
    if(selected.isEmpty()||selected.size() > 1)
        return;
    int row = selected[0].row();
	QStringList playList = ui->playView->list();
    if(!moveUp) {
        if(player->moveDown(row)) {
			playList.move(row,row + 1);
            row++;
        }
    }
    else {
        if(player->moveUp(row)) {
			playList.move(row,row - 1);
            row--;
        }
    }
    RESET_LABEL;
	ui->playView->setList(playList);
    ui->playView->setSelected(row);
}

PlayerWindow::~PlayerWindow() {
    QSettings setting(CONFIG_FILE,QSettings::IniFormat);
    setting.setValue(LAST_PATH,lastPath);
    setting.setValue(LAST_VOLUME,ui->volumeSlider->value());
    setting.setValue(LAST_MODE,(int)player->mode);
    settingWind->saveSettings(setting);
    saveList("default.lst");
    delete ui;
    delete settingWind;
}
#undef Zero
