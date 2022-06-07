#include "music.h"
Music::Music(const QUrl &uri):url(uri) {
    static VlcInstance ins(VlcCommon::args());
    VlcMedia m(uri.toString(),&ins);
    m.parse();
    while (!m.parsed())
        QCoreApplication::processEvents();
    length = m.duration();
    VlcMetaManager tmp(&m);
    title = tmp.title() + "\t\t";
    description = tmp.description();
    album = tmp.album();
}

QString Music::toString() {
    return "标题：" + title + '\n' +
            "时长：" + formatTime() + '\n' +
            "唱片集：" + album + '\n' +
            "描述：" + description;
}

bool Music::equals(const Music &a) const {
    return url == a.url||(length == a.length&&title == a.title);
}

const QUrl &Music::getUrl() const {return url;}

QString Music::formatTime() {
    if(!formattedTime.isEmpty())
        return formattedTime;
    int len = qRound(length / 1000.0);
    return formattedTime = QString::number(len / 60)
            + ':' + QString::number(len % 60);
}

Music Music::getMediaDetail(const QString &fileName) {
    if(!isLegal(fileName))
        return Music();
    return Music(QUrl::fromLocalFile(fileName));
}

bool Music::isLegal(QString media) {
    static bool (*checkers[6])(QFile *,QDataStream &) = {
        &Music::isMP3,&Music::isWav,&Music::isAiff,&Music::isFlac,&Music::isAAC,&Music::isWma
    };
    QFile rawData(media);
    if(!rawData.open(QIODevice::ReadOnly)||rawData.size() <= 1024)
        return false;
    QDataStream ds(&rawData);
    for (auto checker : checkers) {
        if(checker(&rawData,ds)) {
            rawData.close();
            return true;
        }
    }
    rawData.close();
    return false;
}
