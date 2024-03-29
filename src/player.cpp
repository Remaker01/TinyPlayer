#include "player.h"
#ifndef NDEBUG
#include <QDebug>
#endif
const QString PlayerCore::Formats[FORMAT_COUNT] = {".mp3",".mp2",".mp1","mpga",  //MPEG Audio
                                                   ".wav",".wma",   //Windows Audio
                                                   ".flac",".ape",  //Lossless
                                                   ".aac",  //aac
                                                   ".ogg",".oga", // OggVorbis
                                                   ".aif",".aiff",".aifc",  //Aiff
                                                   ".m4a", //m4a
                                                   ".au",".snd"};  //au
const QString PlayerCore::MODE_TIPS[MODE_COUNT] = {"单曲播放","顺序播放","单曲循环","列表循环"};
VlcInstance PlayerCore::ins(VlcCommon::args());
PlayerCore::PlayerCore(QObject *parent):VlcMediaPlayer(&ins) {
    curMedia = new VlcMedia("",&ins);
    connectSlots();
    equ = nullptr;
}

inline void PlayerCore::connectSlots() {
    connect(this,&VlcMediaPlayer::end,this,[this]() {
        switch (mode) {
        case SIGNLE:
            //去掉这句，会导致结束后无法再次开始
            reset();
            emit finished();
            break;
        case SEQUENTIAL:
            if(current < list.size() - 1) {
                goNext();
                play();
            }
            else {
                reset();
                emit finished();
            }
            break;
        case SIGNLE_LOOP:
            //不能直接设为0再重新开始，否则导致进度条一直卡在满格
            reset();
            play();
            break;
        case LIST_LOOP:
            if(list.size() == 1) {
                reset();
            }
            else
                goNext();
            play();
            break;
        }
    });
    connect(this,&VlcMediaPlayer::paused,this,[this]{startLoc = VlcMediaPlayer::time();});
    connect(this,&VlcMediaPlayer::stopped,this,[this]{startLoc = 0;});
}

inline void PlayerCore::reset() {VlcMediaPlayer::stop();startLoc = 0;}

inline void PlayerCore::setMedia(const QString &media) {
    Vlc::State sta = VlcMediaPlayer::state();
    VlcMediaPlayer::stop();
    delete curMedia;
//    curMedia = nullptr;
    if(!media.isEmpty()) {
        curMedia = new VlcMedia(media,&ins);
        curMedia->parse();
        while (!curMedia->parsed())
            QCoreApplication::processEvents();
        VlcMediaPlayer::openOnly(curMedia);
        emit VlcMediaPlayer::lengthChanged(curMedia->duration()); //bug in lib?
        setPos(0);
    }
    else {
        curMedia = nullptr;
        current = -1;
        emit VlcMediaPlayer::lengthChanged(0);
    }
    if(sta == Vlc::Playing||sta == Vlc::Paused)
        emit finished();
}

inline void PlayerCore::setMedia(const QUrl &media) {setMedia(media.toString());}

QUrl PlayerCore::getMedia() {
    if(curMedia == nullptr)
        return QUrl();
    return QUrl(curMedia->currentLocation());
}

const QUrl &PlayerCore::getMedia(int i) {return list[i].getUrl();}

const Music &PlayerCore::getMediaDetail(int i) {return list[i];}

const Music &PlayerCore::getMediaDetail() {
    static const Music NULL_MUSIC = Music();
    return (current >= 0)? list[current]:NULL_MUSIC;
}

int PlayerCore::getPosInSecond() {
    return qRound(VlcMediaPlayer::time() / 1000.0);
}

int PlayerCore::getCurrentMediaIndex() {return current;}

void PlayerCore::setPos(int pos) {
    //查看源码可知只有playing,paused,buffering三种状态可以设置
    Vlc::State sta = VlcMediaPlayer::state();
    if(sta != Vlc::Playing)
        startLoc = pos * 1000;
    else
        VlcMediaPlayer::setTime(pos * 1000);
}

void PlayerCore::setCurrentMediaIndex(int i) {
    if(i >= list.size()||i < 0)
        return;
    if(i == current) {
#ifndef NDEBUG
        qWarning() << "WARNING:setCurrentMediaIndex:current index == i ==" << i << ",Exiting.";
#endif
        return;
    }
    current = i;
    setMedia(list[current].getUrl());
}

bool PlayerCore::addToList(const QString &media, bool local, QString alter) {
   if(list.size() == MAX_MEDIA_COUNT) {
        QMessageBox::warning(nullptr,"警告","已达到音乐数上限" + QString::number(MAX_MEDIA_COUNT));
        return false;
    }
    bool ok = false;
    for(const QString &format:Formats) {
        if(media.indexOf(format,Qt::CaseInsensitive) > 0) {
            ok = true;
            break;
        }
    }
    QUrl tmp = (local)?QUrl::fromLocalFile(media):QUrl(media);
    Music music(tmp,alter);
    if((local&&!Music::isLegal(media))||!ok||medias.contains(music))
        return false;
    list.append(music);
    medias.insert(music);
    return true;
}

bool PlayerCore::removeFromList(int loc) {
    if(loc >= list.size()||loc < 0)
        return false;
    int now = current;
    medias.remove(list[loc]);
    list.removeAt(loc);
    if(list.isEmpty()) { // 删掉了最后一个
        current = -1;
        setMedia("");
        emit finished();
        return true;
    }
    if(loc <= now) { //会导致变current变化
        if(loc == now) {
            current = std::max(0,loc - 1);
            setMedia(list[current].getUrl());
            emit finished();
        }
        //loc<now,此时now一定不为0
        else
            current--;
    }
    return true;
}

void PlayerCore::clear() {
    Vlc::State sta = VlcMediaPlayer::state();
    list.clear();
    medias.clear();
    setMedia("");
    current = -1;
    if(sta != Vlc::Playing&&sta != Vlc::Paused)   //防止重复发送信号
        emit finished();
}
//覆盖play()用于缓解进度条拖动及未开始播放不能设置时间问题
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
    while (VlcMediaPlayer::state() != Vlc::Playing) {
        if(VlcMediaPlayer::state() == Vlc::Error) {
            QMessageBox::critical(nullptr,"出错了!","播放失败,无法访问指定的媒体");
            return;
        }
        QCoreApplication::processEvents();
    }
    VlcMediaPlayer::setTime(startLoc);
}

void PlayerCore::goNext() {
    if(current < 0) {
#ifndef NDEBUG
        qWarning() << "WARNING:goNext:current == -1,exiting.";
#endif
        return;
    }
    if(list.size() > 1)
        setCurrentMediaIndex((current + 1) % list.size());
}

void PlayerCore::goPrevious() {
    if(current < 0) {
#ifndef NDEBUG
        qWarning() << "WARNING:goNext:current == -1,exiting.";
#endif
        return;
    }
    if(list.size() > 1)
        setCurrentMediaIndex((current == 0) ? list.size() - 1 : current - 1);
}

bool PlayerCore::moveUp(int i,int k) {
    if(k > 0&&i >= k&&i < list.size()) {
        list.move(i,i - k);
        if(current == i)
            current -= k;
        else if (current < i&&current >= i - k) //[i-k,i-1]范围内的项目序号受影响
            current++;
        return true;
    }
    return false;
}

bool PlayerCore::moveDown(int i,int k) {
    if(k > 0&&i >= 0&&i + k < list.size()) {
        list.move(i,i + k);
        if(current == i)
            current += k;
        else if (current > i&&current <= i + k)
            current--;
        return true;
    }
    return false;
}

void PlayerCore::setSoundEffect(uint index) {
    static const uint map[] = {0u,1u,4u,5u,7u,11u,13u,16u};
    if(index >= sizeof (map)/sizeof (uint)) {
#ifndef NDEBUG
        qCritical() << "setSoundEffect:index error.";
#endif
        return;
    }
    if(equ == nullptr)
        equ = new VlcEqualizer(this);
    equ->loadFromPreset(map[index]);
    equ->setEnabled(true);
}

PlayerCore::~PlayerCore() {
    delete curMedia;
    curMedia = nullptr;
}
