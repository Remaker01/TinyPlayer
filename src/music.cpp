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
    if(QString::compare(title,url.toString() + "\t\t",Qt::CaseInsensitive) == 0)
        title = url.fileName();
    description = tmp.description();
    album = tmp.album();
    albumImage = tmp.artwork();
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

const QString &Music::getTitle() const {return title;}

const QString &Music::getDcrp() const {return description;}

const QUrl &Music::getAlbumImage() const {return albumImage;}

QString Music::formatTime() {
    if(!formattedTime.isEmpty())
        return formattedTime;
    int len = qRound(length / 1000.0);
    return formattedTime = QString::number(len / 60)
            + ':' + QString::number(len % 60);
}

bool Music::isLegal(QString media) {
    static auto checkers = {
        &Music::isMPEG,&Music::isWav,&Music::isFlac,&Music::isAAC,&Music::isWma,
        &Music::isM4A,&Music::isAiff,&Music::isAPE,&Music::isVorbis,&Music::isAU
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
