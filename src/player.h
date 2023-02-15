#ifndef PLAY_H
#define PLAY_H
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtCore/Audio.h>
#include <VLCQtCore/Equalizer.h>
#include <QMessageBox>
#include "music.h"
#include "onlineseacher.h"
/**
 * 播放器核心类，对VlcMediaPlayer进行了一些封装与扩展
 */
class PlayerCore : public VlcMediaPlayer{
    Q_OBJECT
private:
    static VlcInstance ins;
    VlcEqualizer *equ;
    VlcMedia *curMedia;
    QSet<Music> medias;
    QList<QUrl> list;
    int current = -1;
    int startLoc = 0;
    void connectSlots();
    void setMedia(const QString &media);
public:
    static constexpr int MAX_MEDIA_COUNT = 10000;
    static constexpr int MODE_COUNT = 4,FORMAT_COUNT = 16;
    static const QString Formats[FORMAT_COUNT];
    /// 各模式的提示文字
    static const QString MODE_TIPS[MODE_COUNT];
    /// 播放模式
    enum PlayMode{SIGNLE = 0,SEQUENTIAL,SIGNLE_LOOP,LIST_LOOP};
    PlayMode mode = SIGNLE;
    explicit PlayerCore(QObject *parent = nullptr);
    ///获取当前媒体url
    QUrl getMedia();
    ///获取指定编号的媒体目录url
    const QUrl &getMedia(int i);
    ///获取媒体详细信息
    Music getMediaDetail(int i);
    Music getMediaDetail();
    ///获取以秒为单位的时间
    int getPosInSecond();
    int getCurrentMediaIndex();
    ///设置时间，以秒为单位。
    void setPos(int pos);
    ///设置播放列表位置
    void setCurrentMediaIndex(int i);
    ///添加到播放列表
    bool addToList(const QString &media,bool local = true);
    /**
     * 从播放列表中移除特定媒体
     * 若移除的是当前活动媒体，则自动将当前活动媒体设为上一个，如果同时是第一个则设为删除后的第一个(即原来的第二个)
     */
    bool removeFromList(int loc);
    ///清空播放列表
    void clear();
    ~PlayerCore();
public slots:
    void play();
    void goNext();
    void goPrevious();
    /**
     * @brief moveUp 将指定媒体上移
     * @param i 要移动哪个媒体
     * @param k 移动多少个单位
     * @return 当i位置合法且移动后的位置仍合法，返回true，否则返回false
     * @note moveDown类似
     */
    bool moveUp(int i,int k = 1);
    bool moveDown(int i, int k = 1);
    void setSoundEffect(uint index);
signals:
    void finished();
    //void mediaSourceChanged(qint64 newTime);
};
#endif // PLAY_H
