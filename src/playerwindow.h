#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QPalette>
#include <QDesktopServices>
#include "playerbutton.h"
#include "player.h"

namespace Ui { class PlayerWindow; }
class PlayerWindow : public QMainWindow {
    Q_OBJECT
private:
    Ui::PlayerWindow *ui;
    PlayerButton *playButton;
    PlayerCore *player;
    void ensureExit();
    void setBackground();
    void registerSlots();
    void setIcon(bool needOperation = true);
public:
    PlayerWindow(QWidget *parent = nullptr);
    ~PlayerWindow();

private slots:
    void on_closeButton_clicked();
    void on_volumeSlider_valueChanged(int value);
    void on_progressSlider_valueChanged(int value);
    void on_progressSlider_sliderMoved(int position);
};
#endif // PLAYERWINDOW_H
