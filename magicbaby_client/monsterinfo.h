#ifndef MONSTERINFO_H
#define MONSTERINFO_H

#include <QString>

class monsterinfo
{
public:
    monsterinfo();
    ~monsterinfo();

public:
    QString m_path;
    int m_posfix[11][8][3];
    int m_hurt_index;
    static monsterinfo m_monsterinfo[10];
    int m_hp;
    int m_atk;
    int m_def;
    int m_spd;
};

#endif // MONSTERINFO_H
