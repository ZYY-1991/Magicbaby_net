#include "hero.h"
#include "heroinfo.h"
#include "heromsg.h"
#include <string.h>

hero::hero(QObject *parent) : QObject(parent)
{
    m_leader.setFileName(":/other/leader.gif");
    m_leader.start();
}

hero::~hero()
{

}

void hero::init(int id){
    m_uid = 1;
    m_hero_id = 2;
    strcpy(m_name,"风之若曦");
    m_target = 0;
    m_pos = QPoint(100,100);
    m_targetpos = QPoint(100,100);
    m_state = 0;
    m_dir = 3;
    m_act.setFileName(heroinfo::m_heroinfo[m_hero_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
    m_act.start();
    m_atk = heroinfo::m_heroinfo[m_hero_id].m_atk;
    m_def = heroinfo::m_heroinfo[m_hero_id].m_def;
    m_hp = heroinfo::m_heroinfo[m_hero_id].m_hp;
    m_spd = heroinfo::m_heroinfo[m_hero_id].m_spd;
    for(int i=0;i<6;i++){
        m_skill_list[i] = heroinfo::m_heroinfo[m_hero_id].m_skill_list[i];
    }
    m_pet.init(101);
    m_team_state = 0;
    for(int i=0;i<4;i++){
        m_team_member[i] = 0;
    }
    m_team_pos = 0;
}

void hero::set_hero(heromsg &heromsg){
    m_uid = heromsg.m_uid;
    m_hero_id = heromsg.m_hero_id;
    //m_target = heromsg.m_target;
    strcpy(m_name,heromsg.m_name);
    m_pos = heromsg.m_pos;
    m_targetpos = heromsg.m_targetpos;
    m_state = 0;
    m_dir = heromsg.m_dir;
    m_atk = heromsg.m_atk;
    m_def = heromsg.m_def;
    m_hp = heromsg.m_hp;
    m_spd = heromsg.m_spd;
    for(int i=0;i<6;i++){
        m_skill_list[i] = heromsg.m_skill_list[i];
    }
    m_pet = heromsg.m_pet;
    m_team_state = heromsg.m_team_state;
    for(int i=0;i<4;i++){
        m_team_member[i] = heromsg.m_team_member[i];
    }
    m_team_pos = heromsg.m_team_pos;
}

void hero::display(QPainter &painter){
    QPoint posfix = QPoint(heroinfo::m_heroinfo[m_hero_id].m_posfix[m_state][m_dir][0],heroinfo::m_heroinfo[m_hero_id].m_posfix[m_state][m_dir][1]);
    painter.drawImage(QRect(m_pos - posfix,m_act.currentImage().size()),m_act.currentImage());
    painter.setPen(QPen(Qt::red,5));
    painter.setFont(QFont("文泉驿等宽正黑", 11, QFont::Normal));
    painter.drawText(QRect(m_pos - QPoint(100,90),QSize(200,20)),Qt::AlignCenter,m_name);
    if(m_team_state == 1){
        painter.drawImage(QRect(m_pos - QPoint(10,90),m_leader.currentImage().size()),m_leader.currentImage());
    }
}

void hero::settargetpos(QPoint newpos){
    //QObject::disconnect(&m_act,SIGNAL(frameChanged(int)),this,SLOT(stop_attack(int)));
    if(newpos.x() < 50){
        newpos.setX(50);
    }
    if(newpos.x() > 1150){
        newpos.setX(1150);
    }
    if(newpos.y() < 100){
        newpos.setY(100);
    }
    if(newpos.y() > 750){
        newpos.setY(750);
    }
    m_targetpos = newpos;
    if(m_targetpos != m_pos){
        int old_dir = m_dir;
        QPoint movepos = m_targetpos - m_pos;
        int x = movepos.x();
        int y = movepos.y();
        if(x <= 0 && y <= 0 && y <= x/2 && y >= 2*x){
            m_dir = 0;
        }else if(y <= 0 && y <= -2*x && y <= 2*x){
            m_dir = 1;
        }else if(x >= 0 && y <= 0 && y <= -x/2 && y >= -2*x){
            m_dir = 2;
        }else if(x >= 0 && y <= x/2 && y >= -x/2){
            m_dir = 3;
        }else if(x >= 0 && y >= 0 && y <= 2*x && y >= x/2){
            m_dir = 4;
        }else if(y >= 0 && y >= -2*x && y >= 2*x){
            m_dir = 5;
        }else if(x <= 0 && y >= 0 && y <= -2*x && y >= -x/2){
            m_dir = 6;
        }else if(x <= 0 && y <= -x/2 && y >= x/2){
            m_dir = 7;
        }
        if(m_state != 3 || old_dir != m_dir){
            m_state = 3;
            m_act.stop();
            m_act.setFileName(heroinfo::m_heroinfo[m_hero_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
            m_act.start();
        }
    }
}

void hero::move(){
    QPoint movepos = m_targetpos - m_pos;
    int x = movepos.x();
    int y = movepos.y();
    double dis = qSqrt(x*x + y*y);

    if(dis <= 5){
        m_pos = m_targetpos;
        m_state = 0;
        m_act.stop();
        m_act.setFileName(heroinfo::m_heroinfo[m_hero_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
        m_act.start();
    }else{
        m_pos.setX(m_pos.x() + movepos.x()/dis*5);
        m_pos.setY(m_pos.y() + movepos.y()/dis*5);
    }
}

void hero::attack(void){
    m_targetpos = m_pos;
    m_state = 5;
    m_act.stop();
    m_act.setFileName(heroinfo::m_heroinfo[m_hero_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
    m_act.start();
    QObject::connect(&m_act,SIGNAL(frameChanged(int)),this,SLOT(stop_attack(int)));
}

void hero::stop_attack(int framenum){
    if(framenum == m_act.frameCount()-1){
        QObject::disconnect(&m_act,SIGNAL(frameChanged(int)),this,SLOT(stop_attack(int)));
        m_state = 0;
        m_act.stop();
        m_act.setFileName(heroinfo::m_heroinfo[m_hero_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
        m_act.start();
    }
}

void hero::battleend(void){
    m_state = 0;
    m_act.stop();
    m_act.setFileName(heroinfo::m_heroinfo[m_hero_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
    m_act.start();
}
