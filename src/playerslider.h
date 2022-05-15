#ifndef PLAYERSLIDER_H
#define PLAYERSLIDER_H
#include <QSlider>
#include <QMouseEvent>
#include "player.h"
class PlayerSlider :public QSlider{
    Q_OBJECT
public:
    explicit PlayerSlider(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *e);
signals:
    void playerSliderClicked(int loc);
};

#endif // PLAYERSLIDER_H
