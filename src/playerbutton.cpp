#include "playerbutton.h"
#include <QDebug>
PlayerButton::PlayerButton(QWidget *parent):QLabel(parent) {
    setScaledContents(true);
    setCursor(Qt::PointingHandCursor);
}

void PlayerButton::setClickable(bool f) {clickable = f;}

void PlayerButton::mousePressEvent(QMouseEvent *e) {
    w = width(),h = height();
    if(e->button() == Qt::LeftButton&&clickable) {
        int wi = width(),he = height();
        wi = wi - (wi >> 3);
        he = he - (he >> 3);
        resize(wi,he);
        move(x() + ((w >> 3) -1),y() + (h >> 3)); //除缩小外，还要向右、向下移动
    }
}

void PlayerButton::mouseReleaseEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton&&clickable) {
        resize(w,h);
        move(x() - ((w >> 3) -1),y() - (h >> 3));
        emit clicked();
    }
}

void PlayerButton::changeState(const QString &toolTip, const QPixmap &pixmap) {
    setPixmap(pixmap);
    setToolTip(toolTip);
}

void PlayerButton::click() {
    if(clickable)
        emit clicked();
}
PlayerButton::~PlayerButton() {}
