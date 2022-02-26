#include "play.h"
#ifndef NDEBUG
#include <QDebug>
#endif
Player::Player(QWidget *p):player(new QMediaPlayer),timer(new QTimer(this)) {
    setParent(p);
    QPixmap pic(":/Icons/images/play.png");
    setScaledContents(true);
    setPixmap(pic);
    setCursor(Qt::PointingHandCursor);
    setToolTip("播放");
    registerSlots();
}

Player::Player(int width,int height,QWidget *p):Player(p) {
    resize(width,height);
    w=width,h=height;
}

inline void Player::registerSlots() {
    connect(player,&QMediaPlayer::mediaChanged,this,[this]() {
        static const QPixmap playIcon(":/Icons/images/play.png");
        changeState("播放",playIcon,Player::STOP);
    });
    connect(player,&QMediaPlayer::stateChanged,this,[&,this](QMediaPlayer::State nState){
        if(nState == QMediaPlayer::StoppedState&&player->position() > 0) {
            static const QPixmap playIcon(":/Icons/images/play.png");
            changeState("播放",playIcon,Player::STOP);
            emit finished();
        }
    });
    connect(timer,&QTimer::timeout,this,&Player::timedOut);
    connect(player,&QMediaPlayer::durationChanged,this,&Player::mediaChanged);
}

void Player::changeState(const QString &toolTip, const QPixmap &pixmap,TimerOperation opt) {
    setPixmap(pixmap);
    setToolTip(toolTip);
    if(opt == Player::START)
        timer->start(1000);
    else if(opt == Player::STOP)
        timer->stop();
}

void Player::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton&&player->isAudioAvailable()) {
        int wi = width(),he = height();
        wi = wi - (wi >> 3);
        he = he - (he >> 3);
        resize(wi,he);
        move(x() + 4,y() + 5);
    }
}

void Player::mouseReleaseEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton) {
        //当前仍未播放
        if(player->isAudioAvailable()) {
            resize(w,h);
            move(x() - 4,y() - 5);
            if(player->state() != QMediaPlayer::PlayingState) {
                start();
            }
            else  pause();
        }
    }
}
/**
 * 暂停播放
 */
void Player::pause(bool stopTimer) {
    static const QPixmap playIcon(":/Icons/images/play.png");
    changeState("播放",playIcon,stopTimer ? Player::STOP : Player::NONE);
    player->pause();
}
/**
 * 开始播放
 */
void Player::start(bool startTimer) {
    static const QPixmap pauseIcon(":/Icons/images/pause.png");
    changeState("暂停",pauseIcon,startTimer ? Player::START : Player::NONE);
    player->play();
}

QString Player::getMedia() {return player->media().canonicalUrl().fileName();}
int Player::getPos() {
    int x = qRound(player->position() / 1000.0);
    return x;
}
QMediaPlayer::State Player::getState() {return player->state();}
/**
 * PlayButton::setMedia
 * 设置要播放的媒体
 * @param media 目标媒体
 */
void Player::setMedia(const QFile *media) {
    QString filename = media->fileName();
    QMediaContent content(QUrl::fromLocalFile(filename));
    player->setMedia(content);
}
void Player::setVolum(int v) {player->setVolume(v);}
void Player::setPos(int pos) {player->setPosition((qint64)pos * 1000);}
