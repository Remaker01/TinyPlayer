#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QPalette>
#include <QDesktopServices>
#include "play.h"

namespace Ui { class PlayerWindow; }
class PlayerWindow : public QMainWindow {
    Q_OBJECT
    Player *playButton = nullptr;
    void registerSlots();
    void ensureExit();
public:
    PlayerWindow(QWidget *parent = nullptr);
    ~PlayerWindow();

private slots:
    void on_closeButton_clicked();
    void on_volumeSlider_valueChanged(int value);
    void on_progressSlider_valueChanged(int value);
    void on_progressSlider_sliderMoved(int position);

private:
    Ui::PlayerWindow *ui;
};
#endif // PLAYERWINDOW_H
