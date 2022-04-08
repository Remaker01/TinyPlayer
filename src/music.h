#ifndef MUSIC_H
#define MUSIC_H
#include <QtCore>
#include <QMediaPlayer>
#include <QHash>
#include <QMessageBox>
#include <QFile>
class Music{
private:
    static bool isMP3(QFile *media,uint32_t size);
    static bool isWav(QFile *media,uint32_t size);
    static bool isWma(QFile *media);
    static bool isAiff(QFile *media,uint32_t size);
public:
    /**
     * @brief Music 根据指定的URL建立音乐文件
     * @param uri 给定的URL
     * @note 本构造函数**不会**判断URL是否合法
     */
    Music(const QUrl &uri = QUrl());
    const QUrl url;
    int length;
    //歌曲名
    QString title;
    QString description;
    QString toString() const;
    bool equals(const Music &a) const;
    friend bool operator==(const Music &a,const Music &b) {
        return a.equals(b);
    }
    friend uint qHash(const Music &key,uint seed = 0) {
        return qHash(key.url,seed) ^ qHash(key.length,seed);
    }
    const QUrl &getUrl();
    static bool isLegal(const QString &media);
signals:

};

#endif // MUSIC_H
