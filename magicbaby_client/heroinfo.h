#ifndef HEROINFO_H
#define HEROINFO_H

#include <QString>
class heroinfo
{
public:
    heroinfo();
    ~heroinfo();

public:
    QString m_path;
    QString m_icon_path;
    QString m_name;
    int m_posfix[11][8][3];
    int m_hurt_index;
    int m_hp;
    int m_atk;
    int m_def;
    int m_spd;
    int m_skill_list[6];
    static heroinfo m_heroinfo[999];
};

#endif // HEROINFO_H
