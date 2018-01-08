#ifndef BATTLEMSG_H
#define BATTLEMSG_H


class battlemsg
{
public:
    battlemsg();
    ~battlemsg();

public:
    int m_type;
    int m_uid;
    int m_battle_id;
    int m_battle_pos;
    int m_hero_skill_id;
    int m_hero_target;
    int m_pet_skill_id;
    int m_pet_target;
};

#endif // BATTLEMSG_H
