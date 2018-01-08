#include "battleactor.h"
#include "heroinfo.h"

battleactor::battleactor()
{

}

battleactor::~battleactor()
{

}

void battleactor::sethero(heromsg &heromsg){
    m_type = 0;
    m_id = heromsg.m_hero_id;
    m_atk = heromsg.m_atk;
    m_def = heromsg.m_def;
    m_spd = heromsg.m_spd;
    m_max_hp = heromsg.m_hp;
    m_hp = m_max_hp;
}

void battleactor::setmonster(int monster_id){
    m_type = 2;
    m_id = monster_id;
    m_atk = heroinfo::m_heroinfo[m_id].m_atk;
    m_def = heroinfo::m_heroinfo[m_id].m_def;
    m_spd = heroinfo::m_heroinfo[m_id].m_spd;
    m_max_hp = heroinfo::m_heroinfo[m_id].m_hp;
    m_hp = m_max_hp;
}

void battleactor::setpet(pet &pet){
    m_type = 1;
    m_id = pet.m_pet_id;
    m_atk = pet.m_atk;
    m_def = pet.m_def;
    m_spd = pet.m_spd;
    m_max_hp = pet.m_hp;
    m_hp = m_max_hp;
}

