#include "playerslider.h"
#ifndef NDEBUG
#include <QDebug>
#endif
PlayerSlider::PlayerSlider(QWidget *parent) {
    setCursor(Qt::PointingHandCursor);
    setPageStep(0);
}

void PlayerSlider::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton&&maximum() > 0) {
        double r = e->x() / (double)width();
        int loc = qRound(r * maximum());
        setValue(loc);
#ifndef NDEBUG
        qDebug() << objectName() << "clicked on" << loc;
#endif
        emit playerSliderClicked(loc);
        QSlider::mousePressEvent(e);
    }
}
