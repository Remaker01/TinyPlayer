#include "playerslider.h"
#ifndef NDEBUG
#include <QDebug>
#endif
PlayerSlider::PlayerSlider(QWidget *parent):QSlider(parent) {
    setCursor(Qt::PointingHandCursor);
    setPageStep(0);
    QFile qss(":/Icons/images/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)) {
        QString style = qss.readAll();
        setStyleSheet(style);
    }
    qss.close();
    setFocusPolicy(Qt::NoFocus);
}

void PlayerSlider::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton&&maximum() > 0) {
        double r = e->x() / (double)width();
        int loc = qRound(r * maximum());
        setValue(loc);
        emit playerSliderClicked(loc);
        QSlider::mousePressEvent(e);
    }
}
