#include "confirmbox.h"

confirmbox::confirmbox(QObject *parent) : QObject(parent)
{

}

confirmbox::~confirmbox()
{

}

void confirmbox::display(QPainter &painter){
    painter.setPen(QPen(Qt::red,5));
    painter.setFont(QFont("文泉驿等宽正黑", 20, QFont::Normal));
    painter.drawImage(QRect(400,250,400,300),QImage(":/images/back.png"));
    painter.drawText(QRect(400,300,400,50),Qt::AlignCenter,m_msg);
}

void confirmbox::cancel(void){
    m_confirm.hide();
    m_cancel.hide();
    m_show = false;
}
