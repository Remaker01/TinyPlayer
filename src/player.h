#ifndef PLAY_H
#define PLAY_H
#include <QLabel>
#include <QFile>
#include <QMediaPlayer>
#include <QTimer>
class PlayerCore:public QMediaPlayer {
    Q_OBJECT
private:
    QTimer *timer;
    void registerSlots();
public:
    enum TimerOperation{NONE,START,STOP};
    void changeState(QLabel *label,const QString &toolTip,const QPixmap &pixmap,TimerOperation opt = NONE);
    PlayerCore(QWidget *p = nullptr);
    QUrl getMedia();
    int getPosInSecond();  //获取以秒为单位的时间
    void setMedia(const QFile *media);  //使用本地文件设置媒体
    void setPos(int pos);  //设置时间，以秒为单位
signals:
    void timedOut();
    void finished();
    //void mediaChanged(qint64 newTime);
};

#endif // PLAY_H
