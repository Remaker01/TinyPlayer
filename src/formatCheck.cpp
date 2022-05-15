#include "music.h"
#define RETURN(CONDITION) {\
    media->seek(0ll);\
    return (CONDITION);\
}
//判断方式：头部为"ID3"，或倒数128字节起为"TAG"，或头11位为1
bool Music::isMP3(QFile *media,QDataStream &reader) {
    reader.setByteOrder(QDataStream::BigEndian);
    char head[5];
    head[4] = 0;
    reader.readRawData(head,4);
    QString str(head);
    if(str.left(3) == "ID3")   RETURN(str[3] >= '\02'&&str[3] <= '\04')
    media->seek(media->size() - 128);
    reader.readRawData(head,3);
    str = head;
    if(str == "TAG")   RETURN(true)
    //若以上均不满足,则应满足：头11位为1,即第一字节为FF，第二字节不小于E0
    media->seek(0ll);
    uint16_t first_2B;
    reader >> first_2B;
    //按大端序，这样写是对的？
    RETURN(first_2B >= 0xffe0u)
}
//头部格式："RIFF"+文件大小+"WAVE"+"fmt "
bool Music::isWav(QFile *media,QDataStream &reader) {
    //注意改为小端序
    reader.setByteOrder(QDataStream::LittleEndian);
    uint32_t size = media->size();
    char head[9];
    head[4] = head[8] = 0;
    uint32_t sizePart;
    //头4个字节
    reader.readRawData(head,4);
    QString str(head);
    if(str != "RIFF")   RETURN(false)
    //接下来4字节
    reader >> sizePart;
    if(sizePart != size - 8)   RETURN(false)
    //接下来8字节
    reader.readRawData(head,8);
    str = head;
    RETURN(str == "WAVEfmt ")
}
//头部：46 4F 52 4D，即"FORM";大端序
bool Music::isAiff(QFile *media,QDataStream &reader) {
    reader.setByteOrder(QDataStream::BigEndian);
    uint32_t size = media->size();
    char head[5];
    head[4] = 0;
    uint32_t sizePart;
    //0~3字节
    reader.readRawData(head,4);
    QString str(head);
    if(str != "FORM")   RETURN(false)
    reader >> sizePart;
    if(sizePart != size - 8)   RETURN(false)
    //8~11字节
    reader.readRawData(head,4);
    str = head;
    RETURN(str == "AIFF"||str == "AIFC")
}
//头部fLaC
bool Music::isFlac(QFile *media, QDataStream &reader) {
    char head[5];
    head[4] = 0;
    reader.readRawData(head,4);
    if(QString(head) != "fLaC")    RETURN(false)
    uint8_t t;
    reader >> t;
    t &= 0x7f;  //最高位清0,BLOCK_TYPE∈[0,6]
    RETURN(t <= 0x06)
}
//头部0xfff或ADIF
bool Music::isAAC(QFile *media, QDataStream &reader) {
    reader.setByteOrder(QDataStream::BigEndian);
    char head[5];
    head[4] = 0;
    reader.readRawData(head,4);
    //ADIF
    if(QString(head) == "ADIF")    RETURN(true)
    //ADTS
    uint16_t x;
    reader >> x;
    RETURN(x >= 0xfff0)
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
#undef RETURN
