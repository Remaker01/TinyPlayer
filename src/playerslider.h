#ifndef PLAYERSLIDER_H
#define PLAYERSLIDER_H
#define __WIDGET_NEEDED__
#include "global_include.h"
class PlayerSlider :public QSlider{
    Q_OBJECT
public:
    explicit PlayerSlider(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *e);
signals:
    void playerSliderClicked(int loc);
};

#endif // PLAYERSLIDER_H
