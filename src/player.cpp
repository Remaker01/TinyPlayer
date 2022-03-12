#include "player.h"
#ifndef NDEBUG
#include <QDebug>
#endif
PlayerCore::PlayerCore(QWidget *p):timer(new QTimer(this)),list(new QMediaPlaylist) {
    connectSlots();
    timer->setInterval(250);
}

inline void PlayerCore::connectSlots() {
    connect(timer,&QTimer::timeout,this,&PlayerCore::timedOut);
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

uint PlayerCore::getCurrentMediaIndex() {return list->currentIndex();}
void PlayerCore::setMedia(const QFile *media) {
    QString filename = media->fileName();
    QMediaContent content(QUrl::fromLocalFile(filename));
    QMediaPlayer::setMedia(content);
}

void PlayerCore::setPos(int pos) {QMediaPlayer::setPosition((qint64)pos * 1000);}

void PlayerCore::setCurrentMediaIndex(uint i) {
#ifndef NDEBUG
    qDebug() << "before set:" << list->currentIndex();
#endif
    if(i > list->mediaCount()||i == list->currentIndex())
        return;
    list->setCurrentIndex((int)i);
    QMediaPlayer::setMedia(list->currentMedia());
#ifndef NDEBUG
    qDebug() << "after set:" << list->currentIndex();
#endif
}

void PlayerCore::addToList(const QFile &media) {
    QMediaContent content(QUrl::fromLocalFile(media.fileName()));
    list->addMedia(content);
}

bool PlayerCore::removeFromList(uint loc) {
#ifndef NDEBUG
        qDebug() << "before delete:" << list->currentIndex();
#endif
    uint now = (uint)list->currentIndex();
    bool ret = list->removeMedia(loc);
    if(loc <= now) {
        list->setCurrentIndex(now - 1);
        if(loc==now)
            QMediaPlayer::setMedia(list->currentMedia());
    }
    else    list->setCurrentIndex(now);
#ifndef NDEBUG
        qDebug() << "after delete:" << list->currentIndex();
#endif
        return ret;
}

const QMediaPlaylist *PlayerCore::getAllMedia() {return list;}
