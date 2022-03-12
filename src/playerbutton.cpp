#include "playerbutton.h"

PlayerButton::PlayerButton(int W,int H,QWidget *parent):w(W),h(H){
    setParent(parent);
    setScaledContents(true);
    setCursor(Qt::PointingHandCursor);
    resize(W,H);
}

void PlayerButton::setReplyClick(bool f) {replyClick = f;}
void PlayerButton::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton&&replyClick) {
        int wi = width(),he = height();
        wi = wi - (wi >> 3);
        he = he - (he >> 3);
        resize(wi,he);
        move(x() + (w/10-1),y() + h/10);
    }
}

void PlayerButton::mouseReleaseEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton&&replyClick) {
        //当前仍未播放
        emit clicked();
        resize(w,h);
        move(x() - (w/10-1),y() - h/10);
    }
}

PlayerButton::~PlayerButton() {}
