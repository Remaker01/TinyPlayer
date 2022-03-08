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
        static int baseW = w / 10 - 1,baseH = h / 10;
        move(x() + baseW,y() + baseH);
    }
}

void PlayerButton::mouseReleaseEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton&&replyClick) {
        //当前仍未播放
        emit clicked();
        resize(w,h);
        static int baseW = w / 10 - 1,baseH = h / 10;
        move(x() - baseW,y() - baseH);

    }
}

PlayerButton::~PlayerButton() {}
