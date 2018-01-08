#include "pet.h"
#include "heroinfo.h"

pet::pet()
{

}

pet::~pet()
{

}

void pet::init(int pet_id){
    m_pet_id = pet_id;
    m_atk = heroinfo::m_heroinfo[m_pet_id].m_atk;
    m_def = heroinfo::m_heroinfo[m_pet_id].m_def;
    m_hp = heroinfo::m_heroinfo[m_pet_id].m_hp;
    m_spd = heroinfo::m_heroinfo[m_pet_id].m_spd;
    for(int i=0;i<3;i++){
        m_skill_list[i] = heroinfo::m_heroinfo[m_pet_id].m_skill_list[i];
    }
}

