#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QPalette>
#include <QDesktopServices>
#include <QStringListModel>
#include <QMimeData>
#include <QSettings>
#include <QProcess>
#include <QSystemTrayIcon>
#include "playerbutton.h"
#include "player.h"
#include "./ui_playerwindow.h"
#ifndef SLOTS
#define SLOTS
#endif
namespace Ui { class PlayerWindow; }
class PlayerWindow : public QMainWindow {
    Q_OBJECT
private:
    const QPixmap PLAY_ICON,PAUSE_ICON;
    Ui::PlayerWindow *ui;
    PlayerButton *playButton,*stopButton;
    PlayerCore *player;
    QStringListModel *playListModel;
    QSystemTrayIcon *tray;
    //QUrl:只获取文件名，QFile::fileName:获取文件名及目录名
    //因此这个list里都是带目录名的
    QStringList playList;
    QString lastPath;
    //当前正在播放
    void initUi();
    void ensureExit();
    void initPlayList();
    void initSystemtray();
    void setBackground(const QPixmap &img);
    void setButton(PlayerButton *button,const QPixmap &pic,const QPoint &loc);
    void initConfiguration();
    void connectSlots();
    void connectUiSlots();
    //void scanAndAdd(const QDir &dir);
public:
    PlayerWindow(QWidget *parent = nullptr);
    ~PlayerWindow();

private slots:
    void doAddMedia(QStringList medias);
    void on_volumeSlider_valueChanged(int value);
    void on_progressSlider_valueChanged(int value);
    void on_progressSlider_sliderMoved(int position);
    /*
     * 双击后：
     * 1.触发QStringListView::doubleClicked信号，调用此槽函数。
     * 2.调用PlayerCore::setCurrentMediaIndex，修改当前媒体为当前行对应文件，触发QMediaPlayer::durationChanged信号。
     */
    void on_listView_doubleClicked(const QModelIndex &index);
    void doDelMedia();
    void on_clearButton_clicked();
};
#endif // PLAYERWINDOW_H
