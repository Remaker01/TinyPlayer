#ifndef MUSIC_H
#define MUSIC_H
#include <QtCore>
#include <QHash>
#include <QMessageBox>
#include <QFile>
#include <VLCQtCore/MetaManager.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
class Music{
private:
    QString formattedTime;
    const QUrl url;
    int length;
    //歌曲名
    QString title,description,album;
    static bool isMP3(QFile *media,QDataStream &reader);
    static bool isWav(QFile *media,QDataStream &reader);
    static bool isWma(QFile *media,QDataStream &reader);
    static bool isAiff(QFile *media,QDataStream &reader);
    static bool isFlac(QFile *media,QDataStream &reader);
    static bool isAAC(QFile *media,QDataStream &reader);
public:
    /**
     * 根据指定的URL建立音乐对象
     * @param uri 给定的URL
     * @note 本构造函数**不会**判断URL是否合法
     */
    Music(const QUrl &uri = QUrl());
    ///获取包含标题、时间、唱片集、描述的音乐信息
    QString toString();
    /**
     * 比较两个Music是否相等
     * @param a 另一个Music
     * @return 当且仅当满足以下条件之一时返回true：
     * 1.url相同
     * 2.文件名，长度，标题均相同
     */
    bool equals(const Music &a) const;
    friend bool operator==(const Music &a,const Music &b) {
        return a.equals(b);
    }
    friend bool operator!=(const Music &a,const Music &b) {
        return !a.equals(b);
    }
    friend uint qHash(const Music &key,uint seed = 0) {
        return qHash(key.title,seed) ^ qHash(key.length,seed) ^ qHash(key.url.fileName(),seed);
    }
    ///获取Url
    const QUrl &getUrl() const;
    ///获取经格式化后的时间
    QString formatTime();
    static Music getMediaDetail(const QString &fileName);
    static bool isLegal(QString media);
signals:

};
#endif // MUSIC_H
