#include "framelesswindow.h"
#ifndef NDEBUG
#include <QDebug>
#endif
FramelessWindow::FramelessWindow(QWidget *par) : QWidget(par),leftBtnPressed(false),dir(NONE) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground);
    setMouseTracking(true);
}

void FramelessWindow::region(const QPoint &currentLoc,const QRect &_rect) {
    QPoint topLeft = mapToGlobal(_rect.topLeft()); //将左上角的(0,0)转化为全局坐标
    QPoint rightButtom = mapToGlobal(_rect.bottomRight());

    int x = currentLoc.x(); //当前鼠标的坐标
    int y = currentLoc.y();

//    if(((topLeft.x() + PADDING >= x) && (topLeft.x() <= x))
//            && ((topLeft.y() + PADDING >= y) && (topLeft.y() <= y))){
//        // 左上角
//        dir = LEFT_TOP;
//        QWidget::setCursor(QCursor(Qt::SizeFDiagCursor));  // 设置光标形状
//    }
    if(((x >= rightButtom.x() - PADDING) && (x <= rightButtom.x()))
              && ((y >= rightButtom.y() - PADDING) && (y <= rightButtom.y()))) {
        // 右下角
        dir = RIGHT_DOWN;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
//    else if(((x <= topLeft.x() + PADDING) && (x >= topLeft.x()))
//              && ((y >= rightButtom.y() - PADDING) && (y <= rightButtom.y()))) {
//        //左下角
//        dir = LEFT_DOWN;
//        QWidget::setCursor(QCursor(Qt::SizeBDiagCursor));
//    }
    else if(((x <= rightButtom.x()) && (x >= rightButtom.x() - PADDING))
              && ((y >= topLeft.y()) && (y <= topLeft.y() + PADDING))) {
        // 右上角
        dir = RIGHT_TOP;
        QWidget::setCursor(QCursor(Qt::SizeBDiagCursor));
    }
//    else if((x <= topLeft.x() + PADDING) && (x >= topLeft.x())) {
//        // 左边
//        dir = LEFT;
//        QWidget::setCursor(QCursor(Qt::SizeHorCursor));
//    }
    else if((x <= rightButtom.x()) && (x >= rightButtom.x() - PADDING)) {
        // 右边
        dir = RIGHT;
        QWidget::setCursor(QCursor(Qt::SizeHorCursor));
    }
//    else if((y >= topLeft.y()) && (y <= topLeft.y() + PADDING)){
//        // 上边
//        dir = UP;
//        QWidget::setCursor(QCursor(Qt::SizeVerCursor));
//    }
    else if((y <= rightButtom.y()) && (y >= rightButtom.y() - PADDING)){
        // 下边
        dir = DOWN;
        QWidget::setCursor(QCursor(Qt::SizeVerCursor));
    }
    else {
        // 默认
        dir = NONE;
        QWidget::setCursor(QCursor(Qt::ArrowCursor));
    }
}

void FramelessWindow::setCloseButton(QAbstractButton *closeBtn) {
    if(closeBtn != nullptr)
        connect(closeBtn,&QAbstractButton::clicked,this,&QWidget::close);
}

void FramelessWindow::setMinimizeButton(QAbstractButton *minBtn) {
    if(minBtn != nullptr)
        connect(minBtn,&QAbstractButton::clicked,this,&QWidget::showMinimized);
}

void FramelessWindow::setMaxmizeButton(QAbstractButton *maxBtn) {
    if(maxBtn != nullptr)
        connect(maxBtn,&QAbstractButton::clicked,this,&QWidget::showMaximized);
}

void FramelessWindow::setMenu(QList<QAction *> actions, const QString &style) {
    if(menu == nullptr)
        menu = new QMenu(this);
    menu->clear();
    menu->addActions(actions);
    menu->setStyleSheet(style);
}

QMenu *FramelessWindow::getMenu() {
    return menu;
}

void FramelessWindow::mousePressEvent(QMouseEvent *ev) {
    if(ev->button() == Qt::LeftButton) {
        leftBtnPressed = true;
        if(dir == NONE) {
            mouseLoc = ev->globalPos() - frameGeometry().topLeft();
            //globalPos()鼠标位置，topLeft()窗口左上角的位置
        }
    }
    QWidget::mousePressEvent(ev);
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *ev) {
    QPoint globalPoint = ev->globalPos();   //鼠标全局坐标
    QRect _rect = QWidget::rect();  //rect == QRect(0,0 1280x720)
    QPoint topLeft = mapToGlobal(_rect.topLeft());
    QPoint bottomRight = mapToGlobal(_rect.bottomRight());

    if(!leftBtnPressed) {  //没有按下左键时
        region(globalPoint,_rect); //窗口大小的改变——判断鼠标位置，改变光标形状
    }
    else {
        //如果不在边角处(==NONE)，则移动，否则改变大小
        if(dir != NONE) {  //在边角处
            QRect newRect(topLeft, bottomRight); //定义一个矩形
            switch(dir) {
//                    case LEFT:
//                        if(bottomRight.x() - globalPoint.x() <= this->minimumWidth()) {
//                            newRect.setLeft(topLeft.x());  //小于界面的最小宽度时，设置为左上角横坐标为窗口x
//                            //只改变左边界
//                        }
//                        else {
//                            newRect.setLeft(globalPoint.x());
//                        }
//                        break;
                case RIGHT:
                    newRect.setWidth(globalPoint.x() - topLeft.x());  //只能改变右边界
                    break;
//                    case UP:
//                        if(bottomRight.y() - globalPoint.y() <= minimumHeight()) {
//                            newRect.setY(topLeft.y());
//                        }
//                        else {
//                            newRect.setY(globalPoint.y());
//                        }
//                        break;
                case DOWN:
                    newRect.setHeight(globalPoint.y() - topLeft.y());
                    break;
//                    case LEFT_TOP:
//                        if(bottomRight.x() - globalPoint.x() <= minimumWidth()) {
//                            newRect.setX(topLeft.x());
//                        }
//                        else {
//                            newRect.setX(globalPoint.x());
//                        }

//                        if(bottomRight.y() - globalPoint.y() <= minimumHeight()) {
//                            newRect.setY(topLeft.y());
//                        }
//                        else {
//                            newRect.setY(globalPoint.y());
//                        }
//                        break;
                 case RIGHT_TOP:
                      if (globalPoint.x() - topLeft.x() >= minimumWidth()) {
                          newRect.setWidth(globalPoint.x() - topLeft.x());
                      }
                      else {
                          newRect.setWidth(bottomRight.x() - topLeft.x());
                      }
                      if (bottomRight.y() - globalPoint.y() >= minimumHeight()) {
                          newRect.setY(globalPoint.y());
                      }
                      else {
                          newRect.setY(topLeft.y());
                      }
                      break;
//                     case LEFT_DOWN:
//                          if (bottomRight.x() - globalPoint.x() >= minimumWidth()) {
//                              newRect.setX(globalPoint.x());
//                          }
//                          else {
//                              newRect.setX(topLeft.x());
//                          }
//                          if (globalPoint.y() - topLeft.y() >= minimumHeight()) {
//                              newRect.setHeight(globalPoint.y() - topLeft.y());
//                          }
//                          else {
//                              newRect.setHeight(bottomRight.y() - topLeft.y());
//                          }
//                          break;
                  case RIGHT_DOWN:
                      newRect.setWidth(globalPoint.x() - topLeft.x());
                      newRect.setHeight(globalPoint.y() - topLeft.y());
                      break;
                  default:
                      break;
            }
            setGeometry(newRect);
        }
        else {
            move(ev->globalPos() - mouseLoc); //移动窗口
            ev->accept();
        }
    }
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::LeftButton) {
        leftBtnPressed = false;
        if (dir != NONE) {
            releaseMouse(); //释放鼠标抓取
            setCursor(QCursor(Qt::ArrowCursor));
            dir = NONE;
        }
    }
}

void FramelessWindow::showEvent(QShowEvent *ev) {
    static QPropertyAnimation anim(this, "windowOpacity");
    anim.setDuration(150);
    anim.setStartValue(0.0);
    anim.setEndValue(1.0);
    anim.start();
    ev->accept();
}
