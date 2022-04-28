#ifndef PLAY_H
#define PLAY_H
#include "playerbutton.h"
#include <QFile>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtCore/Audio.h>
#include "music.h"
/**
 * 播放器核心类，对VlcMediaPlayer进行了一些封装与扩展
 */
class PlayerCore : public VlcMediaPlayer{
    Q_OBJECT
private:
    static VlcInstance *ins;
    VlcMedia *curMedia;
    QSet<Music> medias;
    QList<QUrl> list;
    int currentIndex = -1;
    void connectSlots();
    void setMedia(const QString &media,bool start = true);
public:
    static constexpr int MODE_COUNT = 4;
    static const QString Formats[6];
    enum PlayMode{SIGNLE = 0,SEQUENTIAL,SIGNLE_LOOP,LIST_LOOP};
    PlayMode mode = SIGNLE;
    explicit PlayerCore(QObject *parent = nullptr);
    ///获取当前媒体
    QString getMedia();
    ///获取指定编号的媒体目录
    QString getMedia(int i);
    ///获取媒体详细信息
    Music getMediaDetail(int i);
    ///获取以秒为单位的时间
    int getPosInSecond();
    int getCurrentMediaIndex();
    ///设置时间，以秒为单位
    void setPos(int pos);
    void setCurrentMediaIndex(int i);
    ///添加到播放列表
    bool addToList(const QString &media);
    ///从播放列表中移除
    bool removeFromList(uint loc);
    ///清空播放列表
    void clear();
    ~PlayerCore();
signals:
    void finished();
    //void mediaSourceChanged(qint64 newTime);
};

#endif // PLAY_H
