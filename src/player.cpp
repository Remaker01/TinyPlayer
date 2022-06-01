#include "player.h"
#ifndef NDEBUG
#include <QDebug>
#endif
const QString PlayerCore::Formats[FORMAT_COUNT] = {".mp3",".wav",".aiff",".flac",".aac",".wma"};
const QString PlayerCore::MODE_TIPS[MODE_COUNT] {"单曲播放","顺序播放","单曲循环","列表循环"};
VlcInstance PlayerCore::ins(VlcCommon::args());
PlayerCore::PlayerCore(QObject *parent):VlcMediaPlayer(&ins) {
    ins.setLogLevel(Vlc::ErrorLevel);
    curMedia = new VlcMedia("",true,&ins);
    connectSlots();
}

inline void PlayerCore::connectSlots() {
    connect(this,&VlcMediaPlayer::end,this,[this]() {
        switch (mode) {
        case SIGNLE:
            //去掉这句，会导致结束后无法再次开始
            setMedia(list[current].toString(),false);
            emit finished();
            break;
        case SEQUENTIAL:
            if(current < list.size() - 1) {
                current++;
                setMedia(list[current].toString());
            }
            else
                emit finished();
            break;
        case SIGNLE_LOOP:
            //不能直接设为0再重新开始，否则导致进度条一直卡在满格
            setMedia(list[current].toString());
            break;
        case LIST_LOOP:
            current = ((current + 1) % list.size());
            setMedia(list[current].toString());
            break;
        }
    });
}

inline void PlayerCore::setMedia(const QString &media,bool start) {
    int orgLen = (curMedia != nullptr) ? curMedia->duration() : 0;
    delete curMedia;
    curMedia = new VlcMedia(media,&ins);
    curMedia->parse();
    while (!curMedia->parsed())
        QCoreApplication::processEvents();
    VlcMediaPlayer::openOnly(curMedia);
    if(curMedia->duration() != orgLen)
        emit VlcMediaPlayer::lengthChanged(curMedia->duration());
    setPos(0);
    if(start)
        VlcMediaPlayer::play();
}

QString PlayerCore::getMedia() {
    QUrl url(curMedia->currentLocation());
    return url.toLocalFile();
}

QString PlayerCore::getMedia(int i) {
    return list[i].toLocalFile();
}

Music PlayerCore::getMediaDetail(int i) {
    return Music(list[i]);
}

int PlayerCore::getPosInSecond() {
    return qRound(VlcMediaPlayer::time() / 1000.0);
}

int PlayerCore::getCurrentMediaIndex() {return current;}

void PlayerCore::setPos(int pos) {
    Vlc::State sta = VlcMediaPlayer::state();
    if(sta != Vlc::Playing) {
        startLoc = pos * 1000;
    }
    else
        VlcMediaPlayer::setTime(pos * 1000);
}

void PlayerCore::setCurrentMediaIndex(int i) {
    if(i >= list.size()||i == current)
        return;
    current = i;
    setMedia(list[current].toString(),false);
    emit finished();
}

bool PlayerCore::addToList(const QString &media) {
   if(list.size() == MAX_MEDIA_COUNT) {
        QMessageBox::warning(nullptr,"警告","已达到音乐数上限" + QString::number(MAX_MEDIA_COUNT));
        return false;
    }
    bool ok = false;
    for(const QString &format:Formats) {
        if(media.endsWith(format,Qt::CaseInsensitive)) {
            ok = true;
            break;
        }
    }
    QUrl tmp = QUrl::fromLocalFile(media);
    if(!Music::isLegal(media)||!ok||medias.contains(tmp))
        return false;
    Music music(tmp);
    if(music.getUrl().isEmpty())
        return false;
    list.append(tmp);
    medias.insert(music);
    return true;
}

bool PlayerCore::removeFromList(int loc) {
    if(loc >= list.size()||loc < 0)
        return false;
    int now = current;
    medias.remove(list[loc]);
    list.removeAt(loc);
    if(list.isEmpty()) {
        current = -1;
        setMedia("");
        emit finished();
        return true;
    }
    if(loc <= now) {
        if(loc == now) {
            current = std::max(0,loc - 1);
            setMedia(list[current].toString(),false);
            emit finished();
        }
        //loc<now,此时now一定不为0
        else
            current--;
    }
    return true;
}

void PlayerCore::clear() {
    list.clear();
    medias.clear();
    setMedia("");
    current = -1;
    emit finished();
}
//覆盖play()与pause()用于缓解进度条拖动及未开始播放不能设置时间问题
void PlayerCore::play() {
    Vlc::State sta = VlcMediaPlayer::state();  //获取初始播放状态
    if(sta == Vlc::Playing)
        return;
    //暂停
    else if (sta == Vlc::Paused)
        VlcMediaPlayer::resume();
    //停止
    else
        VlcMediaPlayer::play();
    while (VlcMediaPlayer::state() != Vlc::Playing)
        QCoreApplication::processEvents();
    VlcMediaPlayer::setTime(startLoc);
#ifndef NDEBUG
    qDebug() << "start =" << startLoc;
#endif
    startLoc = 0;
}

void PlayerCore::pause() {
    int tme = VlcMediaPlayer::time();
    VlcMediaPlayer::togglePause();
    startLoc = tme;
}

PlayerCore::~PlayerCore() {
    delete curMedia;
}
