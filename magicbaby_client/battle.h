#ifndef BATTLE_H
#define BATTLE_H

#include <QObject>
#include <QPainter>
#include <QMouseEvent>
#include <QRect>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMessageBox>
#include "battleactor.h"
#include "hero.h"
#include "battledamagemsg.h"

class battle : public QObject
{
    Q_OBJECT
public:
    explicit battle(hero &hero,QObject *parent = 0);
    ~battle();
    void create_battle(int battle_id, int monster_group_id, hero **p_hero);
    void display(QPainter &painters);
    void turn_begin(void);
    void settargetpos(void);
    void move(void);
    void timeevent(void);
    void attack(void);
    void mousepress(QMouseEvent *event);
    void mousemove(QMouseEvent *event);
    void set_damage(int target,int damage);
    void set_heal(int target,int damage);
    void set_res(int target,int damage);
    bool battle_end(void);
    void timerEvent(QTimerEvent *);

public:
    QTcpSocket tcpsocket;
    battledamagemsg m_battledamagemsg;
    int m_battle_id;
    battleactor m_battleactor[20];
    int m_index;
    int m_select;
    int m_skill_index;
    int m_petskill_index;
    int m_skill_select;
    int m_battle_section;
    int m_turn;
    QRect m_command_rect[6];
    hero &m_hero;
    bool m_battleend;
    int m_turntime;
    int m_lefttime;
    int m_secondtime;

private slots:
    void attack(int framenum);
    void cast(int framenum);
    void onerror(QAbstractSocket::SocketError);
    void onconnect(void);
    void onreadyread(void);
};

#endif // BATTLE_H
