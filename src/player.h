#ifndef PLAY_H
#define PLAY_H
#include "playerbutton.h"
#include <QFile>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QTimer>
#include <QDir>
/**
 * 播放器核心类
 */
class PlayerCore:public QMediaPlayer {
    Q_OBJECT
private:
    QTimer *timer;
    void connectSlots();
    QSet<QUrl> medias;
    QMediaPlaylist *list;
public:
    enum TimerOperation{NONE,START,STOP};
    enum PlayMode{SIGNLE = 0,SEQUENTIAL,LOOP};
    PlayMode mode = SIGNLE;
    PlayerCore(QWidget *p = nullptr);
    void changeState(PlayerButton *label,const QString &toolTip,const QPixmap &pixmap,TimerOperation opt = NONE);
    ///获取当前媒体
    QUrl getMedia();
    ///获取指定媒体
    QUrl getMedia(int i);
    ///获取以秒为单位的时间
    int getPosInSecond();
    int getCurrentMediaIndex();
    ///使用本地文件设置媒体
    void setMedia(const QFile *media);
    ///设置时间，以秒为单位
    void setPos(int pos);
    bool setCurrentMediaIndex(uint i);
    ///添加到播放列表
    bool addToList(const QString &media);
    ///从播放列表中移除
    bool removeFromList(uint loc);
    ///清空播放列表
    void clear();
    ///打开文件所在文件夹
    ~PlayerCore();
signals:
    void timedOut();
    void finished();
    //void mediaChanged(qint64 newTime);
};

#endif // PLAY_H
