#ifndef BATTLE_H
#define BATTLE_H

#include "battleactor.h"
#include "battlemsg.h"
#include "heromsg.h"
#include "battledamagemsg.h"

class battle
{
public:
    battle();
    ~battle();
    bool command(battlemsg &battlemsg);
    void create_battle(int monster_group_id);
    void turn_begin(void);
    bool test_begin(void);
    void calculate(void);
    void settarget(void);
    int set_damage(int target);
    int set_heal(int target);
    int set_res(int target);
    bool battle_end(void);

public:
    battledamagemsg m_battledamagemsg;
    int m_battle_id;
    battleactor m_battleactor[20];
    int m_index;
    bool m_battleend;
};

#endif // BATTLE_H
