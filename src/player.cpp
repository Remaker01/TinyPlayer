#include "player.h"
#ifndef NDEBUG
#include <QDebug>
#endif
PlayerCore::PlayerCore(QWidget *p):timer(new QTimer(this)) {
    registerSlots();
}

inline void PlayerCore::registerSlots() {
    connect(timer,&QTimer::timeout,this,&PlayerCore::timedOut);
}

void PlayerCore::changeState(QLabel *label,const QString &toolTip, const QPixmap &pixmap,TimerOperation opt) {
    label->setPixmap(pixmap);
    label->setToolTip(toolTip);
    if(opt == PlayerCore::START&&!timer->isActive())
        timer->start();
    else if(opt == PlayerCore::STOP)
        timer->stop();
}

QUrl PlayerCore::getMedia() {return QMediaPlayer::media().canonicalUrl();}
int PlayerCore::getPosInSecond() {
    return qRound(QMediaPlayer::position() / 1000.0);
}
/**
 * PlayButton::setMedia
 * 设置要播放的媒体
 * @param media 目标媒体
 */
void PlayerCore::setMedia(const QFile *media) {
    QString filename = media->fileName();
    QMediaContent content(QUrl::fromLocalFile(filename));
    QMediaPlayer::setMedia(content);
}

void PlayerCore::setPos(int pos) {QMediaPlayer::setPosition((qint64)pos * 1000);}
