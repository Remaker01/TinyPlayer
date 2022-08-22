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

void PlayListView::contextMenuEvent(QContextMenuEvent *e) {
    QModelIndexList tmp = QListView::selectedIndexes();
    if(!tmp.empty()) {
        QMenu menu(this);
        del = menu.addAction("删除");
        connect(del,&QAction::triggered,this,&PlayListView::itemDelRequirement);
        if(tmp.size() == 1) {
            int row = tmp[0].row();
            QAction *open = menu.addAction("打开目录"),*showDetail = menu.addAction("详细信息");
            if(playList[row].contains("[线上音乐]"))
                open->setEnabled(false);
            connect(open,&QAction::triggered,this,[&,this]() {
               emit openDirRequirement(row);
            });
            connect(showDetail,&QAction::triggered,this,[&,this](){
               emit showDetailRequirement(row);
            });
            open->deleteLater();
            showDetail->deleteLater();
        }
        menu.exec(QCursor::pos());
    }
    QListView::contextMenuEvent(e);
}

QModelIndexList PlayListView::getSelections() {return QListView::selectedIndexes();}

void PlayListView::setOpacity(double value) {
    int a = std::min(255,qRound(256*value));
    setStyleSheet("background-color: rgba(255,255,255," + QString::number(a) + ");");
}

void PlayListView::commitChange() {model->setStringList(playList);}

QStringList &PlayListView::list() {return playList;}

PlayListView::~PlayListView() {}
