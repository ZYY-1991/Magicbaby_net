#include "serverdialog.h"
#include "ui_serverdialog.h"
#include "battleactor.h"
#include "heroinfo.h"
#include "skillinfo.h"
#include "monstergroupinfo.h"
#include "battledamagemsg.h"

serverdialog::serverdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::serverdialog)
{
    ui->setupUi(this);
    qsrand(QTime::currentTime().msec());
    m_uid = 1;
    m_battle_id = 1;
    m_flushtime = startTimer(20);
    m_monster.m_type = 4;
    m_monster.m_hero_id = 201;
    m_monster.m_uid = 1;
    m_monster.m_dir = 4;
    int x = qrand()%1000+100;
    int y = qrand()%600+100;
    m_monster.m_pos = QPoint(x,y);
}

serverdialog::~serverdialog()
{
    delete ui;
}

void serverdialog::on_pushButton_clicked()
{
    short port = ui->mapportEdit->text().toShort();
    bool res = tcpserver.listen(QHostAddress::Any,port);
    port = ui->battleportEdit->text().toShort();
    bool res2 = tcpbattleserver.listen(QHostAddress::Any,port);
    if(res == false || res2 == false){
        qDebug("创建服务器失败！");
        return;
    }
    ui->pushButton->setEnabled(false);
    connect(&tcpserver,SIGNAL(newConnection()),this,SLOT(onnewconnection()));
    connect(&tcpbattleserver,SIGNAL(newConnection()),this,SLOT(onbattleconnection()));
}

void serverdialog::onnewconnection(void){
    QTcpSocket *tcpclientsocket = tcpserver.nextPendingConnection();
    heromsg *new_hero = new heromsg;
    tcpclientlist.append(qMakePair(tcpclientsocket,new_hero));
    connect(tcpclientsocket,SIGNAL(readyRead()),this,SLOT(onreadyread()));
}

void serverdialog::onbattleconnection(void){
    QTcpSocket *tcpclientsocket = tcpbattleserver.nextPendingConnection();
    battlemsg *new_battle = new battlemsg;
    tcpbattleclientlist.append(qMakePair(tcpclientsocket,new_battle));
    connect(tcpclientsocket,SIGNAL(readyRead()),this,SLOT(onbattlereadyread()));
}

void serverdialog::onreadyread(void){
    ondisconnect();
    for(int i=0;i<tcpclientlist.size();i++){
        while(tcpclientlist.at(i).first->bytesAvailable()){
            heromsg tmp;
            tcpclientlist.at(i).first->read((char *)&tmp, sizeof(heromsg));
            if(tmp.m_type == 0){//有角色登录
                //检测UID
                *tcpclientlist.at(i).second = tmp;
                for(int j=0;j<tcpclientlist.size();j++){
                    if(j!=i && tcpclientlist.at(j).second->m_uid == tmp.m_uid){
                        tcpclientlist.at(i).second->m_type = 2;
                        tcpclientlist.at(i).first->write((char *)tcpclientlist.at(i).second, sizeof(heromsg));
                        delete tcpclientlist.at(i).second;
                        tcpclientlist.removeAt(i);
                        return;
                    }
                }
                //发送怪物信息
                tcpclientlist.at(i).first->write((char *)&m_monster, sizeof(heromsg));
                //发送其他角色信息给他
                for(int j=0;j<tcpclientlist.size();j++){
                    if(j != i){
                        tcpclientlist.at(j).second->m_type = 0;
                        tcpclientlist.at(i).first->write((char *)tcpclientlist.at(j).second, sizeof(heromsg));
                    };
                }
                //发送他的信息给其他客户端
                tcpclientlist.at(i).second->m_type = 0;
                for(int j=0;j<tcpclientlist.size();j++){
                    if(j != i){
                        tcpclientlist.at(j).first->write((char *)tcpclientlist.at(i).second, sizeof(heromsg));
                    }
                }
            }else if(tmp.m_type  == 1){//角色状态改变
                tmp.m_type = 1;
                //更新服务器信息
                for(int j=0;j<tcpclientlist.size();j++){
                    if(tcpclientlist.at(j).second->m_uid == tmp.m_uid){
                        *tcpclientlist.at(j).second = tmp;
                        break;
                    }
                }
                //发送给其他玩家
                for(int j=0;j<tcpclientlist.size();j++){
                    if(j != i){
                        tcpclientlist.at(j).first->write((char *)&tmp, sizeof(heromsg));
                    }
                }
            }else if(tmp.m_type == 2){//进入战斗
                tmp.m_type = 5;
                tmp.m_battle_id = m_battle_id;
                battle *p_battle = new battle;
                battlelist.append(p_battle);
                p_battle->m_battle_id = tmp.m_battle_id;
                p_battle->create_battle(tmp.m_monstergroup_id);
                m_battle_id++;
                int pos = tcpclientlist.at(i).second->m_team_pos;
                p_battle->m_battleactor[pos].sethero(*tcpclientlist.at(i).second);
                p_battle->m_battleactor[pos+5].setpet(tcpclientlist.at(i).second->m_pet);
                tcpclientlist.at(i).first->write((char *)&tmp, sizeof(heromsg));
                for(int k=0;k<4;k++){
                    if(tmp.m_team_member[k] != 0){
                        for(int j=0;j<tcpclientlist.size();j++){
                            if(tcpclientlist.at(j).second->m_uid == tmp.m_team_member[k]){
                                pos = tcpclientlist.at(j).second->m_team_pos;
                                p_battle->m_battleactor[pos].sethero(*tcpclientlist.at(j).second);
                                p_battle->m_battleactor[pos+5].setpet(tcpclientlist.at(j).second->m_pet);
                                tcpclientlist.at(j).first->write((char *)&tmp, sizeof(heromsg));
                            }
                        }
                    }
                }
                p_battle->turn_begin();
            }else if(tmp.m_type == 3){
                for(int j=0;j<tcpclientlist.size();j++){
                    if(tcpclientlist.at(j).second->m_uid == tmp.m_target){
                        tmp.m_type = 6;
                        tcpclientlist.at(j).first->write((char *)&tmp, sizeof(heromsg));
                        break;
                    }
                }
            }
        }
    }
}

void serverdialog::onbattlereadyread(void){
    onbattledisconnect();
    for(int i=0;i<tcpbattleclientlist.size();i++){
        while(tcpbattleclientlist.at(i).first->bytesAvailable()){
            battlemsg tmp;
            tcpbattleclientlist.at(i).first->read((char *)&tmp, sizeof(battlemsg));
            if(tmp.m_type == 0){//新连接
                *tcpbattleclientlist.at(i).second = tmp;
            }else if(tmp.m_type == 1){//下指令
                for(int k=0;k<tcpbattleclientlist.size();k++){
                    if(tcpbattleclientlist.at(k).second->m_battle_id == tmp.m_battle_id){
                        battledamagemsg tmp2;
                        tmp2.m_type = 0;
                        tmp2.m_pos = tcpbattleclientlist.at(i).second->m_battle_pos;
                        tcpbattleclientlist.at(k).first->write((char *)&tmp2, sizeof(battledamagemsg));
                    }
                }
                for(int j=0;j<battlelist.size();j++){
                    if(battlelist.at(j)->m_battle_id == tmp.m_battle_id){
                        if(battlelist.at(j)->command(tmp) == true){
                            for(int k=0;k<tcpbattleclientlist.size();k++){
                                if(tcpbattleclientlist.at(k).second->m_battle_id == tmp.m_battle_id){
                                    battlelist.at(j)->m_battledamagemsg.m_type = 2;
                                    tcpbattleclientlist.at(k).first->write((char *)&(battlelist.at(j)->m_battledamagemsg),sizeof(battledamagemsg));
                                }
                            }
                        }
                        break;
                    }
                }
            }else if(tmp.m_type == 2){//战斗结束
                for(int j=0;j<battlelist.size();j++){
                    if(battlelist.at(j)->m_battle_id == tmp.m_battle_id){
                        delete battlelist.at(j);
                        battlelist.removeAt(j);
                        break;
                    }
                }
                int x = qrand()%1000+100;
                int y = qrand()%600+100;
                m_monster.m_pos = QPoint(x,y);
                for(int j=0;j<tcpclientlist.size();j++){
                    tcpclientlist.at(j).first->write((char *)&m_monster, sizeof(heromsg));
                }
            }
        }
    }
}

void serverdialog::timerEvent(QTimerEvent *event){
    if(event->timerId() == m_flushtime){
        ondisconnect();
        onbattledisconnect();
        for(int i=0;i<tcpclientlist.size();i++){
            if(tcpclientlist.at(i).second != NULL && tcpclientlist.at(i).second->m_targetpos != tcpclientlist.at(i).second->m_pos){
                tcpclientlist.at(i).second->move();
            }
        }
    }
}

void serverdialog::ondisconnect(void){
    for(int i=0;i<tcpclientlist.size();i++){
        while(i<tcpclientlist.size() && tcpclientlist.at(i).first->state() == QAbstractSocket::UnconnectedState){
            tcpclientlist.at(i).second->m_type = 3;
            for(int j=0;j<tcpclientlist.size();j++){
                if(j != i){
                    tcpclientlist.at(j).first->write((char *)tcpclientlist.at(i).second, sizeof(heromsg));
                }
            }
            if(tcpclientlist.at(i).second->m_team_state == 1){//队长掉线
                for(int j=0;j<4;j++){
                    if(tcpclientlist.at(i).second->m_team_member[j] != 0){
                        for(int k=0;k<tcpclientlist.size();k++){
                            if(tcpclientlist.at(i).second->m_team_member[j] == tcpclientlist.at(k).second->m_uid){
                                tcpclientlist.at(k).second->m_team_state = 0;
                                tcpclientlist.at(k).second->m_team_pos = 0;
                                for(int kk=0;kk<4;kk++){
                                    tcpclientlist.at(k).second->m_team_member[kk] = 0;
                                }
                                tcpclientlist.at(k).second->m_type = 1;
                                for(int kk=0;kk<tcpclientlist.size();kk++){
                                    if(kk != i){
                                        tcpclientlist.at(kk).first->write((char *)tcpclientlist.at(k).second, sizeof(heromsg));
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }else if(tcpclientlist.at(i).second->m_team_state == 2){//队员掉线
                for(int j=0;j<4;j++){
                    if(tcpclientlist.at(i).second->m_team_member[j] != 0){
                        for(int k=0;k<tcpclientlist.size();k++){
                            if(tcpclientlist.at(i).second->m_team_member[j] == tcpclientlist.at(k).second->m_uid){
                                for(int kk=0;kk<4;kk++){
                                    if(tcpclientlist.at(k).second->m_team_member[kk] == tcpclientlist.at(i).second->m_uid){
                                        tcpclientlist.at(k).second->m_team_member[kk] = 0;
                                        break;
                                    }
                                }
                                if(tcpclientlist.at(k).second->m_team_pos > tcpclientlist.at(i).second->m_team_pos){
                                    tcpclientlist.at(k).second->m_team_pos--;
                                }
                                for(int kk=0;kk<tcpclientlist.size();kk++){
                                    if(kk != i){
                                        tcpclientlist.at(kk).first->write((char *)tcpclientlist.at(k).second, sizeof(heromsg));
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
            delete tcpclientlist.at(i).second;
            tcpclientlist.removeAt(i);
        }
    }
}

void serverdialog::onbattledisconnect(void){
    for(int i=0;i<tcpbattleclientlist.size();i++){
        while(i<tcpbattleclientlist.size() && tcpbattleclientlist.at(i).first->state() == QAbstractSocket::UnconnectedState){
            int battle_id = tcpbattleclientlist.at(i).second->m_battle_id;
            int pos = tcpbattleclientlist.at(i).second->m_battle_pos;
            for(int j=0;j<battlelist.size();j++){
                if(battlelist.at(j)->m_battle_id == battle_id){//找到对应战斗
                    battlelist.at(j)->m_battleactor[pos].m_connect = false;
                    //如果全部掉线则删除战斗
                    int k;
                    for(k=0;k<5;k++){
                        if(battlelist.at(j)->m_battleactor[k].m_type != -1 && battlelist.at(j)->m_battleactor[k].m_connect == true){
                            break;
                        }
                    }
                    if(k==5){
                        delete battlelist.at(j);
                        battlelist.removeAt(j);
                        break;
                    }
                    //给战斗中其他人发消息
                    for(k=0;k<tcpbattleclientlist.size();k++){
                        if(tcpbattleclientlist.at(k).second->m_battle_id == battle_id && k != i){
                            battledamagemsg tmp;
                            tmp.m_type = 1;
                            tmp.m_pos = tcpbattleclientlist.at(i).second->m_battle_pos;
                            tcpbattleclientlist.at(k).first->write((char *)&tmp, sizeof(battledamagemsg));
                        }
                    }
                    //检测是否所有人下达指令结束
                    if(battlelist.at(j)->test_begin() == true){
                        for(k=0;k<tcpbattleclientlist.size();k++){
                            if(tcpbattleclientlist.at(k).second->m_battle_id == battle_id && k!=i){
                                battlelist.at(j)->m_battledamagemsg.m_type = 2;
                                tcpbattleclientlist.at(k).first->write((char *)&(battlelist.at(j)->m_battledamagemsg),sizeof(battledamagemsg));
                            }
                        }
                    }
                    break;
                }
            }
            delete tcpbattleclientlist.at(i).second;
            tcpbattleclientlist.removeAt(i);
        }
    }
}

void serverdialog::init(void){
    //螳螂
    {
    heroinfo::m_heroinfo[201].m_atk = 200;
    heroinfo::m_heroinfo[201].m_def = 50;
    heroinfo::m_heroinfo[201].m_hp = 1000;
    heroinfo::m_heroinfo[201].m_spd = 10;
    }
    //露比
    {
    heroinfo::m_heroinfo[211].m_atk = 400;
    heroinfo::m_heroinfo[211].m_def = 50;
    heroinfo::m_heroinfo[211].m_hp = 2000;
    heroinfo::m_heroinfo[211].m_spd = 20;
    }
    //老鼠
    {
    heroinfo::m_heroinfo[212].m_atk = 200;
    heroinfo::m_heroinfo[212].m_def = 50;
    heroinfo::m_heroinfo[212].m_hp = 1500;
    heroinfo::m_heroinfo[212].m_spd = 10;
    }
    //刀
    {
    heroinfo::m_heroinfo[213].m_atk = 150;
    heroinfo::m_heroinfo[213].m_def = 50;
    heroinfo::m_heroinfo[213].m_hp = 1000;
    heroinfo::m_heroinfo[213].m_spd = 10;
    }
    //普攻
    skillinfo::m_skillinfo[0].m_skill_type = 0;
    skillinfo::m_skillinfo[0].m_target_type = 0;
    skillinfo::m_skillinfo[0].m_attack_times = 1;
    skillinfo::m_skillinfo[0].m_damage_value1 = 1;
    skillinfo::m_skillinfo[0].m_damage_value2 = 0;
    //连击
    skillinfo::m_skillinfo[1].m_skill_type = 1;
    skillinfo::m_skillinfo[1].m_target_type = 0;
    skillinfo::m_skillinfo[1].m_attack_times = 3;
    skillinfo::m_skillinfo[1].m_damage_value1 = 0.8;
    skillinfo::m_skillinfo[1].m_damage_value2 = 0;
    //乾坤
    skillinfo::m_skillinfo[2].m_skill_type = 1;
    skillinfo::m_skillinfo[2].m_target_type = 0;
    skillinfo::m_skillinfo[2].m_attack_times = 1;
    skillinfo::m_skillinfo[2].m_damage_value1 = 2;
    skillinfo::m_skillinfo[2].m_damage_value2 = 0;
    //单冰
    skillinfo::m_skillinfo[3].m_skill_type = 2;
    skillinfo::m_skillinfo[3].m_target_type = 0;
    skillinfo::m_skillinfo[3].m_attack_times = 1;
    skillinfo::m_skillinfo[3].m_damage_value1 = 2;
    skillinfo::m_skillinfo[3].m_damage_value2 = 0;
    //强冰
    skillinfo::m_skillinfo[4].m_skill_type = 2;
    skillinfo::m_skillinfo[4].m_target_type = 1;
    skillinfo::m_skillinfo[4].m_attack_times = 1;
    skillinfo::m_skillinfo[4].m_damage_value1 = 0.6;
    skillinfo::m_skillinfo[4].m_damage_value2 = 0;
    //超冰
    skillinfo::m_skillinfo[5].m_skill_type = 2;
    skillinfo::m_skillinfo[5].m_target_type = 2;
    skillinfo::m_skillinfo[5].m_attack_times = 1;
    skillinfo::m_skillinfo[5].m_damage_value1 = 0.4;
    skillinfo::m_skillinfo[5].m_damage_value2 = 0;
    //单疗
    skillinfo::m_skillinfo[6].m_skill_type = 2;
    skillinfo::m_skillinfo[6].m_target_type = 3;
    skillinfo::m_skillinfo[6].m_attack_times = 1;
    skillinfo::m_skillinfo[6].m_damage_value1 = 3;
    skillinfo::m_skillinfo[6].m_damage_value2 = 0;
    //强疗
    skillinfo::m_skillinfo[7].m_skill_type = 2;
    skillinfo::m_skillinfo[7].m_target_type = 4;
    skillinfo::m_skillinfo[7].m_attack_times = 1;
    skillinfo::m_skillinfo[7].m_damage_value1 = 0.8;
    skillinfo::m_skillinfo[7].m_damage_value2 = 0;
    //群疗
    skillinfo::m_skillinfo[8].m_skill_type = 2;
    skillinfo::m_skillinfo[8].m_target_type = 5;
    skillinfo::m_skillinfo[8].m_attack_times = 1;
    skillinfo::m_skillinfo[8].m_damage_value1 = 0.5;
    skillinfo::m_skillinfo[8].m_damage_value2 = 0;
    //复活
    skillinfo::m_skillinfo[9].m_skill_type = 2;
    skillinfo::m_skillinfo[9].m_target_type = 6;
    skillinfo::m_skillinfo[9].m_attack_times = 1;
    skillinfo::m_skillinfo[9].m_damage_value1 = 1.5;
    skillinfo::m_skillinfo[9].m_damage_value2 = 0;

    {
    int monster_id[10] = {211,212,212,212,212,213,213,213,213,213};
    for(int i=0;i<10;i++){
        monstergroupinfo::m_monstergroupinfo[1].m_monster_id[i] = monster_id[i];
    }
    }

}
