#include "player.h"
#ifndef NDEBUG
#include <QDebug>
#endif
static const QString supportFormats[] = {".mp3",".wav",".wma",".aiff"};
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
            case LOOP:
                QMediaPlayer::setPosition(0ll);
                QMediaPlayer::play();
                break;
            }
        }
    });
}

void PlayerCore::changeState(QLabel *label,const QString &toolTip, const QPixmap &pixmap,TimerOperation opt) {
    label->setPixmap(pixmap);
    label->setToolTip(toolTip);
    if(opt == START&&!timer->isActive())
        timer->start();
    else if(opt == STOP)
        timer->stop();
}

QUrl PlayerCore::getMedia() {return QMediaPlayer::media().canonicalUrl();}
int PlayerCore::getPosInSecond() {
    return qRound(QMediaPlayer::position() / 1000.0);
}

uint PlayerCore::getCurrentMediaIndex() {return (uint)list->currentIndex();}
void PlayerCore::setMedia(const QFile *media) {
    QString filename = media->fileName();
    QMediaContent content(QUrl::fromLocalFile(filename));
    QMediaPlayer::setMedia(content);
}

void PlayerCore::setPos(int pos) {QMediaPlayer::setPosition((qint64)pos * 1000);}

void PlayerCore::setCurrentMediaIndex(uint i) {
    if(i > list->mediaCount()||i == list->currentIndex())
        return;
    list->setCurrentIndex((int)i);
    QMediaPlayer::setMedia(list->currentMedia());
    emit finished();
}

bool PlayerCore::addToList(const QString &media) {
    bool ok = false;
    for (const QString &format : supportFormats) {
        if(media.endsWith(format,Qt::CaseInsensitive)) {
            ok = true;
            break;
        }
    }
    if(!ok)    return false;
    QMediaContent content(QUrl::fromLocalFile(media));
    return list->addMedia(content);
}

bool PlayerCore::removeFromList(uint loc) {
#ifndef NDEBUG
        qDebug() << "before delete:" << list->currentIndex();
#endif
    uint now = (uint)list->currentIndex();
    bool ret = list->removeMedia((int)loc);
    if(loc <= now) {
        list->setCurrentIndex(now - 1);
        if(loc==now) {
            QMediaPlayer::setMedia(list->currentMedia());
            emit finished();
        }
    }
    else    list->setCurrentIndex(now);
#ifndef NDEBUG
        qDebug() << "after delete:" << list->currentIndex();
#endif
        return ret;
}

void PlayerCore::clear() {
    list->clear();
    QMediaPlayer::setMedia(QUrl(""));
    emit finished();
}
