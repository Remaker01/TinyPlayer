#include "player.h"
#ifndef NDEBUG
#include <QDebug>
#endif
PlayerCore::PlayerCore(QWidget *p):timer(new QTimer(this)) {
    registerSlots();
    timer->setInterval(250);
}

inline void PlayerCore::registerSlots() {
    connect(timer,&QTimer::timeout,this,[this]() {
#ifndef NDEBUG
    qDebug() << QMediaPlayer::position();
#endif
    emit timedOut();
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

void PlayerCore::setMedia(const QFile *media) {
    QString filename = media->fileName();
    QMediaContent content(QUrl::fromLocalFile(filename));
    QMediaPlayer::setMedia(content);
}

void PlayerCore::setPos(int pos) {QMediaPlayer::setPosition((qint64)pos * 1000);}

//void PlayerCore::setRatio(double r) {
//    setPlaybackRate(r);
//    timer->setInterval(1000 / r);
//}
