#include "playerbutton.h"

PlayerButton::PlayerButton(int w,int h,QWidget *parent){
    setParent(parent);
    setScaledContents(true);
    setCursor(Qt::PointingHandCursor);
    resize(w,h);
    this->w = w;
    this->h = h;
}

void PlayerButton::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton) {
        int wi = width(),he = height();
        wi = wi - (wi >> 3);
        he = he - (he >> 3);
        resize(wi,he);
        int baseW = w / 10 - 1,baseH = h / 10;
        move(x() + baseW,y() + baseH);
    }
}

void PlayerButton::mouseReleaseEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton) {
        //当前仍未播放
        emit clicked();
        resize(w,h);
        int baseW = w / 10 - 1,baseH = h / 10;
        move(x() - baseW,y() - baseH);

    }
}

PlayerButton::~PlayerButton() {}
