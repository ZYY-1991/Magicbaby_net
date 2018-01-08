#ifndef SKILLINFO_H
#define SKILLINFO_H

#include <QString>
#include <QPoint>

class skillinfo
{
public:
    skillinfo();
    ~skillinfo();

public:
    QString m_skill_name;
    int m_skill_type;//0.普通攻击 1.近战 2.远程
    int m_target_type;//0.敌方单体 1.敌方十字 2.敌方全体 3.友方单体 4.友方十字 5.友方群体 6.友方死亡目标
    int m_attack_times;
    double m_damage_value1;
    int m_damage_value2;
    QString m_be_attack_eff;
    QPoint m_be_attack_posfix;
    QString m_attack_eff;
    QPoint m_attack_posfix;
    static skillinfo m_skillinfo[999];
};

#endif // SKILLINFO_H
