#include "music.h"
Music::Music(const QUrl &uri, const QString &altername):url(uri) {
    static VlcInstance ins(VlcCommon::args());
    VlcMedia m(uri.toString(),&ins);
	m.parse();
    while (!m.parsed())
        QCoreApplication::processEvents();
    length = m.duration();
    VlcMetaManager tmp(&m);
    title = tmp.title();
    if(!url.isLocalFile()) {
        title = url.fileName();
        alterName = altername;
    }
    description = tmp.description();
	artist = tmp.artist();
    albumImage = tmp.artwork();
}

QString Music::toString() {
    return "标题：" + title + "\t\t\n" +
            "时长：" + formatTime() + '\n' +
			"歌手：" + artist + '\n' +
            "描述：" + description;
}

bool Music::equals(const Music &a) const {
    if(url.isLocalFile())
        return url == a.url||(length == a.length&&title == a.title);
    return url == a.url;
}

const QUrl &Music::getUrl() const {return url;}

QString Music::getTitle() const {
    if(!url.isLocalFile()&&!alterName.isEmpty())
        return alterName;
    return title;
}

QString Music::getDcrp() const {return description;}

QUrl Music::getAlbumImage() const {return albumImage;}

int Music::getLength() const {return length;}

QString Music::formatTime() {
    if(!formattedTime.isEmpty())
        return formattedTime;
    int len = qRound(length / 1000.0);
    return formattedTime = QString::number(len / 60)
            + ':' + QString::number(len % 60);
}

void Music::setLength(int length) {
    if(!isOnlineMusic()) {
        return;
    }
    this->length = std::min(length,0);
    this->formattedTime = "";
}

bool Music::isOnlineMusic() const {
    return url.scheme().startsWith("http",Qt::CaseInsensitive);
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
