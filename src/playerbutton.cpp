#include "playerbutton.h"
PlayerButton::PlayerButton(QWidget *parent){
    setParent(parent);
    setScaledContents(true);
    setCursor(Qt::PointingHandCursor);
}

void PlayerButton::setReplyClick(bool f) {replyClick = f;}
void PlayerButton::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton&&replyClick) {
        int wi = width(),he = height();
        w = wi,h = he;
        wi = wi - (wi >> 3);
        he = he - (he >> 3);
        resize(wi,he);
        move(x() + (w/10-1),y() + h/10);
    }
}

void PlayerButton::mouseReleaseEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton&&replyClick) {
        emit clicked();
        resize(w,h);
        move(x() - (w/10-1),y() - h/10);
    }
}

PlayerButton::~PlayerButton() {}
