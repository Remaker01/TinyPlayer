#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include "framelesswindow.h"
#include "playerbutton.h"
#include "player.h"
#include "playlistview.h"
#include "settingwindow.h"
#include "./ui_playerwindow.h"
#include "searchresultwidget.h"
#ifndef SLOTS
#define SLOTS
#endif
namespace Ui { class PlayerWindow; }
class PlayerWindow : public FramelessWindow {
    Q_OBJECT
private:
    static const QString CONFIG_FILE;
    const QPixmap PLAY_ICON,PAUSE_ICON;
    Ui::PlayerWindow *ui;
    PlayerCore *player;
    SettingWindow *settingWind;
    QSystemTrayIcon *tray;
    QMenu *trayMenu;
    QString lastPath;
    OnlineSeacher *scher;
    SearchResultWidget *res;
    void initUi();
    void initSystemtray();
    void setTitlebar(double opacity);
    void setBackground(const QPixmap &img);
    void changeMode(PlayerCore::PlayMode m);
    void connectSlots();
    void connectUiSlots();
    void ensureExit();
protected:
    void keyReleaseEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *ev);
public:
    explicit PlayerWindow(const QString &arg = QString(),QWidget *parent = nullptr);
    void initConfiguration();
    ~PlayerWindow();
private slots:
    void doAddMedia(QStringList medias);
    void doAddOnlineMedia(const QList<ResultInfo> &medias);
    void on_volumeSlider_valueChanged(int value);
    void on_progressSlider_valueChanged(int value);
    void on_progressSlider_sliderMoved(int position);
    void doDelMedia();
    void on_clearButton_clicked();
    void on_addButton_clicked();
    void on_onlineSearcher_done();
    bool saveList(const QString &file);
    bool openList(const QString &file);
    void moveItem(bool moveUp);
    void on_playView_doubleClicked(const QModelIndex &index);
};
#endif // PLAYERWINDOW_H
