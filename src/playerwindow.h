#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QPalette>
#include <QDesktopServices>
#include <QSettings>
#include <QProgressDialog>
#include <QSystemTrayIcon>
#include <QKeyEvent>
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
    static const QString CONFIG_FILE;
    const QPixmap PLAY_ICON,PAUSE_ICON;
    Ui::PlayerWindow *ui;
    PlayerCore *player;
    QSystemTrayIcon *tray;
    QMenu *trayMenu;
    QStringList playList;
    QString lastPath;
    void initUi();
    void ensureExit();
    void initPlayList();
    void initSystemtray();
    void setBackground(const QPixmap &img);
    void changeMode(PlayerCore::PlayMode m);
    void connectSlots();
    void connectUiSlots();
protected:
    void keyReleaseEvent(QKeyEvent *e);
public:
    explicit PlayerWindow(QWidget *parent = nullptr);
    void initConfiguration();
    ~PlayerWindow();

private slots:
    void doAddMedia(QStringList medias);
    void on_volumeSlider_valueChanged(int value);
    void on_progressSlider_valueChanged(int value);
    void on_progressSlider_sliderMoved(int position);

    void on_listView_doubleClicked(const QModelIndex &index);
    void doDelMedia();
    void on_clearButton_clicked();
    void on_addButton_clicked();
    bool saveList(const QString &file);
    bool openList(const QString &file);
};
#endif // PLAYERWINDOW_H
