#include "heromsg.h"
#include <string.h>

heromsg::heromsg()
{

}

heromsg::heromsg(hero &hero){
    m_uid = hero.m_uid;
    m_hero_id = hero.m_hero_id;
    m_target = hero.m_target;
    strcpy(m_name,hero.m_name);
    m_pos = hero.m_pos;
    m_targetpos = hero.m_targetpos;
    m_dir = hero.m_dir;
    m_hp = hero.m_hp;
    m_atk = hero.m_atk;
    m_def = hero.m_def;
    m_spd = hero.m_spd;
    for(int i=0;i<6;i++){
        m_skill_list[i] = hero.m_skill_list[i];
    }
    m_pet = hero.m_pet;
    m_team_state = hero.m_team_state;//0.未组队 1.队长 2.队员
    for(int i=0;i<4;i++){
        m_team_member[i] = hero.m_team_member[i];
    }
    m_team_pos = hero.m_team_pos;
}

heromsg::~heromsg()
{

}

