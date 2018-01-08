#ifndef BATTLEACTOR_H
#define BATTLEACTOR_H

#include <QObject>
#include <QMovie>
#include <QRect>
#include "hero.h"
#include "pet.h"

class battleactor : public QObject
{
    Q_OBJECT
public:
    explicit battleactor(QObject *parent = 0);
    ~battleactor();
    void sethero(hero &hero);
    void setmonster(int monster_id);
    void setpet(pet &pet);

public:
    void settargetpos(QPoint newpos);
    void move(void);
    void attack(void);

public:
    int m_type;//0.角色 1.宠物 2.怪物
    int m_id;
    QString m_name;
    bool m_connect;
    int m_target;
    int m_skill_id;
    int m_attack_times;
    int m_attack_times_index;
    QPoint m_pos;
    QPoint m_targetpos;
    QPoint m_standpos;
    int m_dir;
    int m_state;//0.待机 3.移动 5.攻击 8.受击 10.死亡

    //普攻 敌单体 单次
    //近战技能 敌单体 多次
    //远程技能 敌单体十字群体 友方单体十字群体 单次

    //普通攻击 待机-移动-攻击-移动回-回合结束
    //近战 待机-施法(特效)-（移动-攻击-等待(等待动作播完)）*攻击次数-移动回-回合结束
    //远程 待机-施法(动作特效)-等待施法(等待特效播完)-回合结束

    int m_battlestate;//0.待机 1.移动 2.攻击 3.移动回 4.施法 5.回合结束 6.等待施法 7.等待
    int m_actorstate;//0.正常 1.死亡
    bool m_be_attack;
    bool m_battleready;
    QMovie m_act;
    int m_hp;
    int m_max_hp;
    int m_atk;
    int m_def;
    int m_spd;
    QRect m_rect;
    QMovie m_skill;
    QPoint m_skill_posfix;
    QImage m_damagenum;


public slots:
    void be_attack(int framenum);
    void be_heal(int framenum);
    void skill(int framenum);
    void dead(int framenum);
};

#endif // BATTLEACTOR_H
