#ifndef BATTLEDAMAGEMSG_H
#define BATTLEDAMAGEMSG_H


class battledamagemsg
{
public:
    battledamagemsg();
    ~battledamagemsg();

public:
    //0.指令下达 1.掉线 2.战斗信息
    int m_type;
    int m_pos;
    int m_skill_id[20];
    int m_target[20][10];
    int m_damage[20][10];
};

#endif // BATTLEDAMAGEMSG_H
