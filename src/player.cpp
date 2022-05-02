#include "player.h"
#ifndef NDEBUG
#include <QDebug>
#endif
const QString PlayerCore::Formats[6] = {".mp3",".wav",".aiff",".flac",".aac",".wma"};
VlcInstance *PlayerCore::ins = new VlcInstance(VlcCommon::args());
PlayerCore::PlayerCore(QObject *parent):VlcMediaPlayer(ins) {
    curMedia = new VlcMedia("",true,ins);
    connectSlots();
}

inline void PlayerCore::connectSlots() {
    connect(this,&VlcMediaPlayer::end,this,[this]() {
        //qDebug() << "Ended";
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
    curMedia = new VlcMedia(media,ins);
    curMedia->parse();
    while (!curMedia->parsed())
        QCoreApplication::processEvents();
    VlcMediaPlayer::openOnly(curMedia);
    if(curMedia->duration() != orgLen)
        emit VlcMediaPlayer::lengthChanged(curMedia->duration());
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
    QString tmp = getMedia(i);
    return Music(QUrl::fromLocalFile(tmp));
}

int PlayerCore::getPosInSecond() {
    return qRound(VlcMediaPlayer::time() / 1000.0);
}

int PlayerCore::getCurrentMediaIndex() {return current;}

void PlayerCore::setPos(int pos) {
    //TODO: 刚加载文件时无法设置
    Vlc::State sta = VlcMediaPlayer::state();
    if(sta != Vlc::Playing&&sta != Vlc::Paused) {
        startLoc = pos * 1000;
        qDebug() << sta;
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
    if(music.getUrl().toLocalFile().isEmpty())
        return false;
    list.append(tmp);
    medias.insert(music);
    return true;
}

bool PlayerCore::removeFromList(uint loc) {
    if(loc >= list.size())
        return false;
    uint now = current;
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
            current = std::max(0,(int)loc - 1);
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

void PlayerCore::play() {
    Vlc::State sta = VlcMediaPlayer::state();  //获取初始播放状态
    if(sta == Vlc::Playing)
        return;
    VlcMediaPlayer::play();
    while (VlcMediaPlayer::state() != Vlc::Playing)
        QCoreApplication::processEvents();
    if(sta != Vlc::Paused) {
        VlcMediaPlayer::setTime(startLoc);
#ifndef NDEBUG
        qDebug() << "start =" << startLoc;
#endif
        startLoc = 0;
    }
}

PlayerCore::~PlayerCore() {
    delete curMedia;
    ins->deleteLater();
}
