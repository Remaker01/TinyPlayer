#ifndef PLAY_H
#define PLAY_H
#include <QLabel>
#include <QFile>
#include <QMediaPlayer>
#include <QTimer>
/**
 * 播放器核心类
 */
class PlayerCore:public QMediaPlayer {
    Q_OBJECT
private:
    QTimer *timer;
    void registerSlots();
public:
    enum TimerOperation{NONE,START,STOP};
    void changeState(QLabel *label,const QString &toolTip,const QPixmap &pixmap,TimerOperation opt = NONE);
    PlayerCore(QWidget *p = nullptr);
    ///获取当前媒体
    QUrl getMedia();
    ///获取以秒为单位的时间
    int getPosInSecond();
    ///使用本地文件设置媒体
    void setMedia(const QFile *media);
    ///设置时间，以秒为单位
    void setPos(int pos);
    ///设置播放倍率
    //void setRatio(double r);
signals:
    void timedOut();
    void finished();
    //void mediaChanged(qint64 newTime);
};

#endif // PLAY_H
