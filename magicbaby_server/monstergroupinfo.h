#ifndef MONSTERGROUPINFO_H
#define MONSTERGROUPINFO_H


class monstergroupinfo
{
public:
    monstergroupinfo();
    ~monstergroupinfo();

public:
    int m_monster_id[10];
    static monstergroupinfo m_monstergroupinfo[999];
};

#endif // MONSTERGROUPINFO_H
