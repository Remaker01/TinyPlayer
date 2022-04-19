#ifndef MUSIC_H
#define MUSIC_H
#include <QtCore>
#include <QMediaPlayer>
#include <QHash>
#include <QMessageBox>
#include <QFile>
class Music{
private:
    QString formattedTime;
    const QUrl url;
    int length;
    //歌曲名
    QString title,description,album;
    static bool isMP3(QFile *media,QDataStream &reader,uint32_t size);
    static bool isWav(QFile *media,QDataStream &reader,uint32_t size);
    static bool isWma(QFile *media,QDataStream &reader);
    static bool isAiff(QFile *media,QDataStream &reader,uint32_t size);
public:
    /**
     * @brief Music 根据指定的URL建立音乐文件
     * @param uri 给定的URL
     * @note 本构造函数**不会**判断URL是否合法
     */
    Music(const QUrl &uri = QUrl());
    ///获取包含标题、时间、唱片集、描述的音乐信息
    QString toString();
    ///比较是否相等。若Url相同，或Url不同但长度、标题均相同
    bool equals(const Music &a) const;
    friend bool operator==(const Music &a,const Music &b) {
        return a.equals(b);
    }
    friend bool operator!=(const Music &a,const Music &b) {
        return !a.equals(b);
    }
    friend uint qHash(const Music &key,uint seed = 0) {
        return qHash(key.url,seed) ^ qHash(key.length,seed);
    }
    ///获取Url
    const QUrl &getUrl() const;
    ///获取经格式化后的时间
    QString formatTime();
    static bool isLegal(const QString &media);
signals:

};

#endif // MUSIC_H
