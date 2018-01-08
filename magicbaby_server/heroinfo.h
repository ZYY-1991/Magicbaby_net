#ifndef HEROINFO_H
#define HEROINFO_H

#include <QString>
class heroinfo
{
public:
    heroinfo();
    ~heroinfo();

public:
    int m_hp;
    int m_atk;
    int m_def;
    int m_spd;
    static heroinfo m_heroinfo[999];
};

#endif // HEROINFO_H
