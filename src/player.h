#ifndef PLAY_H
#define PLAY_H
#include <QLabel>
#include <QFile>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QTimer>
/**
 * 播放器核心类
 */
class PlayerCore:public QMediaPlayer {
    Q_OBJECT
private:
    QTimer *timer;
    void connectSlots();
    QMediaPlaylist *list;
public:
    enum TimerOperation{NONE,START,STOP};
    PlayerCore(QWidget *p = nullptr);
    void changeState(QLabel *label,const QString &toolTip,const QPixmap &pixmap,TimerOperation opt = NONE);
//    void clear();
    ///获取当前媒体
    QUrl getMedia();
    ///获取以秒为单位的时间
    int getPosInSecond();
    uint getCurrentMediaIndex();
    ///使用本地文件设置媒体
    void setMedia(const QFile *media);
    ///设置时间，以秒为单位
    void setPos(int pos);
    void setCurrentMediaIndex(uint i);
    ///添加到播放列表
    void addToList(const QFile &media);
    ///从播放列表中移除
    bool removeFromList(uint loc);
    const QMediaPlaylist *getAllMedia();
signals:
    void timedOut();
    void finished();
    //void mediaChanged(qint64 newTime);
};

#endif // PLAY_H
