#include "heromsg.h"

heromsg::heromsg()
{
    m_uid = -2;
}

heromsg::~heromsg()
{

}

void heromsg::move(){
    QPoint movepos = m_targetpos - m_pos;
    int x = movepos.x();
    int y = movepos.y();
    double dis = qSqrt(x*x + y*y);

    if(dis <= 5){
        m_pos = m_targetpos;
    }else{
        m_pos.setX(m_pos.x() + movepos.x()/dis*5);
        m_pos.setY(m_pos.y() + movepos.y()/dis*5);
    }
}

