#include "battle.h"
#include "heroinfo.h"
#include "skillinfo.h"
#include "monstergroupinfo.h"
#include "battlemsg.h"

battle::battle(hero &hero,QObject *parent) : QObject(parent), m_hero(hero){

}

battle::~battle()
{

}
//鼠标按下
void battle::mousepress(QMouseEvent *event){
    if(m_battle_section == 0 && m_select!=-1){//角色目标选择
        m_battleactor[m_hero.m_team_pos].m_target = m_select;
        m_battleactor[m_hero.m_team_pos].m_skill_id = m_skill_select;
        m_battle_section = 1;
        m_skill_select = m_hero.m_pet.m_skill_list[m_petskill_index];
        mousemove(event);
    }else if(m_battle_section == 1 && m_select!=-1){//宠物目标选择
        m_battleactor[m_hero.m_team_pos+5].m_target = m_select;
        m_battleactor[m_hero.m_team_pos+5].m_skill_id = m_skill_select;
        m_battle_section = 3;
        m_skill_select = m_hero.m_skill_list[m_skill_index];
        battlemsg tmp;
        tmp.m_type = 1;
        tmp.m_battle_id = m_battle_id;
        tmp.m_uid = m_hero.m_uid;
        tmp.m_battle_pos = m_hero.m_team_pos;
        tmp.m_hero_skill_id = m_battleactor[m_hero.m_team_pos].m_skill_id;
        tmp.m_hero_target = m_battleactor[m_hero.m_team_pos].m_target;
        tmp.m_pet_skill_id = m_battleactor[m_hero.m_team_pos+5].m_skill_id;
        tmp.m_pet_target = m_battleactor[m_hero.m_team_pos+5].m_target;
        tcpsocket.write((char *)&tmp, sizeof(battlemsg));
        killTimer(m_turntime);
        killTimer(m_secondtime);
    }else if(m_battle_section == 0){//角色技能选择
        for(int i=0;i<6;i++){
            if(m_command_rect[i].contains(event->pos()) == true){
                m_skill_index = i;
                m_skill_select = m_hero.m_skill_list[i];
                break;
            }
        }
    }else if(m_battle_section == 1){//宠物技能选择
        for(int i=0;i<3;i++){
            if(m_command_rect[i].contains(event->pos()) == true){
                m_petskill_index = i;
                m_skill_select = m_hero.m_pet.m_skill_list[i];
                break;
            }
        }
    }
}

//鼠标移动
void battle::mousemove(QMouseEvent *event){
    if(m_battle_section != 2){
        m_select = -1;
        int target_type = skillinfo::m_skillinfo[m_skill_select].m_target_type;
        if(target_type <= 2){
            for(int i=10;i<20;i++){
                if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_rect.contains(event->pos()) == true){
                    m_select = i;
                    break;
                }
            }
        }else{
            for(int i=0;i<10;i++){
                if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_rect.contains(event->pos()) == true){
                    m_select = i;
                    break;
                }
            }
        }
    }
}

//创建战斗
void battle::create_battle(int battle_id, int monster_group_id,hero **p_hero)
{
    connect(&tcpsocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onerror(QAbstractSocket::SocketError)));
    connect(&tcpsocket,SIGNAL(connected()),this,SLOT(onconnect()));
    connect(&tcpsocket,SIGNAL(readyRead()),this,SLOT(onreadyread()));

    short port;
    QString IP;
    QString str;
    QFile file("config.info");
    if(file.exists() == false){
        QMessageBox::critical(NULL,"ERROR","配置文件错误");
        exit(0);
    }
    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    while(in.atEnd() == false){
        in >> str;
        if(str == "ip"){
            in >> IP;
        }else if(str == "battle_port"){
            in >> port;
        }
    }
    file.close();
    QHostAddress ip(IP);
    tcpsocket.connectToHost(ip,port);

    m_battle_id = battle_id;
    for(int i=0;i<20;i++){
        m_battleactor[i].m_type = -1;
        m_battleactor[i].m_actorstate = 0;
        m_battleactor[i].m_battlestate = 0;
        m_battleactor[i].m_be_attack = false;
        m_battleactor[i].m_connect = true;
    }
    m_command_rect[0] = QRect(900,41,64,20);
    m_command_rect[1] = QRect(990,41,64,20);
    m_command_rect[2] = QRect(1080,41,64,20);
    m_command_rect[3] = QRect(900,67,64,20);
    m_command_rect[4] = QRect(990,67,64,20);
    m_command_rect[5] = QRect(1080,67,64,20);
    int pos[20][2] = {{980,625},{890,680},{1070,570},{800,735},{1160,515},
                      {920,560},{830,615},{1010,505},{740,670},{1100,450},
                      {270,200},{180,255},{360,145},{90,310},{450,90},
                      {330,265},{240,320},{420,210},{150,375},{510,155}};
    for(int i=0;i<5;i++){
        if(p_hero[i] != NULL){
            m_battleactor[i].sethero(*p_hero[i]);
            m_battleactor[i+5].setpet(p_hero[i]->m_pet);
        }
    }
    for(int i=10;i<20;i++){
        if(monstergroupinfo::m_monstergroupinfo[monster_group_id].m_monster_id[i-10] != 0){
            m_battleactor[i].setmonster(monstergroupinfo::m_monstergroupinfo[monster_group_id].m_monster_id[i-10]);
        }
    }
    for(int i=0;i<20;i++){
        if(m_battleactor[i].m_type != -1){
            m_battleactor[i].m_pos = QPoint(pos[i][0],pos[i][1]);
            m_battleactor[i].m_targetpos = QPoint(pos[i][0],pos[i][1]);
            m_battleactor[i].m_standpos = QPoint(pos[i][0],pos[i][1]);
            QPoint posfix = QPoint(heroinfo::m_heroinfo[m_battleactor[i].m_id].m_posfix[m_battleactor[i].m_state][m_battleactor[i].m_dir][0],
                                   heroinfo::m_heroinfo[m_battleactor[i].m_id].m_posfix[m_battleactor[i].m_state][m_battleactor[i].m_dir][1]);
            m_battleactor[i].m_rect = QRect(m_battleactor[i].m_pos - posfix,m_battleactor[i].m_act.currentImage().size());
        }
    }
    m_turn = 0;
    m_battleend = false;
    m_skill_index = 0;
    m_petskill_index = 0;
    m_skill_select = 0;
    turn_begin();
}

//回合开始
void battle::turn_begin(void){
    for(int i=0;i<20;i++){
        m_battleactor[i].m_attack_times_index = 0;
        m_battleactor[i].m_battlestate = 0;
        m_battleactor[i].m_skill_id = 0;
        m_battleactor[i].m_target = -1;
        m_battleactor[i].m_be_attack = false;
        m_battleactor[i].m_battleready = false;
    }
    //设置下一个出手对象
    int spd_max = -1;
    for(int i=0;i<20;i++){
        if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1 && m_battleactor[i].m_spd > spd_max){
            spd_max = m_battleactor[i].m_spd;
            m_index = i;
        }
    }
    m_battle_section = 0;
    m_select = -1;
    m_turn++;
    m_turntime = startTimer(30000);
    m_secondtime = startTimer(1000);
    m_lefttime = 30;
}

//显示
void battle::display(QPainter &painter){
    //背景
    painter.drawImage(QRect(0,0,1200,800),QImage(":/map/battle.gif"));
    //角色
    for(int i=19;i>=0;i--){
        if(m_battleactor[i].m_type != -1){
            QPoint posfix;
            posfix = QPoint(heroinfo::m_heroinfo[m_battleactor[i].m_id].m_posfix[m_battleactor[i].m_state][m_battleactor[i].m_dir][0],heroinfo::m_heroinfo[m_battleactor[i].m_id].m_posfix[m_battleactor[i].m_state][m_battleactor[i].m_dir][1]);
            painter.drawImage(QRect(m_battleactor[i].m_pos - posfix,m_battleactor[i].m_act.currentImage().size()),m_battleactor[i].m_act.currentImage());
            painter.drawImage(QRect(m_battleactor[i].m_pos.x()-25, m_battleactor[i].m_pos.y()+12,50,10), QImage(":/images/blood_frame.png"));
            int length = m_battleactor[i].m_hp*33/m_battleactor[i].m_max_hp;
            painter.drawImage(QRect(m_battleactor[i].m_pos.x()-13, m_battleactor[i].m_pos.y()+14,length,3), QImage(":/images/blood.png"));
            painter.setPen(QPen(Qt::red,5));
            painter.setFont(QFont("文泉驿等宽正黑", 11, QFont::Normal));
            painter.drawText(QRect(m_battleactor[i].m_pos - QPoint(100,90),QSize(200,20)),Qt::AlignCenter,m_battleactor[i].m_name);
            //技能特效
            if(m_battleactor[i].m_skill.state() == QMovie::Running){
                painter.drawImage(QRect(m_battleactor[i].m_pos - m_battleactor[i].m_skill_posfix, m_battleactor[i].m_skill.currentImage().size()),m_battleactor[i].m_skill.currentImage());
            }
        }
    }
    //状态
    for(int i=0;i<5;i++){
        if(m_battleactor[i].m_type != -1){
            if(m_battleactor[i].m_connect == false){
                painter.drawText(QRect(m_battleactor[i].m_pos - QPoint(100,110),QSize(200,20)),Qt::AlignCenter,QString("离线"));
            }else if(m_battleactor[i].m_battleready == false){
                painter.drawText(QRect(m_battleactor[i].m_pos - QPoint(100,110),QSize(200,20)),Qt::AlignCenter,QString("等待"));
            }
        }
    }
    //伤害数字
    for(int i=19;i>=0;i--){
        if(m_battleactor[i].m_be_attack == true){
            painter.drawImage(QRect(m_battleactor[i].m_pos - QPoint(m_battleactor[i].m_damagenum.width()/2,70), m_battleactor[i].m_damagenum.size()),m_battleactor[i].m_damagenum);
        }
    }
    //选择框
    if(m_select != -1 && m_battle_section < 2){
        painter.setPen(QPen(Qt::green,4));
        int target_type = skillinfo::m_skillinfo[m_skill_select].m_target_type;
        if(target_type == 0 || target_type == 3 || target_type == 6){
            painter.drawRect(m_battleactor[m_select].m_rect);
        }else if(target_type == 1 || target_type == 4){
            int target_list[20][4] = {{0,1,2,5},{1,0,3,6},{2,0,4,7},{3,1,8,-1},{4,2,9,-1},
                                      {5,6,7,0},{6,5,8,1},{7,5,9,2},{8,6,3,-1},{9,7,4,-1},
                                      {10,11,12,15},{11,10,13,16},{12,10,14,17},{13,11,18,-1},{14,12,19,-1},
                                      {15,16,17,10},{16,15,18,11},{17,15,19,12},{18,16,13,-1},{19,17,14,-1}};
            for(int i=0;i<4;i++){
                int target = target_list[m_select][i];
                if(target != -1 && m_battleactor[target].m_type != -1){
                    painter.drawRect(m_battleactor[target].m_rect);
                }
            }
        }else if(target_type == 2){
            for(int i=10;i<20;i++){
                if(m_battleactor[i].m_type != -1){
                    painter.drawRect(m_battleactor[i].m_rect);
                }
            }
        }else if(target_type == 5){
            for(int i=0;i<10;i++){
                if(m_battleactor[i].m_type != -1){
                    painter.drawRect(m_battleactor[i].m_rect);
                }
            }
        }
    }
    //指令框
    if(m_battle_section == 0){
        painter.drawImage(QRect(880,20,300,70), QImage(":/images/hero_command.png"));
        for(int i=0;i<6;i++){
            if(m_skill_index == i){
                painter.drawImage(m_command_rect[i], QImage(":/images/attack_choose.png"));
            }else{
                painter.drawImage(m_command_rect[i], QImage(":/images/attack.png"));
            }
        }
        painter.setPen(QPen(Qt::white,5));
        painter.setFont(QFont("文泉驿等宽正黑", 11, QFont::Normal));
        painter.drawText(QRect(902,41,64,20), skillinfo::m_skillinfo[m_hero.m_skill_list[0]].m_skill_name);
        painter.drawText(QRect(992,41,64,20), skillinfo::m_skillinfo[m_hero.m_skill_list[1]].m_skill_name);
        painter.drawText(QRect(1082,41,64,20), skillinfo::m_skillinfo[m_hero.m_skill_list[2]].m_skill_name);
        painter.drawText(QRect(902,67,64,20), skillinfo::m_skillinfo[m_hero.m_skill_list[3]].m_skill_name);
        painter.drawText(QRect(992,67,64,20), skillinfo::m_skillinfo[m_hero.m_skill_list[4]].m_skill_name);
        painter.drawText(QRect(1082,67,64,20), skillinfo::m_skillinfo[m_hero.m_skill_list[5]].m_skill_name);
    }else if(m_battle_section == 1){
        painter.drawImage(QRect(880,20,300,70), QImage(":/images/pet_command.png"));
        for(int i=0;i<3;i++){
            if(m_petskill_index == i){
                painter.drawImage(m_command_rect[i], QImage(":/images/attack_choose.png"));
            }else{
                painter.drawImage(m_command_rect[i], QImage(":/images/attack.png"));
            }
        }
        painter.setPen(QPen(Qt::white,5));
        painter.setFont(QFont("文泉驿等宽正黑", 11, QFont::Normal));
        painter.drawText(QRect(902,41,64,20), skillinfo::m_skillinfo[m_hero.m_pet.m_skill_list[0]].m_skill_name);
        painter.drawText(QRect(992,41,64,20), skillinfo::m_skillinfo[m_hero.m_pet.m_skill_list[1]].m_skill_name);
        painter.drawText(QRect(1082,41,64,20), skillinfo::m_skillinfo[m_hero.m_pet.m_skill_list[2]].m_skill_name);
    }
    //回合
    painter.setPen(QPen(Qt::white,10));
    QFont ft_turn("文泉驿等宽正黑", 50, QFont::Black);
    ft_turn.setItalic(true);
    painter.setFont(ft_turn);
    painter.drawText(QRect(50,650,400,200),QString().sprintf("第 %02d 回 合", m_turn));
    //倒计时
    if(m_battle_section < 2){
        painter.setPen(QPen(Qt::white,10));
        QFont ft_time("文泉驿等宽正黑", 80, QFont::Black);
        ft_time.setItalic(true);
        painter.setFont(ft_time);
        painter.drawText(QRect(530,150,400,200),QString().sprintf("%02d", m_lefttime));
    }
}

//设置移动位置
void battle::settargetpos(void){
    if(m_battleactor[m_index].m_battlestate == 1){
        int target = m_battleactor[m_index].m_target;
        if(m_battleactor[m_index].m_type < 2){
            m_battleactor[m_index].m_targetpos = m_battleactor[target].m_pos + QPoint(40,40);
        }else if(m_battleactor[m_index].m_type == 2){
            m_battleactor[m_index].m_targetpos = m_battleactor[target].m_pos - QPoint(40,40);
        }
    }else{
        m_battleactor[m_index].m_targetpos = m_battleactor[m_index].m_standpos;
        m_battleactor[m_index].m_dir = 4 - m_battleactor[m_index].m_dir;
    }
    m_battleactor[m_index].m_state = 3;
    m_battleactor[m_index].m_act.stop();
    m_battleactor[m_index].m_act.setFileName(heroinfo::m_heroinfo[m_battleactor[m_index].m_id].m_path+QString::number(m_battleactor[m_index].m_dir)+"_"+QString::number(m_battleactor[m_index].m_state)+".gif");
    m_battleactor[m_index].m_act.start();
}

//移动
void battle::move(void){
    QPoint movepos = m_battleactor[m_index].m_targetpos - m_battleactor[m_index].m_pos;
    int x = movepos.x();
    int y = movepos.y();
    double dis = qSqrt(x*x + y*y);

    if(dis <= 20 && m_battleactor[m_index].m_battlestate == 1){//移动到目标面前时
        m_battleactor[m_index].m_battlestate = 2;
        m_battleactor[m_index].m_pos = m_battleactor[m_index].m_targetpos;
        m_battleactor[m_index].m_state = 5;
        m_battleactor[m_index].m_act.stop();
        m_battleactor[m_index].m_act.setFileName(heroinfo::m_heroinfo[m_battleactor[m_index].m_id].m_path+QString::number(m_battleactor[m_index].m_dir)+"_"+QString::number(m_battleactor[m_index].m_state)+".gif");
        m_battleactor[m_index].m_act.start();
        QObject::connect(&m_battleactor[m_index].m_act,SIGNAL(frameChanged(int)),this,SLOT(attack(int)));
    }else if(dis <= 20 && m_battleactor[m_index].m_battlestate == 3){//移动回原位时
        m_battleactor[m_index].m_dir = 4 - m_battleactor[m_index].m_dir;
        m_battleactor[m_index].m_pos = m_battleactor[m_index].m_targetpos;
        m_battleactor[m_index].m_state = 0;
        m_battleactor[m_index].m_act.stop();
        m_battleactor[m_index].m_act.setFileName(heroinfo::m_heroinfo[m_battleactor[m_index].m_id].m_path+QString::number(m_battleactor[m_index].m_dir)+"_"+QString::number(m_battleactor[m_index].m_state)+".gif");
        m_battleactor[m_index].m_act.start();
        m_battleactor[m_index].m_battlestate = 5;
    }else{//移动
        m_battleactor[m_index].m_pos.setX(m_battleactor[m_index].m_pos.x() + movepos.x()/dis*20);
        m_battleactor[m_index].m_pos.setY(m_battleactor[m_index].m_pos.y() + movepos.y()/dis*20);
    }
}

//刷新
void battle::timeevent(void){
    if(m_battle_section != 2){
        return;
    }
    if(m_index == -1){
        turn_begin();
        return;
    }
    if(m_battleactor[m_index].m_battlestate == 7 || m_battleactor[m_index].m_battlestate == 6){//等待状态
        int skill_target_type = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_target_type;
        if((skill_target_type <= 2 && m_battleactor[m_index].m_type == 2) || (skill_target_type > 2 && m_battleactor[m_index].m_type < 2)){
            for(int i=0;i<10;i++){
                if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_be_attack == true){
                    return;
                }
            }
        }else{
            for(int i=10;i<20;i++){
                if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_be_attack == true){
                    return;
                }
            }
        }
        if((m_battleend = battle_end()) == true){
            battlemsg tmp;
            if(m_hero.m_team_state == 0 || m_hero.m_team_state == 1){
                tmp.m_type = 2;
                tmp.m_battle_id = m_battle_id;
                tcpsocket.write((char *)&tmp, sizeof(battlemsg));
            }
            tcpsocket.disconnectFromHost();
            return;
        }
        if(m_battleactor[m_index].m_battlestate == 7){
            m_battleactor[m_index].m_attack_times_index++;
            m_battleactor[m_index].m_battlestate = 1;
            m_battleactor[m_index].m_target = m_battledamagemsg.m_target[m_index][m_battleactor[m_index].m_attack_times_index];
            settargetpos();
        }else{
            m_battleactor[m_index].m_battlestate = 5;
        }
    }else if(m_battleactor[m_index].m_battlestate == 5 || m_battleactor[m_index].m_actorstate == 1){//回合结束状态
        if((m_battleend = battle_end()) == true){
            battlemsg tmp;
            if(m_hero.m_team_state == 0 || m_hero.m_team_state == 1){
                tmp.m_type = 2;
                tmp.m_battle_id = m_battle_id;
                tcpsocket.write((char *)&tmp, sizeof(battlemsg));
            }
            tcpsocket.disconnectFromHost();
            return;
        }
        int spd_max = -1;
        m_index = -1;
        for(int i=0;i<20;i++){
            if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1 && m_battleactor[i].m_battlestate == 0 && m_battleactor[i].m_spd > spd_max){
                spd_max = m_battleactor[i].m_spd;
                m_index = i;
            }
        }
        return;
    }else if(m_battleactor[m_index].m_battlestate == 0){//待机状态
        m_battleactor[m_index].m_skill_id = m_battledamagemsg.m_skill_id[m_index];
        m_battleactor[m_index].m_target = m_battledamagemsg.m_target[m_index][0];
        m_battleactor[m_index].m_attack_times = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_attack_times;
        if(skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_skill_type == 0){//普通攻击
            m_battleactor[m_index].m_battlestate = 1;
            settargetpos();
        }else{//技能
            m_battleactor[m_index].m_battlestate = 4;
            m_battleactor[m_index].m_skill_posfix = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_attack_posfix;
            m_battleactor[m_index].m_skill.setFileName(skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_attack_eff);
            m_battleactor[m_index].m_skill.start();
            QObject::connect(&m_battleactor[m_index].m_skill,SIGNAL(frameChanged(int)),this,SLOT(cast(int)));
            if(skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_skill_type == 2){
                m_battleactor[m_index].m_state = 6;
                m_battleactor[m_index].m_act.stop();
                m_battleactor[m_index].m_act.setFileName(heroinfo::m_heroinfo[m_battleactor[m_index].m_id].m_path+QString::number(m_battleactor[m_index].m_dir)+"_"+QString::number(m_battleactor[m_index].m_state)+".gif");
                m_battleactor[m_index].m_act.start();
            }
        }
    }else if(m_battleactor[m_index].m_battlestate == 1 || m_battleactor[m_index].m_battlestate == 3){//移动中
        move();
    }
}

//攻击动作回调
void battle::attack(int framenum){
    if(framenum == 0){//攻击结束
        m_battleactor[m_index].m_attack_times--;
        QObject::disconnect(&m_battleactor[m_index].m_act,SIGNAL(frameChanged(int)),this,SLOT(attack(int)));
        if(m_battleactor[m_index].m_attack_times == 0){//攻击次数结束
            m_battleactor[m_index].m_battlestate = 3;
            settargetpos();
        }else{//等待下一次攻击
            m_battleactor[m_index].m_battlestate = 7;
            m_battleactor[m_index].m_state = 0;
            m_battleactor[m_index].m_act.stop();
            m_battleactor[m_index].m_act.setFileName(heroinfo::m_heroinfo[m_battleactor[m_index].m_id].m_path+QString::number(m_battleactor[m_index].m_dir)+"_"+QString::number(m_battleactor[m_index].m_state)+".gif");
            m_battleactor[m_index].m_act.start();
        }
    }else{//触发敌人受伤
        if(framenum == heroinfo::m_heroinfo[m_battleactor[m_index].m_id].m_hurt_index){
            int target = m_battleactor[m_index].m_target;
            int damage = m_battledamagemsg.m_damage[m_index][m_battleactor[m_index].m_attack_times_index];
            set_damage(target, damage);
        }
    }
}

//施法回调
void battle::cast(int framenum){
    if(framenum == 0){
        QObject::disconnect(&m_battleactor[m_index].m_skill,SIGNAL(frameChanged(int)),this,SLOT(cast(int)));
        m_battleactor[m_index].m_skill.stop();
        if(skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_skill_type == 1){//近战技能
            m_battleactor[m_index].m_battlestate = 1;
            settargetpos();
        }else{//远程技能
            m_battleactor[m_index].m_state = 0;
            m_battleactor[m_index].m_act.stop();
            m_battleactor[m_index].m_act.setFileName(heroinfo::m_heroinfo[m_battleactor[m_index].m_id].m_path+QString::number(m_battleactor[m_index].m_dir)+"_"+QString::number(m_battleactor[m_index].m_state)+".gif");
            m_battleactor[m_index].m_act.start();

            m_battleactor[m_index].m_battlestate = 6;
            int target = m_battleactor[m_index].m_target;
            int target_type = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_target_type;
            int target_list[20][4] = {{0,1,2,5},{1,0,3,6},{2,0,4,7},{3,1,8,-1},{4,2,9,-1},
                                      {5,6,7,0},{6,5,8,1},{7,5,9,2},{8,6,3,-1},{9,7,4,-1},
                                      {10,11,12,15},{11,10,13,16},{12,10,14,17},{13,11,18,-1},{14,12,19,-1},
                                      {15,16,17,10},{16,15,18,11},{17,15,19,12},{18,16,13,-1},{19,17,14,-1}};
            if(target_type == 0 || target_type == 3){//单体
                if(target_type == 0){
                    int damage = m_battledamagemsg.m_damage[m_index][0];
                    set_damage(target,damage);
                }else{
                    int damage = m_battledamagemsg.m_damage[m_index][0];
                    set_heal(target,damage);
                }
            }else if(target_type == 6){
                int damage = m_battledamagemsg.m_damage[m_index][0];
                set_res(target,damage);
            }else if(target_type == 1 || target_type == 4){//十字
                for(int i=0;i<4;i++){
                    int t_target = target_list[target][i];
                    if(t_target != -1 && m_battleactor[t_target].m_type != -1 && m_battleactor[t_target].m_actorstate != 1){
                        if(target_type == 1){
                            int damage = m_battledamagemsg.m_damage[m_index][i];
                            set_damage(t_target,damage);
                        }else{
                            int damage = m_battledamagemsg.m_damage[m_index][i];
                            set_heal(t_target,damage);
                        }
                    }
                }
            }else if((target_type == 2 && m_battleactor[m_index].m_type == 2) || (target_type == 5 && m_battleactor[m_index].m_type < 2)){//目标为我方
                for(int i=0;i<10;i++){
                    if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1){
                        if(target_type == 2){
                            int damage = m_battledamagemsg.m_damage[m_index][i];
                            set_damage(i,damage);
                        }else{
                            int damage = m_battledamagemsg.m_damage[m_index][i];
                            set_heal(i,damage);
                        }
                    }
                }
            }else{//目标为敌方
                for(int i=10;i<20;i++){
                    if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1){
                        if(target_type == 2){
                            int damage = m_battledamagemsg.m_damage[m_index][i-10];
                            set_damage(i,damage);
                        }else{
                            int damage = m_battledamagemsg.m_damage[m_index][i-10];
                            set_heal(i,damage);
                        }
                    }
                }
            }
        }
    }
}

//设置伤害
void battle::set_damage(int target,int damage){
    m_battleactor[target].m_state = 8;
    m_battleactor[target].m_be_attack = true;
    m_battleactor[target].m_act.stop();
    m_battleactor[target].m_act.setFileName(heroinfo::m_heroinfo[m_battleactor[target].m_id].m_path+QString::number(m_battleactor[target].m_dir)+"_"+QString::number(m_battleactor[target].m_state)+".gif");
    m_battleactor[target].m_act.start();
    m_battleactor[target].m_skill_posfix = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_be_attack_posfix;
    m_battleactor[target].m_skill.setFileName(skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_be_attack_eff);
    m_battleactor[target].m_skill.start();
    m_battleactor[target].m_hp -= damage;
    if(m_battleactor[target].m_hp <= 0){
        m_battleactor[target].m_hp = 0;
        m_battleactor[target].m_actorstate = 1;
    }
    int num = 0;
    int tmp_damage = damage;
    while(tmp_damage > 0){
        tmp_damage /= 10;
        num++;
    }
    QImage image_damage(12*num,16,QImage::Format_ARGB32);
    image_damage.fill(Qt::transparent);
    QPainter image_painter(&image_damage);
    while(damage > 0){
        num--;
        image_painter.drawImage(QRect(12*num,0,12,16),QImage(":/images/damage_"+QString::number(damage%10)+".png"));
        damage /= 10;
    }
    m_battleactor[target].m_damagenum = image_damage;
    QObject::connect(&m_battleactor[target].m_act,SIGNAL(frameChanged(int)),&m_battleactor[target],SLOT(be_attack(int)));
    QObject::connect(&m_battleactor[target].m_skill,SIGNAL(frameChanged(int)),&m_battleactor[target],SLOT(skill(int)));
}

//设置治疗
void battle::set_heal(int target,int damage){
    m_battleactor[target].m_be_attack = true;
    m_battleactor[target].m_skill_posfix = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_be_attack_posfix;
    m_battleactor[target].m_skill.setFileName(skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_be_attack_eff);
    m_battleactor[target].m_skill.start();
    m_battleactor[target].m_hp += damage;
    if(m_battleactor[target].m_hp > m_battleactor[target].m_max_hp){
        m_battleactor[target].m_hp = m_battleactor[target].m_max_hp;
    }
    int num = 0;
    int tmp_damage = damage;
    while(tmp_damage > 0){
        tmp_damage /= 10;
        num++;
    }
    QImage image_damage(12*num,16,QImage::Format_ARGB32);
    image_damage.fill(Qt::transparent);
    QPainter image_painter(&image_damage);
    while(damage > 0){
        num--;
        image_painter.drawImage(QRect(12*num,0,12,16),QImage(":/images/heal_"+QString::number(damage%10)+".png"));
        damage /= 10;
    }
    m_battleactor[target].m_damagenum = image_damage;
    QObject::connect(&m_battleactor[target].m_skill,SIGNAL(frameChanged(int)),&m_battleactor[target],SLOT(be_heal(int)));
}

//设置复活
void battle::set_res(int target,int damage){
    m_battleactor[target].m_be_attack = true;
    m_battleactor[target].m_skill_posfix = skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_be_attack_posfix;
    m_battleactor[target].m_skill.setFileName(skillinfo::m_skillinfo[m_battleactor[m_index].m_skill_id].m_be_attack_eff);
    m_battleactor[target].m_skill.start();
    QObject::connect(&m_battleactor[target].m_skill,SIGNAL(frameChanged(int)),&m_battleactor[target],SLOT(be_heal(int)));
    if(damage == 0){
        m_battleactor[target].m_damagenum = QImage("");
        return;
    }
    m_battleactor[target].m_actorstate = 0;
    m_battleactor[target].m_state = 0;
    m_battleactor[target].m_act.stop();
    m_battleactor[target].m_act.setFileName(heroinfo::m_heroinfo[m_battleactor[target].m_id].m_path+QString::number(m_battleactor[target].m_dir)+"_"+QString::number(m_battleactor[target].m_state)+".gif");
    m_battleactor[target].m_act.start();
    m_battleactor[target].m_hp += damage;
    if(m_battleactor[target].m_hp > m_battleactor[target].m_max_hp){
        m_battleactor[target].m_hp = m_battleactor[target].m_max_hp;
    }
    int num = 0;
    int tmp_damage = damage;
    while(tmp_damage > 0){
        tmp_damage /= 10;
        num++;
    }
    QImage image_damage(12*num,16,QImage::Format_ARGB32);
    image_damage.fill(Qt::transparent);
    QPainter image_painter(&image_damage);
    while(damage > 0){
        num--;
        image_painter.drawImage(QRect(12*num,0,12,16),QImage(":/images/heal_"+QString::number(damage%10)+".png"));
        damage /= 10;
    }
    m_battleactor[target].m_damagenum = image_damage;
}

//战斗结束
bool battle::battle_end(void){
    bool win = true;
    for(int i=10;i<20;i++){
        if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1){
            win = false;
            break;
        }
    }
    bool lose = true;
    for(int i=0;i<10;i++){
        if(m_battleactor[i].m_type != -1 && m_battleactor[i].m_actorstate != 1){
            lose = false;
            break;
        }
    }
    if(win || lose){
        return true;
    }else{
        return false;
    }
}

//倒计时
void battle::timerEvent(QTimerEvent *event){
    if(event->timerId() == m_turntime){
        m_battle_section = 3;
        m_skill_select = m_hero.m_skill_list[m_skill_index];
        battlemsg tmp;
        tmp.m_type = 1;
        tmp.m_battle_id = m_battle_id;
        tmp.m_uid = m_hero.m_uid;
        tmp.m_battle_pos = m_hero.m_team_pos;
        tmp.m_hero_skill_id = m_battleactor[m_hero.m_team_pos].m_skill_id;
        tmp.m_hero_target = m_battleactor[m_hero.m_team_pos].m_target;
        tmp.m_pet_skill_id = m_battleactor[m_hero.m_team_pos+5].m_skill_id;
        tmp.m_pet_target = m_battleactor[m_hero.m_team_pos+5].m_target;
        tcpsocket.write((char *)&tmp, sizeof(battlemsg));
        killTimer(m_turntime);
        killTimer(m_secondtime);
    }else if(event->timerId() == m_secondtime){
        m_lefttime--;
    }
}

//连接错误
void battle::onerror(QAbstractSocket::SocketError){
    QMessageBox::critical(NULL,"ERROR",tcpsocket.errorString());
    exit(0);
}

//新连接
void battle::onconnect(void){
    battlemsg tmp;
    tmp.m_type = 0;
    tmp.m_battle_id = m_battle_id;
    tmp.m_uid = m_hero.m_uid;
    tmp.m_battle_pos = m_hero.m_team_pos;
    tcpsocket.write((char *)&tmp, sizeof(battlemsg));
}

//接受战斗消息
void battle::onreadyread(void){
    while(tcpsocket.bytesAvailable()){
        battledamagemsg tmp;
        tcpsocket.read((char *)&tmp, sizeof(battledamagemsg));
        if(tmp.m_type == 0){
            m_battleactor[tmp.m_pos].m_battleready = true;
        }else if(tmp.m_type == 1){
            m_battleactor[tmp.m_pos].m_connect = false;
        }else if(tmp.m_type == 2){
            m_battledamagemsg = tmp;
            for(int i=0;i<20;i++){
                m_battleactor[i].m_skill_id = m_battledamagemsg.m_skill_id[i];
            }
            m_battle_section = 2;
        }
    }
}

