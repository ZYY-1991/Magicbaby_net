#ifndef MONSTER_H
#define MONSTER_H

#include <QObject>
#include <QMovie>
#include <QRect>
#include "heromsg.h"

class monster : public QObject
{
    Q_OBJECT
public:
    explicit monster(QObject *parent = 0);
    ~monster();
    void init(heromsg &heromsg);

public:
    int m_monstergroup_id;
    int m_monster_id;
    int m_dir;
    int m_state;
    QMovie m_act;
    QPoint m_pos;
    QRect m_rect;

public slots:
};

#endif // MONSTER_H
