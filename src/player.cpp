#include "player.h"
#ifndef NDEBUG
#include <QDebug>
#endif
static const QString Formats[] = {".mp3",".wav",".wma",".aiff"};
PlayerCore::PlayerCore(QWidget *p):timer(new QTimer(this)),list(new QMediaPlaylist) {
    connectSlots();
    timer->setInterval(250);
}

inline void PlayerCore::connectSlots() {
    connect(timer,&QTimer::timeout,this,&PlayerCore::timedOut);
    connect(this,&QMediaPlayer::stateChanged,this,[this]() {
        if(QMediaPlayer::state() == QMediaPlayer::StoppedState&&position() > 0) {
            switch (mode) {
            case SIGNLE:
                emit finished();
                break;
            case SEQUENTIAL:
                if(list->currentIndex() != list->mediaCount() - 1) {
                    list->next();
                    QMediaPlayer::setMedia(list->currentMedia());
                    QMediaPlayer::play();
                }
                else    emit finished();
                break;
            case SIGNLE_LOOP:
                QMediaPlayer::setPosition(0ll);
                QMediaPlayer::play();
                break;
            case LIST_LOOP:
                int cur = list->currentIndex();
                list->setCurrentIndex((cur + 1)%list->mediaCount());
                QMediaPlayer::setMedia(list->currentMedia());
                QMediaPlayer::play();
                break;
            }
        }
    });
}

void PlayerCore::changeState(PlayerButton *label,const QString &toolTip, const QPixmap &pixmap,TimerOperation opt) {
    label->setPixmap(pixmap);
    label->setToolTip(toolTip);
    if(opt == START&&!timer->isActive())
        timer->start();
    else if(opt == STOP)
        timer->stop();
}

QUrl PlayerCore::getMedia() {return QMediaPlayer::media().canonicalUrl();}

QUrl PlayerCore::getMedia(int i) {
    QMediaContent content(list->media(i));
    return content.canonicalUrl();
}

Music PlayerCore::getMediaDetail(int i) {
    return Music(getMedia(i));
}

int PlayerCore::getPosInSecond() {
    return qRound(QMediaPlayer::position() / 1000.0);
}

int PlayerCore::getCurrentMediaIndex() {return list->currentIndex();}
void PlayerCore::setMedia(const QFile *media) {
    QString filename = media->fileName();
    QUrl s = QUrl::fromLocalFile(filename);
    QMediaContent content(s);
    if(!medias.contains(s)) {
        medias.insert(s);
        list->addMedia(content);
    }
    QMediaPlayer::setMedia(content);
}

void PlayerCore::setPos(int pos) {QMediaPlayer::setPosition((qint64)pos * 1000);}

bool PlayerCore::setCurrentMediaIndex(uint i) {
    if(i > list->mediaCount()||i == list->currentIndex())
        return false;
    list->setCurrentIndex((int)i);
    QMediaPlayer::setMedia(list->currentMedia());
    emit finished();
    return QMediaPlayer::isAudioAvailable();
}

bool PlayerCore::addToList(const QString &media) {
    bool ok = false;
    for (const QString &format : Formats) {
        if(media.endsWith(format,Qt::CaseInsensitive)) {
            ok = true;
            break;
        }
    }
    QUrl tmp = QUrl::fromLocalFile(media);
    if(!Music::isLegal(media)||!ok||medias.contains(tmp))
        return false;
    Music music(tmp);
    if(music.getUrl().toLocalFile() != media)
        return false;
    QMediaContent content(tmp);
    ok &= list->addMedia(content);
    if(ok)    medias.insert(music);
    return ok;
}

bool PlayerCore::removeFromList(uint loc) {
    uint now = (uint)list->currentIndex();
    medias.remove(list->media((int)loc).canonicalUrl());
    bool ret = list->removeMedia((int)loc);
    if(loc <= now) {
        list->setCurrentIndex(now - 1);
        if(loc==now) {
            if(now == 0)
                QMediaPlayer::setMedia(QUrl(""));
            else
                QMediaPlayer::setMedia(list->currentMedia());
            emit finished();
        }
    }
    else    list->setCurrentIndex(now);
    return ret;
}

void PlayerCore::clear() {
    list->clear();
    medias.clear();
    QMediaPlayer::setMedia(QUrl(""));
    emit finished();
}

PlayerCore::~PlayerCore() {delete list;}
