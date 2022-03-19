#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QPalette>
#include <QDesktopServices>
#include <QStringListModel>
#include <QMimeData>
#include "playerbutton.h"
#include "player.h"

namespace Ui { class PlayerWindow; }
class PlayerWindow : public QMainWindow {
    Q_OBJECT
private:
    const QPixmap PLAY_ICON,PAUSE_ICON;
    Ui::PlayerWindow *ui;
    PlayerButton *playButton,*stopButton;
    PlayerCore *player;
    QStringListModel *playListModel;
    //QUrl:只获取文件名，QFile::fileName:获取文件名及目录名
    //因此这个list里都是带目录名的
    QStringList playList;
    //当前正在播放
    int selected = -1;
    void ensureExit();
    void initUi();
    void setBackground(const QPixmap &img);
    void initPlayList();
    void setButton(PlayerButton *button,const QPixmap &pic,const QPoint &loc);
    void connectSlots();
    void doAddMedia(QStringList medias);
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
public:
    PlayerWindow(QWidget *parent = nullptr);
    ~PlayerWindow();

private slots:
    void on_volumeSlider_valueChanged(int value);
    void on_progressSlider_valueChanged(int value);
    void on_progressSlider_sliderMoved(int position);
    void on_listView_doubleClicked(const QModelIndex &index);
    void on_listView_clicked(const QModelIndex &index);
    void on_delButton_clicked();
    void on_clearButton_clicked();
};
#endif // PLAYERWINDOW_H
