#include "playlistview.h"
#ifndef NDEBUG
#include <QDebug>
#endif
PlayListView::PlayListView(QWidget *parent):QListView(parent) {
    model = new QStringListModel(this);
    setModel(model);
    setAcceptDrops(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setFrameShape(QFrame::Box);

}

void PlayListView::dragEnterEvent(QDragEnterEvent *e) {e->accept();}

void PlayListView::dragMoveEvent(QDragMoveEvent *e) {
    e->setDropAction(Qt::MoveAction);
    e->accept();
}

void PlayListView::dropEvent(QDropEvent *e) {
    //获取拖拽进的文件带路径的文件名
    QList<QUrl> urls(e->mimeData()->urls());
    QStringList medias(QUrl::toStringList(urls,QUrl::PreferLocalFile));
    std::sort(medias.begin(),medias.end());
    emit mediaDropin(medias);
    e->accept();
    QListView::dropEvent(e);
}
#define CONNECT_DOWNLOAD(open) {\
    (open)->setText("下载");\
    connect((open),&QAction::triggered,this,[&,this](){\
        emit downloadRequirement(tmp);\
    });\
}
void PlayListView::contextMenuEvent(QContextMenuEvent *e) {
    QModelIndexList tmp = QListView::selectedIndexes();
    if(!tmp.empty()) {
        QMenu menu(this);
        del = menu.addAction("删除");
        QAction *open = menu.addAction("打开目录");
        connect(del,&QAction::triggered,this,&PlayListView::itemDelRequirement);
        if(tmp.size() == 1) {
            int row = tmp[0].row();
            QAction *showDetail = menu.addAction("详细信息");
            if(!playList[row].contains("[线上音乐]")) {
                connect(open,&QAction::triggered,this,[&,this]() {
                   emit openRequirement(row);
                });
            }
            else    CONNECT_DOWNLOAD(open)
            connect(showDetail,&QAction::triggered,this,[&,this](){
               emit showDetailRequirement(row);
            });
        }
        else {
            bool hasLocal = false;
            for (QModelIndex &index:tmp) {
                if(!playList[index.row()].contains("[线上音乐]")) {
                    hasLocal = true;
                    break;
                }
            }
            if(!hasLocal)    CONNECT_DOWNLOAD(open)
        }
        menu.exec(QCursor::pos());
    }
    QListView::contextMenuEvent(e);
}
#undef CONNECT_DOWNLOAD
QModelIndexList PlayListView::getSelections() {return QListView::selectedIndexes();}

void PlayListView::setOpacity(double value) {
    int a = std::min(255,qRound(256*value));
    setStyleSheet("background-color: rgba(255,255,255," + QString::number(a) + ");");
}

void PlayListView::commitChange() {model->setStringList(playList);}

QStringList &PlayListView::list() {return playList;}

PlayListView::~PlayListView() {}
