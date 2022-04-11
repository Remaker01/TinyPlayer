#include "music.h"
Music::Music(const QUrl &uri):url(uri) {
    QMediaPlayer tmp;
    tmp.setMedia(QMediaContent(url));
    while(!tmp.isMetaDataAvailable())
        QCoreApplication::processEvents();
    length = tmp.duration();
    title = tmp.metaData("Title").toString();
    description = tmp.metaData("Description").toString();
}

QString Music::toString() const{
    int tmp = length / 1000;
    QString len = QString::number(tmp / 60)
            + ':' + QString::number(tmp % 60);
    return "标题：" + title + '\n' +
            "时长：" + len + '\n' +
            "描述：" + description;
}

bool Music::equals(const Music &a) const {
    return url == a.url||
            (title == a.title&&
             length == a.length);
}

const QUrl &Music::getUrl() {return url;}

bool Music::isLegal(const QString &media) {
    QFile rawData(media);
    if(!rawData.open(QIODevice::ReadOnly)||rawData.size() <= 1024)
        return false;
    int size = rawData.size();
    return isMP3(&rawData,size)||isWav(&rawData,size)||isWma(&rawData)||isAiff(&rawData,size);
}
#define RETURN(POINTER_NAME,VALUE) {\
    delete [] (POINTER_NAME);\
    media->seek(0ll);\
    return (VALUE);\
}
//判断方式：头部为"ID3"，或倒数128字节起为"TAG"，或头11位为1
bool Music::isMP3(QFile *media,uint32_t size) {
    QDataStream reader(media);
    reader.setByteOrder(QDataStream::BigEndian);
    char *head = new char[5];
    head[4] = 0;
    reader.readRawData(head,4);
    QString str(head);
    if(str.left(3) == "ID3")   RETURN(head,str[3] >= '\02'&&str[3] <= '\04')
    bool ret = false;
    media->seek(size - 128);
    reader.readRawData(head,3);
    str = head;
    if(str == "TAG")   RETURN(head,true)
    //若以上均不满足,则应满足：头11位为1,即第一字节为FF，第二字节不小于E0
    media->seek(0ll);
    uint16_t first_2B;
    reader >> first_2B;
    //按大端序，这样写是对的？
    if(first_2B >= 0xffe0u)
        ret = true;
    delete [] head;
    media->seek(0ll);
    return ret;
}
//头部格式："RIFF"+文件大小+"WAVE"+"fmt "
bool Music::isWav(QFile *media,uint32_t size) {
    QDataStream reader(media);
    //注意改为小端序
    reader.setByteOrder(QDataStream::LittleEndian);
    char *head = new char[9];
    head[4] = head[8] = 0;
    uint32_t sizePart;
    //头4个字节
    reader.readRawData(head,4);
    QString str1(head);
    if(str1 != "RIFF")   RETURN(head,false)
    //接下来4字节
    reader >> sizePart;
    if(sizePart != size - 8)   RETURN(head,false)
    //接下来7字节
    reader.readRawData(head,8);
    QString str2(head);
    delete [] head;
    media->seek(0ll);
    return str2 == "WAVEfmt ";
}
//头部格式：前16B为30 26 B2 75 8E 66 CF 11 A6 D9 00 AA 00 62 CE 6C
bool Music::isWma(QFile *media) {
    QDataStream reader(media);
    reader.setByteOrder(QDataStream::LittleEndian);
    static constexpr int BASE = 4;
    static const uint32_t ss[BASE] = {0x75b22630u,0x11cf668eu,0xaa00d9a6u,0x6cce6200u};
    uint32_t *first_16B = new uint32_t[BASE];
    for(int i = 0; i < BASE; i++) {
        reader >> first_16B[i];
    }
    for(int i = 0; i < BASE; i++) {
        if(first_16B[i] != ss[i])   RETURN(first_16B,false)
    }
    delete [] first_16B;
    media->seek(0ll);
    return true;
}
//头部：46 4F 52 4D，即"FROM";大端序
bool Music::isAiff(QFile *media,uint32_t size) {
    QDataStream reader(media);
    reader.setByteOrder(QDataStream::BigEndian);
    char *head = new char[5];
    head[4] = 0;
    uint32_t sizePart;
    //0~3字节
    reader.readRawData(head,4);
    QString str1(head);
    if(str1 != "FROM")   RETURN(head,false)
    reader >> sizePart;
    if(sizePart != size - 8)   RETURN(head,false)
    //8~11字节
    reader.readRawData(head,4);
    QString str2(head);
    delete [] head;
    media->seek(0ll);
    return str2 == "AIFF"||str2 == "AIFC";
}
#undef RETURN
