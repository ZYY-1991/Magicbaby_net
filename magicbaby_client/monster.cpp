#include "monster.h"
#include "heroinfo.h"

monster::monster(QObject *parent) : QObject(parent)
{
    m_monster_id = 0;
}

monster::~monster()
{

}

void monster::init(heromsg &heromsg){
    m_monster_id = heromsg.m_hero_id;
    m_monstergroup_id = heromsg.m_uid;
    m_pos = heromsg.m_pos;
    m_dir = heromsg.m_dir;
    m_state = 0;
    m_act.stop();
    m_act.setFileName(heroinfo::m_heroinfo[m_monster_id].m_path+QString::number(m_dir)+"_"+QString::number(m_state)+".gif");
    m_act.start();
    QPoint posfix = QPoint(heroinfo::m_heroinfo[m_monster_id].m_posfix[m_state][m_dir][0],heroinfo::m_heroinfo[m_monster_id].m_posfix[m_state][m_dir][1]);
    m_rect = QRect(m_pos - posfix,m_act.currentImage().size());
}

