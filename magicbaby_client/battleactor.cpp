#include "battleactor.h"
#include "heroinfo.h"

battleactor::battleactor(QObject *parent) : QObject(parent)
{

}

battleactor::~battleactor()
{

}

void battleactor::sethero(hero &hero){
    m_type = 0;
    m_id = hero.m_hero_id;
    m_name = hero.m_name;
    m_state = 0;
    m_dir = 0;
    m_act.stop();
    m_act.setFileName(heroinfo::m_heroinfo[m_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
    m_act.start();
    m_atk = hero.m_atk;
    m_def = hero.m_def;
    m_spd = hero.m_spd;
    m_max_hp = hero.m_hp;
    m_hp = m_max_hp;
}

void battleactor::setmonster(int monster_id){
    m_type = 2;
    m_id = monster_id;
    m_name = heroinfo::m_heroinfo[m_id].m_name;
    m_state = 0;
    m_dir = 4;
    m_act.stop();
    m_act.setFileName(heroinfo::m_heroinfo[m_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
    m_act.start();
    m_atk = heroinfo::m_heroinfo[m_id].m_atk;
    m_def = heroinfo::m_heroinfo[m_id].m_def;
    m_spd = heroinfo::m_heroinfo[m_id].m_spd;
    m_max_hp = heroinfo::m_heroinfo[m_id].m_hp;
    m_hp = m_max_hp;
}

void battleactor::setpet(pet &pet){
    m_type = 1;
    m_id = pet.m_pet_id;
    m_name = heroinfo::m_heroinfo[m_id].m_name;
    m_state = 0;
    m_dir = 0;
    m_act.stop();
    m_act.setFileName(heroinfo::m_heroinfo[m_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
    m_act.start();
    m_atk = pet.m_atk;
    m_def = pet.m_def;
    m_spd = pet.m_spd;
    m_max_hp = pet.m_hp;
    m_hp = m_max_hp;
}

void battleactor::be_attack(int framenum){
    if(framenum == 0){
        QObject::disconnect(&m_act,SIGNAL(frameChanged(int)),this,SLOT(be_attack(int)));
        m_be_attack = false;
        if(m_actorstate != 1){
            m_state = 0;
            m_act.stop();
            m_act.setFileName(heroinfo::m_heroinfo[m_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
            m_act.start();
        }else{
            m_state = 10;
            m_act.stop();
            m_act.setFileName(heroinfo::m_heroinfo[m_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
            m_act.start();
            QObject::connect(&m_act,SIGNAL(frameChanged(int)),this,SLOT(dead(int)));
        }
    }
}

void battleactor::skill(int framenum){
    if(framenum == 0){
        QObject::disconnect(&m_skill,SIGNAL(frameChanged(int)),this,SLOT(skill(int)));
        m_skill.stop();

    }
}

void battleactor::be_heal(int framenum){
    if(framenum == 0){
        QObject::disconnect(&m_skill,SIGNAL(frameChanged(int)),this,SLOT(be_heal(int)));
        m_skill.stop();
        m_be_attack = false;

    }
}

void battleactor::dead(int framenum){
    if(framenum == m_act.frameCount() - 1){
        QObject::disconnect(&m_act,SIGNAL(frameChanged(int)),this,SLOT(dead(int)));
        m_act.setPaused(true);
    }
}

