#ifndef HERO_H
#define HERO_H

#include <QObject>
#include <QMovie>
#include <QtMath>
#include <QDebug>
#include <QPainter>
#include "pet.h"

class heromsg;
class hero : public QObject
{
    Q_OBJECT
public:
    explicit hero(QObject *parent = 0);
    ~hero();

public:
    void init(int id);
    void set_hero(heromsg &heromsg);
    void settargetpos(QPoint newpos);
    void move(void);
    void attack(void);
    void battleend(void);
    void display(QPainter &painter);

private slots:
    void stop_attack(int framenum);

public:
    int m_uid;
    int m_hero_id;
    int m_target;
    char m_name[30];
    QPoint m_pos;
    QRect m_rect;
    QPoint m_targetpos;
    int m_dir;
    int m_state;//0.待机 3.移动 5.攻击 8.受击 10.死亡
    QMovie m_act;
    QMovie m_leader;
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

#endif // HERO_H
