#include "battle.h"
#include "monstergroupinfo.h"
#include "skillinfo.h"

battle::battle()
{

}

battle::~battle()
{

}

void battle::create_battle(int monster_group_id){
    for(int i=0;i<20;i++){
        m_battleactor[i].m_type = -1;
        m_battleactor[i].m_actorstate = 0;
        m_battleactor[i].m_connect = true;
    }
    for(int i=10;i<20;i++){
        if(monstergroupinfo::m_monstergroupinfo[monster_group_id].m_monster_id[i-10] != 0){
            m_battleactor[i].setmonster(monstergroupinfo::m_monstergroupinfo[monster_group_id].m_monster_id[i-10]);
        }
    }
}

void battle::turn_begin(void){
    for(int i=0;i<20;i++){
        m_battleactor[i].m_skill_id = 0;
        m_battleactor[i].m_battlestate = 0;
        m_battleactor[i].m_battleready = false;
        m_battleactor[i].m_target = -1;
    }
    m_battleactor[10].m_skill_id = 5;
    m_battleactor[11].m_skill_id = 4;
    m_battleactor[12].m_skill_id = 4;
    m_battleactor[13].m_skill_id = 3;
    m_battleactor[14].m_skill_id = 3;
    m_battleactor[15].m_skill_id = 8;
    m_battleactor[16].m_skill_id = 0;
    m_battleactor[17].m_skill_id = 0;
    m_battleactor[18].m_skill_id = 2;
    m_battleactor[19].m_skill_id = 2;
}

bool battle::command(battlemsg &battlemsg){
    int pos=battlemsg.m_battle_pos;
    m_battleactor[pos].m_skill_id = battlemsg.m_hero_skill_id;
    m_battleactor[pos].m_target = battlemsg.m_hero_target;
    m_battleactor[pos+5].m_skill_id = battlemsg.m_pet_skill_id;
    m_battleactor[pos+5].m_target = battlemsg.m_pet_target;
    m_battleactor[pos].m_battleready = true;
    return test_begin();
}

bool battle::test_begin(void){
    for(int i=0;i<5;i++){
        if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_battleready == false && m_battleactor[i].m_connect == true){
            return false;
        }
    }
    calculate();
    turn_begin();
    return true;
}

void battle::calculate(void){
    while(1){
        int spd_max = -1;
        m_index = -1;
        for(int i=0;i<20;i++){
            if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1 && m_battleactor[i].m_battlestate == 0 && m_battleactor[i].m_spd > spd_max){
                spd_max = m_battleactor[i].m_spd;
                m_index = i;
            }
        }
        if(m_index == -1){
            return;
        }
        m_battleactor[m_index].m_attack_times = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_attack_times;
        int target = m_battleactor[m_index].m_target;
        int skill_type = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_skill_type;
        int skill_target_type = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_target_type;
        int target_list[20][4] = {{0,1,2,5},{1,0,3,6},{2,0,4,7},{3,1,8,-1},{4,2,9,-1},
                                  {5,6,7,0},{6,5,8,1},{7,5,9,2},{8,6,3,-1},{9,7,4,-1},
                                  {10,11,12,15},{11,10,13,16},{12,10,14,17},{13,11,18,-1},{14,12,19,-1},
                                  {15,16,17,10},{16,15,18,11},{17,15,19,12},{18,16,13,-1},{19,17,14,-1}};
        if(target == -1 || ((skill_target_type == 0 || skill_target_type == 3) && m_battleactor[target].m_actorstate == 1)){
            settarget();
        }else if(skill_target_type == 1 || skill_target_type == 4){
            int i;
            for(i=0;i<4;i++){
                int target = target_list[m_battleactor[m_index].m_target][i];
                if(m_battleactor[target].m_type != -1 && m_battleactor[target].m_actorstate != 1){
                    break;
                }
            }
            if(i == 4){
                settarget();
            }
        }
        m_battledamagemsg.m_skill_id[m_index] = m_battleactor[m_index].m_skill_id;
        if(skill_type == 0 || skill_type == 1){
            int i=0;
            while(m_battleactor[m_index].m_attack_times > 0){
                if(i > 0){
                    settarget();
                }
                m_battleactor[m_index].m_attack_times--;
                m_battledamagemsg.m_target[m_index][i] = m_battleactor[m_index].m_target;
                m_battledamagemsg.m_damage[m_index][i] = set_damage(m_battleactor[m_index].m_target);
                if(battle_end() == true){
                    return;
                }
                i++;
            }
        }else{
            target = m_battleactor[m_index].m_target;
            int target_type = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_target_type;
            m_battledamagemsg.m_target[m_index][0] = m_battleactor[m_index].m_target;
            if(target_type == 0 || target_type == 3){//单体
                if(target_type == 0){
                    m_battledamagemsg.m_damage[m_index][0] = set_damage(target);
                }else{
                    m_battledamagemsg.m_damage[m_index][0] = set_heal(target);
                }
            }else if(target_type == 6){//复活
                m_battledamagemsg.m_damage[m_index][0] = set_res(target);
            }else if(target_type == 1 || target_type == 4){//十字
                for(int i=0;i<4;i++){
                    int t_target = target_list[target][i];
                    if(t_target != -1 && m_battleactor[t_target].m_type != -1 && m_battleactor[t_target].m_actorstate != 1){
                        if(target_type == 1){
                            m_battledamagemsg.m_damage[m_index][i] = set_damage(t_target);
                        }else{
                            m_battledamagemsg.m_damage[m_index][i] = set_heal(t_target);
                        }
                    }
                }
            }else if((target_type == 2 && m_battleactor[m_index].m_type == 2) || (target_type == 5 && m_battleactor[m_index].m_type < 2)){//目标为我方
                for(int i=0;i<10;i++){
                    if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1){
                        if(target_type == 2){
                            m_battledamagemsg.m_damage[m_index][i] = set_damage(i);
                        }else{
                            m_battledamagemsg.m_damage[m_index][i] = set_heal(i);
                        }
                    }
                }
            }else{//目标为敌方
                for(int i=10;i<20;i++){
                    if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1){
                        if(target_type == 2){
                            m_battledamagemsg.m_damage[m_index][i-10] = set_damage(i);
                        }else{
                            m_battledamagemsg.m_damage[m_index][i-10] = set_heal(i);
                        }
                    }
                }
            }
            if(battle_end() == true){
                return;
            }
        }
        m_battleactor[m_index].m_battlestate = 5;
    }
}

void battle::settarget(void){
    do{
        int skill_target_type = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_target_type;
        if((m_battleactor[m_index].m_type == 2 && skill_target_type <= 2) || (m_battleactor[m_index].m_type < 2 && skill_target_type > 2)){
            m_battleactor[m_index].m_target = rand() % 10;
        }else{
            m_battleactor[m_index].m_target = rand() % 10 + 10;
        }
    }while(m_battleactor[m_battleactor[m_index].m_target].m_type == -1 || m_battleactor[m_battleactor[m_index].m_target].m_actorstate == 1);
}

//设置伤害
int battle::set_damage(int target){
    double damage_value1 = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_damage_value1;
    int damage_value2 = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_damage_value2;
    int damage = ((m_battleactor[m_index].m_atk - m_battleactor[target].m_def)*damage_value1+damage_value2)*(9000 + qrand()%2000)/10000;
    if(damage < 10){
        damage = qrand()%9+1;
    }
    m_battleactor[target].m_hp -= damage;
    if(m_battleactor[target].m_hp <= 0){
        m_battleactor[target].m_hp = 0;
        m_battleactor[target].m_actorstate = 1;
    }
    return damage;
}

//设置治疗
int battle::set_heal(int target){
    double damage_value1 = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_damage_value1;
    int damage_value2 = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_damage_value2;
    int damage = ((m_battleactor[m_index].m_atk)*damage_value1+damage_value2)*(9000 + qrand()%2000)/10000;
    if(damage < 10){
        damage = qrand()%9+1;
    }
    m_battleactor[target].m_hp += damage;
    if(m_battleactor[target].m_hp > m_battleactor[target].m_max_hp){
        m_battleactor[target].m_hp = m_battleactor[target].m_max_hp;
    }
    return damage;
}

//设置复活
int battle::set_res(int target){
    if(m_battleactor[target].m_actorstate == 0){
        return 0;
    }
    double damage_value1 = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_damage_value1;
    int damage_value2 = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_damage_value2;
    int damage = ((m_battleactor[m_index].m_atk)*damage_value1+damage_value2)*(9000 + qrand()%2000)/10000;
    if(damage < 10){
        damage = qrand()%9+1;
    }
    m_battleactor[target].m_hp += damage;
    if(m_battleactor[target].m_hp > m_battleactor[target].m_max_hp){
        m_battleactor[target].m_hp = m_battleactor[target].m_max_hp;
    }
    m_battleactor[target].m_actorstate = 0;
    return damage;
}

bool battle::battle_end(void){
    bool win = true;
    for(int i=10;i<20;i++){
        if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1){
            win = false;
            break;
        }
    }
    bool lose = true;
    for(int i=0;i<10;i++){
        if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1){
            lose = false;
            break;
        }
    }
    if(win || lose){
        return true;
    }else{
        return false;
    }
}
