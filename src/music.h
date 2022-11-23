#ifndef MUSIC_H
#define MUSIC_H
#include "global_include.h"
#include <VLCQtCore/MetaManager.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
class Music {
private:
    QString formattedTime;
    QUrl url;
    //歌曲名
    QString title,description,album;
    QUrl albumImage;
    int length;
    static bool isMPEG(QFile *media,QDataStream &reader);
    static bool isWav(QFile *media,QDataStream &reader);
    static bool isWma(QFile *media,QDataStream &reader);
    static bool isAiff(QFile *media,QDataStream &reader);
    static bool isFlac(QFile *media,QDataStream &reader); 
    static bool isAAC(QFile *media,QDataStream &reader);
    static bool isM4A(QFile *media,QDataStream &reader);
    static bool isAPE(QFile *media,QDataStream &reader);
    static bool isVorbis(QFile *media,QDataStream &reader);
    static bool isAU(QFile *media,QDataStream &reader);
public:
    /**
     * 根据指定的URL建立音乐对象
     * @param uri 给定的URL
     * @note 本构造函数**不会**判断URL是否合法
     */
    Music(const QUrl &uri = QUrl());
    Music(const Music &a) = default;
    ///获取包含标题、时间、唱片集、描述的音乐信息
    QString toString();
    /**
     * 比较两个Music是否相等
     * @param a 另一个Music
     * @return 当且仅当满足以下条件之一时返回true：
     * 1.url相同
     * 2.长度，标题均相同
     */
    bool equals(const Music &a) const;
    friend bool operator==(const Music &a,const Music &b) {
        return a.equals(b);
    }
    friend bool operator!=(const Music &a,const Music &b) {
        return !a.equals(b);
    }
    //hash相同，可能不同，但若hash不同，则认为一定不同
    friend uint qHash(const Music &key,uint seed = 0) {
        return qHash(key.title,seed) ^ qHash(key.length,seed);
    }
    //getters
    ///获取Url
    const QUrl &getUrl() const;
    /// 获取标题
    const QString &getTitle() const;
    /// 获取介绍
    const QString &getDcrp() const;
    /// 获取专辑图片 注意：可能不准
    const QUrl &getAlbumImage() const;
    ///获取经格式化后的时间
    QString formatTime();
//    static Music getMediaDetail(const QString &fileName);
    /**
     * @brief 检查文件是否合法
     * @param media  被检查的文件名。
     * @return 当文件>1KB且符合任意一种支持的格式时，返回true，否则返回false
     */
    static bool isLegal(QString media);

};
#endif // MUSIC_H
