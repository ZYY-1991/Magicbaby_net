#include "tips.h"

tips::tips(QObject *parent) : QObject(parent)
{

}

tips::~tips()
{

}

void tips::display(QPainter &painter){
    painter.setPen(QPen(Qt::red,5));
    painter.setFont(QFont("文泉驿等宽正黑", 20, QFont::Normal));
    painter.drawText(QRect(0,375,1200,50),Qt::AlignCenter,m_tips);
}

void tips::send_tips(QString str){
    m_time = startTimer(1000);
    m_tips = str;
    m_show = true;
}

void tips::timerEvent(QTimerEvent *event){
    if(event->timerId() == m_time){
        m_show = false;
    }
}
