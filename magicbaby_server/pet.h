#ifndef PET_H
#define PET_H


class pet
{
public:
    pet();
    ~pet();

public:
    int m_pet_id;
    int m_hp;
    int m_atk;
    int m_def;
    int m_spd;
    int m_skill_list[3];
};

#endif // PET_H
