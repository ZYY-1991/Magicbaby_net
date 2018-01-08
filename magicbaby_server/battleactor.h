#ifndef BATTLEACTOR_H
#define BATTLEACTOR_H

#include "heromsg.h"
#include "pet.h"

class battleactor
{
public:
    battleactor();
    ~battleactor();
    void sethero(heromsg &heromsg);
    void setmonster(int monster_id);
    void setpet(pet &pet);

public:
    int m_type;//0.角色 1.宠物 2.怪物
    int m_id;
    int m_target;
    int m_skill_id;
    int m_attack_times;
    int m_battlestate;
    int m_actorstate;//0.正常 1.死亡
    bool m_battleready;
    bool m_connect;
    int m_hp;
    int m_max_hp;
    int m_atk;
    int m_def;
    int m_spd;
};

#endif // BATTLEACTOR_H
