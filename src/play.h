#ifndef PLAY_H
#define PLAY_H
#include <QLabel>
#include <QMouseEvent>
#include <QMessageBox>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QTimer>
#include <QSlider>
class Player:public QLabel {
    Q_OBJECT
private:
    enum TimerOperation{NONE,START,STOP};
    int w,h;
    QMediaPlayer *player;
    QTimer *timer;
    void registerSlots();
    void changeState(const QString &toolTip,const QPixmap &pixmap,TimerOperation opt = Player::NONE);
public:
    Player(int width,int height,QWidget *p = nullptr);
    Player(QWidget *p = nullptr);

    //void ModifyTime(QLabel &label);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void start(bool startTimer = true);
    void pause(bool stopTimer = true);
    QString getMedia();
    int getPos();
    QMediaPlayer::State getState();
    void setMedia(const QFile *media);
    void setVolum(int v);
    void setPos(int pos);
signals:
    void timedOut();
    void finished();
    void mediaChanged(qint64 newTime);
};

#endif // PLAY_H
