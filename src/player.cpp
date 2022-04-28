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
            setMedia(list[currentIndex].toString(),false);
            emit finished();
            break;
        case SEQUENTIAL:
            if(currentIndex < list.size() - 1) {
                currentIndex++;
                setMedia(list[currentIndex].toString());
            }
            else
                emit finished();
            break;
        case SIGNLE_LOOP:
            //不能直接设为0再重新开始，否则导致进度条一直卡在满格
            setMedia(list[currentIndex].toString());
            break;
        case LIST_LOOP:
            currentIndex = ((currentIndex + 1) % list.size());
            setMedia(list[currentIndex].toString());
            break;
        }
    });
}

inline void PlayerCore::setMedia(const QString &media,bool start) {
    int orgLen = curMedia->duration();
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

int PlayerCore::getCurrentMediaIndex() {return currentIndex;}

void PlayerCore::setPos(int pos) {
    //TODO: 刚加载文件时无法设置
    VlcMediaPlayer::setTime(qRound(pos * 1000.0));
}

void PlayerCore::setCurrentMediaIndex(int i) {
    if(i >= list.size()||i == currentIndex)
        return;
    currentIndex = i;
    setMedia(list[currentIndex].toString(),false);
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
    uint now = currentIndex;
    medias.remove(list[loc]);
    qDebug() << loc;
    list.removeAt(loc);
    if(list.isEmpty()) {
        currentIndex = -1;
        return true;
    }
    if(loc <= now) {
        if(loc == now) {
            currentIndex = std::max(0,(int)loc - 1);
            setMedia(list[currentIndex].toString(),false);
            emit finished();
        }
        //loc<now,此时now一定不为0
        else
            currentIndex--;
    }
#ifndef NDEBUG
    qDebug() << "now =" << currentIndex;
#endif
    return true;
}

void PlayerCore::clear() {
    list.clear();
    medias.clear();
    VlcMediaPlayer::stop();
    currentIndex = -1;
    emit finished();
}

PlayerCore::~PlayerCore() {
    delete curMedia;
}
