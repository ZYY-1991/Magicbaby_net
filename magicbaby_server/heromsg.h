#ifndef HEROMSG_H
#define HEROMSG_H

#include <QPoint>
#include <QtMath>
#include <QTime>
#include "pet.h"

class heromsg
{
public:
    heromsg();
    ~heromsg();
    void move(void);

public:
    //服务器->客户端：0.新角色上线 1.老角色状态改变 2.检测uid 3.角色离线 4.怪物信息 5.创建战斗
    //                6.队伍邀请
    //客户端->服务器：0.上线 1.老角色状态改变 2.进入战斗 3.队伍邀请
    int m_type;
    int m_uid;
    int m_hero_id;
    int m_target;
    char m_name[30];
    int m_monstergroup_id;
    int m_battle_id;
    QPoint m_pos;
    QPoint m_targetpos;
    int m_dir;
    int m_hp;
    int m_atk;
    int m_def;
    int m_spd;
    int m_skill_list[6];
    pet m_pet;
    int m_team_state;//0.未组队 1.队长 2.队员
    int m_team_member[4];
    int m_team_pos;
};

#endif // HEROMSG_H
