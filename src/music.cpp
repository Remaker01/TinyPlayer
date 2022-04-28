#include "music.h"

Music::Music(const QUrl &uri):url(uri) {
    static VlcInstance *ins = new VlcInstance(VlcCommon::args());
    VlcMedia m(uri.toString(),ins);
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
    QString name1(url.fileName()),name2(url.fileName());
    return url == a.url||
#ifndef Q_OS_LINUX
            (name1.compare(name2,Qt::CaseInsensitive) == 0
             &&length == a.length&&title == a.title);
#else
            (name1 == name2
             &&length == a.length&&title == a.title);
#endif
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
    if(!Music::isLegal(fileName))
        return Music();
    return Music(QUrl::fromLocalFile(fileName));
}

bool Music::isLegal(QString media) {
    QFile rawData(media);
    if(!rawData.open(QIODevice::ReadOnly)||rawData.size() <= 1024)
        return false;
    QDataStream ds(&rawData);
    uint32_t size = media.size();
    return isMP3(&rawData,ds,size)||
            isWav(&rawData,ds,size)||
            isAiff(&rawData,ds,size)||
            isFlac(&rawData,ds)||
            isAAC(&rawData,ds)||
            isWma(&rawData,ds);
}
