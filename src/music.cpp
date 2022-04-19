#include "music.h"
Music::Music(const QUrl &uri):url(uri) {
    QMediaPlayer tmp;
    tmp.setMedia(QMediaContent(url));
    while(!tmp.isMetaDataAvailable())
        QCoreApplication::processEvents();
    length = tmp.duration();
    title = tmp.metaData("Title").toString() + "\t\t";
    description = tmp.metaData("Description").toString();
    album = tmp.metaData(QStringLiteral("AlbumTitle")).toString();
}

QString Music::toString() {
    return "标题：" + title + '\n' +
            "时长：" + formatTime() + '\n' +
            "唱片集：" + album + '\n' +
            "描述：" + description;
}

bool Music::equals(const Music &a) const {
    return url == a.url||
            (title == a.title&&
             length == a.length);
}

const QUrl &Music::getUrl() const {return url;}

QString Music::formatTime() {
    if(!formattedTime.isEmpty())
        return formattedTime;
    int len = qRound(length / 1000.0);
    return formattedTime = QString::number(len / 60)
            + ':' + QString::number(len % 60);
}

bool Music::isLegal(const QString &media) {
    QFile rawData(media);
    if(!rawData.open(QIODevice::ReadOnly)||rawData.size() <= 1024)
        return false;
    uint32_t size = rawData.size();
    QDataStream ds(&rawData);
    return isMP3(&rawData,ds,size)||isWav(&rawData,ds,size)||isWma(&rawData,ds)||isAiff(&rawData,ds,size);
}
#define RETURN(CONDITION) {\
    media->seek(0ll);\
    return (CONDITION);\
}
//判断方式：头部为"ID3"，或倒数128字节起为"TAG"，或头11位为1
bool Music::isMP3(QFile *media,QDataStream &reader,uint32_t size) {
    reader.setByteOrder(QDataStream::BigEndian);
    char head[5];
    head[4] = 0;
    reader.readRawData(head,4);
    QString str(head);
    if(str.left(3) == "ID3")   RETURN(str[3] >= '\02'&&str[3] <= '\04')
    bool ret = false;
    media->seek(size - 128);
    reader.readRawData(head,3);
    str = head;
    if(str == "TAG")   RETURN(true)
    //若以上均不满足,则应满足：头11位为1,即第一字节为FF，第二字节不小于E0
    media->seek(0ll);
    uint16_t first_2B;
    reader >> first_2B;
    //按大端序，这样写是对的？
    if(first_2B >= 0xffe0u)
        ret = true;
    media->seek(0ll);
    return ret;
}
//头部格式："RIFF"+文件大小+"WAVE"+"fmt "
bool Music::isWav(QFile *media,QDataStream &reader,uint32_t size) {
    //注意改为小端序
    reader.setByteOrder(QDataStream::LittleEndian);
    char head[9];
    head[4] = head[8] = 0;
    uint32_t sizePart;
    //头4个字节
    reader.readRawData(head,4);
    QString str1(head);
    if(str1 != "RIFF")   RETURN(false)
    //接下来4字节
    reader >> sizePart;
    if(sizePart != size - 8)   RETURN(false)
    //接下来8字节
    reader.readRawData(head,8);
    QString str2(head);
    RETURN(str2 == "WAVEfmt ")
}
//头部格式：前16B为30 26 B2 75 8E 66 CF 11 A6 D9 00 AA 00 62 CE 6C
bool Music::isWma(QFile *media,QDataStream &reader) {
    reader.setByteOrder(QDataStream::LittleEndian);
    static constexpr int BASE = 4;
    static const uint32_t ss[BASE] = {0x75b22630u,0x11cf668eu,0xaa00d9a6u,0x6cce6200u};
    uint32_t now;
    for(int i = 0; i < BASE; i++) {
        reader >> now;
        if(now != ss[i])    RETURN(false)
    }
    RETURN(true)
}
//头部：46 4F 52 4D，即"FORM";大端序
bool Music::isAiff(QFile *media,QDataStream &reader,uint32_t size) {
    reader.setByteOrder(QDataStream::BigEndian);
    char head[5];
    head[4] = 0;
    uint32_t sizePart;
    //0~3字节
    reader.readRawData(head,4);
    QString str1(head);
    if(str1 != "FORM")   RETURN(false)
    reader >> sizePart;
    if(sizePart != size - 8)   RETURN(false)
    //8~11字节
    reader.readRawData(head,4);
    QString str2(head);
    RETURN(str2 == "AIFF"||str2 == "AIFC")
}
#undef RETURN
