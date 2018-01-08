#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "heromsg.h"
#include "heroinfo.h"
#include "skillinfo.h"
#include "monstergroupinfo.h"

mainwindow::mainwindow(QWidget *parent) :
    QDialog(parent),
    m_battle(m_hero)
{
    setWindowTitle("Game");
    m_flushtime = startTimer(20);
    setMouseTracking(true);
    qsrand(QTime::currentTime().msec());
    m_hero.init(2);
    m_window = 0;
    m_team.setParent(this);
    m_team.resize(100,50);
    m_team.move(0,100);
    m_team.setAutoDefault(false);
    m_invate.setParent(this);
    m_invate.setText("队伍邀请");
    m_invate.resize(100,50);
    m_invate.move(910,80);
    m_invate.setAutoDefault(false);
    m_look.setParent(this);
    m_look.setText("查看信息");
    m_look.resize(100,50);
    m_look.move(910,140);
    m_look.setAutoDefault(false);
    m_team.show();
    m_confirmbox.m_confirm.setParent(this);
    m_confirmbox.m_confirm.setText("接受");
    m_confirmbox.m_confirm.resize(60,50);
    m_confirmbox.m_confirm.move(630,490);
    m_confirmbox.m_confirm.setAutoDefault(false);
    m_confirmbox.m_confirm.hide();
    m_confirmbox.m_cancel.setParent(this);
    m_confirmbox.m_cancel.setText("拒绝");
    m_confirmbox.m_cancel.resize(60,50);
    m_confirmbox.m_cancel.move(510,490);
    m_confirmbox.m_cancel.setAutoDefault(false);
    m_confirmbox.m_cancel.hide();
    connect(&m_team,SIGNAL(clicked()),this,SLOT(team()));
    connect(&m_invate,SIGNAL(clicked()),this,SLOT(invate()));
    connect(&m_confirmbox.m_confirm,SIGNAL(clicked()),this,SLOT(confirm_invate()));
    connect(&m_confirmbox.m_cancel,SIGNAL(clicked()),&m_confirmbox,SLOT(cancel()));
    connect(&tcpsocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onerror(QAbstractSocket::SocketError)));
    connect(&tcpsocket,SIGNAL(connected()),this,SLOT(onconnect()));
    connect(&tcpsocket,SIGNAL(readyRead()),this,SLOT(onreadyread()));

    short port;
    QString IP;
    QString str;
    QFile file("config.info");
    if(file.exists() == false){
        QMessageBox::critical(this,"ERROR","配置文件错误");
        exit(0);
    }
    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    while(in.atEnd() == false){
        in >> str;
        if(str == "ip"){
            in >> IP;
        }else if(str == "port"){
            in >> port;
        }
    }
    file.close();
    QHostAddress ip(IP);
    tcpsocket.connectToHost(ip,port);
}

mainwindow::~mainwindow()
{
}

//鼠标按下
void mainwindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if(m_window == 0){
            m_hero.settargetpos(event->pos());
            heromsg tmp(m_hero);
            tmp.m_type = 1;
            tcpsocket.write((char *)&tmp, sizeof(tmp));
            m_hero.m_target = 0;
            for(int i=0;i<m_other_hero.size();i++){
                if(m_other_hero.at(i)->m_rect.contains(event->pos()) == true){
                    m_hero.m_target = m_other_hero.at(i)->m_uid;
                    break;
                }
            }
        }else if(m_window == 1){
            m_battle.mousepress(event);
        }
    }
}

//鼠标移动
void mainwindow::mouseMoveEvent(QMouseEvent *event){
    if(m_window == 1){
        m_battle.mousemove(event);
    }
    //qDebug() << QString().sprintf("%d %d", event->pos().x(), event->pos().y());
}

//键盘按下
void mainwindow::keyPressEvent(QKeyEvent *event){
    qDebug() << QString().sprintf("%x",event->key());
    /*if(event->key() == J){
        if(m_hero.m_state != 5){
            m_hero.attack();
        }
    }*/
}

//时间刷新
void mainwindow::timerEvent(QTimerEvent *event){
    if(event->timerId() == m_flushtime){
        if(m_window == 0){
            if(m_monster.m_rect.contains(m_hero.m_pos) == true){
                if(m_hero.m_team_state == 0 || m_hero.m_team_state == 1){
                    m_hero.m_targetpos = m_hero.m_pos;
                    heromsg tmp(m_hero);
                    tmp.m_type = 1;
                    tcpsocket.write((char *)&tmp, sizeof(heromsg));
                    heromsg tmp2(m_hero);
                    tmp2.m_type = 2;
                    tmp2.m_monstergroup_id = m_monster.m_monstergroup_id;
                    tcpsocket.write((char *)&tmp2, sizeof(heromsg));
                }
            }
            if(m_hero.m_targetpos != m_hero.m_pos){
                m_hero.move();
            }
            for(int i=0;i<m_other_hero.size();i++){
                if(m_other_hero.at(i)->m_targetpos != m_other_hero.at(i)->m_pos){
                    m_other_hero.at(i)->move();
                }
                QPoint posfix = QPoint(heroinfo::m_heroinfo[m_other_hero.at(i)->m_hero_id].m_posfix[m_other_hero.at(i)->m_state][m_other_hero.at(i)->m_dir][0],
                                       heroinfo::m_heroinfo[m_other_hero.at(i)->m_hero_id].m_posfix[m_other_hero.at(i)->m_state][m_other_hero.at(i)->m_dir][1]);
                m_other_hero.at(i)->m_rect = QRect(m_other_hero.at(i)->m_pos - posfix,m_other_hero.at(i)->m_act.currentImage().size());
            }
        }else{
            if(m_battle.m_battleend == true){
                m_window = 0;
                m_team.show();
                m_hero.battleend();
            }else{
                m_battle.timeevent();
            }
        }
        update();
    }
}

//显示
void mainwindow::paintEvent(QPaintEvent *){
    setFixedSize(1200,800);
    QPainter painter(this);
    if(m_window == 0){
        //背景
        painter.drawImage(QRect(0,0,1200,800),QImage(":/map/map.gif"));
        //其他角色
        for(int i=0;i<m_other_hero.size();i++){
            m_other_hero.at(i)->display(painter);
        }
        //自己
        m_hero.display(painter);
        //怪物
        if(m_monster.m_monster_id != 0){
            QPoint posfix = QPoint(heroinfo::m_heroinfo[m_monster.m_monster_id].m_posfix[m_monster.m_state][m_monster.m_dir][0],heroinfo::m_heroinfo[m_monster.m_monster_id].m_posfix[m_monster.m_state][m_monster.m_dir][1]);
            painter.drawImage(QRect(m_monster.m_pos - posfix,m_monster.m_act.currentImage().size()),m_monster.m_act.currentImage());
        }
        //目标头像
        if(m_hero.m_target != 0){
            int i;
            for(i=0;i<m_other_hero.size();i++){
                if(m_other_hero.at(i)->m_uid == m_hero.m_target){
                    QString icon=heroinfo::m_heroinfo[m_other_hero.at(i)->m_hero_id].m_icon_path;
                    painter.drawImage(QRect(800,0,64,72),QImage(icon));
                    painter.drawImage(QRect(900,0,120,200),QImage(":/images/back.png"));
                    painter.setPen(QPen(Qt::red,5));
                    painter.setFont(QFont("文泉驿等宽正黑", 15, QFont::Normal));
                    painter.drawText(QRect(910,10,100,30),Qt::AlignCenter,m_other_hero.at(i)->m_name);
                    m_invate.show();
                    m_look.show();
                    break;
                }
            }
            if(i == m_other_hero.size()){
                m_invate.hide();
                m_look.hide();
            }
        }else{
            m_invate.hide();
            m_look.hide();
        }
        //队伍信息
        if(m_hero.m_team_state != 0){
            QString icon=heroinfo::m_heroinfo[m_hero.m_hero_id].m_icon_path;
            int team_pos = m_hero.m_team_pos;
            painter.drawImage(QRect(0,150+72*team_pos,64,72),QImage(icon));
            painter.setPen(QPen(Qt::red,5));
            painter.setFont(QFont("文泉驿等宽正黑", 15, QFont::Normal));
            painter.drawText(QRect(70,150+72*team_pos,300,72),Qt::AlignVCenter,m_hero.m_name);
            for(int i=0;i<4;i++){
                if(m_hero.m_team_member[i] != 0){
                    for(int j=0;j<m_other_hero.size();j++){
                        if(m_other_hero.at(j)->m_uid == m_hero.m_team_member[i]){
                            icon = heroinfo::m_heroinfo[m_other_hero.at(j)->m_hero_id].m_icon_path;
                            team_pos = m_other_hero.at(j)->m_team_pos;
                            painter.drawImage(QRect(0,150+72*team_pos,64,72),QImage(icon));
                            painter.setPen(QPen(Qt::red,5));
                            painter.setFont(QFont("文泉驿等宽正黑", 15, QFont::Normal));
                            painter.drawText(QRect(70,150+72*team_pos,300,72),Qt::AlignVCenter,m_other_hero.at(j)->m_name);
                            break;
                        }
                    }
                }
            }
        }
        //队伍按钮
        if(m_hero.m_team_state == 0){
            m_team.setText("创建队伍");
        }else if(m_hero.m_team_state == 1){
            m_team.setText("解散队伍");
        }else if(m_hero.m_team_state == 2){
            m_team.setText("离开队伍");
        }
        //组队确认框
        if(m_confirmbox.m_show == true){
            m_confirmbox.display(painter);
        }
        //tips
        if(m_tips.m_show == true){
            m_tips.display(painter);
        }
    }else if(m_window == 1){
        m_battle.display(painter);
    }
}

//连接错误
void mainwindow::onerror(QAbstractSocket::SocketError){
    QMessageBox::critical(this,"ERROR",tcpsocket.errorString());
    exit(0);
}

//新连接
void mainwindow::onconnect(void){
    heromsg tmp(m_hero);
    tmp.m_type = 0;
    tcpsocket.write((char *)&tmp, sizeof(heromsg));
}

//接受消息
void mainwindow::onreadyread(void){
    while(tcpsocket.bytesAvailable()){
        heromsg tmp;
        tcpsocket.read((char *)&tmp, sizeof(heromsg));
        if(tmp.m_type == 0){//新角色上线
            hero *new_hero = new hero();
            new_hero->set_hero(tmp);
            m_other_hero.append(new_hero);
            new_hero->m_act.setFileName(heroinfo::m_heroinfo[new_hero->m_hero_id].m_path+QString::number(new_hero->m_dir)+"_"+QString::number(new_hero->m_state)+".gif");
            new_hero->m_act.start();
            new_hero->settargetpos(new_hero->m_targetpos);
        }else if(tmp.m_type == 1){//角色状态改变
            if(tmp.m_uid == m_hero.m_uid){
                m_hero.set_hero(tmp);
                m_hero.settargetpos(m_hero.m_targetpos);
            }else{
                for(int i=0;i<m_other_hero.size();i++){
                    if(m_other_hero.at(i)->m_uid == tmp.m_uid){
                        m_other_hero.at(i)->set_hero(tmp);
                        m_other_hero.at(i)->settargetpos(m_other_hero.at(i)->m_targetpos);
                        break;
                    }
                }
            }
        }else if(tmp.m_type == 2){//uid检测
            QMessageBox::critical(this,"ERROR","uid已存在");
            exit(0);
        }else if(tmp.m_type == 3){//角色离线
            for(int i=0;i<m_other_hero.size();i++){
                if(m_other_hero.at(i)->m_uid == tmp.m_uid){
                    delete m_other_hero.at(i);
                    m_other_hero.removeAt(i);
                    break;
                }
            }
        }else if(tmp.m_type == 4){//怪物信息
            m_monster.init(tmp);
        }else if(tmp.m_type == 5){//创建战斗
            m_window = 1;
            m_team.hide();
            hero *battle_tmp[5] = {0};
            battle_tmp[m_hero.m_team_pos] = &m_hero;
            for(int i=0;i<4;i++){
                if(m_hero.m_team_member[i] != 0){
                    for(int j=0;j<m_other_hero.size();j++){
                        if(m_hero.m_team_member[i] == m_other_hero.at(j)->m_uid){
                            battle_tmp[m_other_hero.at(j)->m_team_pos] = m_other_hero.at(j);
                            break;
                        }
                    }
                }
            }
            m_battle.create_battle(tmp.m_battle_id,tmp.m_monstergroup_id,battle_tmp);
        }else if(tmp.m_type == 6){//队伍邀请
            m_confirmbox.m_target = tmp.m_uid;
            m_confirmbox.m_msg = QString(tmp.m_name) + "邀请你加入队伍";
            m_confirmbox.m_show = true;
            m_confirmbox.m_confirm.show();
            m_confirmbox.m_cancel.show();
        }
    }
}

//点击队伍按钮
void mainwindow::team(void){
    if(m_hero.m_team_state == 0){//无队伍时创建队长
        m_hero.m_team_state = 1;
        heromsg tmp(m_hero);
        tmp.m_type = 1;
        tcpsocket.write((char *)&tmp, sizeof(heromsg));
    }else if(m_hero.m_team_state == 1){//队长解散队伍
        //处理队员
        for(int i=0;i<4;i++){
            if(m_hero.m_team_member[i] != 0){
                for(int j=0;j<m_other_hero.size();j++){
                    if(m_other_hero.at(j)->m_uid == m_hero.m_team_member[i]){
                        m_other_hero.at(j)->m_team_state = 0;
                        m_other_hero.at(j)->m_team_pos = 0;
                        for(int k=0;k<4;k++){
                            m_other_hero.at(j)->m_team_member[k] = 0;
                        }
                        heromsg tmp(*m_other_hero.at(j));
                        tmp.m_type = 1;
                        tcpsocket.write((char *)&tmp, sizeof(heromsg));
                        break;
                    }
                }
            }
        }
        //处理自己
        m_hero.m_team_state = 0;
        for(int i=0;i<4;i++){
            m_hero.m_team_member[i] = 0;
        }
        m_hero.m_team_pos = 0;
        heromsg tmp(m_hero);
        tmp.m_type = 1;
        tcpsocket.write((char *)&tmp, sizeof(heromsg));
    }else if(m_hero.m_team_state == 2){//队员离开队伍
        //处理队伍中其他人
        for(int i=0;i<4;i++){
            if(m_hero.m_team_member[i] != 0){
                for(int j=0;j<m_other_hero.size();j++){
                    if(m_other_hero.at(j)->m_uid == m_hero.m_team_member[i]){
                        for(int k=0;k<4;k++){
                            if(m_other_hero.at(j)->m_team_member[k] == m_hero.m_uid){
                                m_other_hero.at(j)->m_team_member[k] = 0;
                                break;
                            }
                        }
                        if(m_other_hero.at(j)->m_team_pos > m_hero.m_team_pos){
                            m_other_hero.at(j)->m_team_pos--;
                        }
                        heromsg tmp(*m_other_hero.at(j));
                        tmp.m_type = 1;
                        tcpsocket.write((char *)&tmp, sizeof(heromsg));
                        break;
                    }
                }
            }
        }
        //处理自己
        m_hero.m_team_state = 0;
        for(int i=0;i<4;i++){
            m_hero.m_team_member[i] = 0;
        }
        m_hero.m_team_pos = 0;
        heromsg tmp(m_hero);
        tmp.m_type = 1;
        tcpsocket.write((char *)&tmp, sizeof(heromsg));
    }
}

//邀请按钮
void mainwindow::invate(void){
    if(m_hero.m_team_state != 1){
        m_tips.send_tips("你不是队长！");
        return;
    }
    int i;
    for(i=0;i<4;i++){
        if(m_hero.m_team_member[i] == 0){
            break;
        }
    }
    if(i == 4){
        m_tips.send_tips("队伍已满！");
        return;
    }
    for(int i=0;i<m_other_hero.size();i++){
        if(m_other_hero.at(i)->m_uid == m_hero.m_target && m_other_hero.at(i)->m_team_state != 0){
            m_tips.send_tips("对方已有队伍！");
            return;
        }
    }
    heromsg tmp(m_hero);
    tmp.m_type = 3;
    tcpsocket.write((char *)&tmp, sizeof(heromsg));
}

//接受邀请按钮
void mainwindow::confirm_invate(void){
    m_confirmbox.m_confirm.hide();
    m_confirmbox.m_cancel.hide();
    m_confirmbox.m_show = false;
    if(m_hero.m_team_state != 0){
        m_tips.send_tips("你已经有队伍！");
        return;
    }
    for(int i=0;i<m_other_hero.size();i++){
        if(m_other_hero.at(i)->m_uid == m_confirmbox.m_target){//找到队长
            if(m_other_hero.at(i)->m_team_state != 1){
                m_tips.send_tips("目标队伍不存在！");
                return;
            }
            int num = 0;
            for(int j=0;j<4;j++){
                if(m_other_hero.at(i)->m_team_member[j] != 0){
                    num++;
                }
            }
            if(num == 4){
                m_tips.send_tips("目标队伍已满！");
                return;
            }
            //队伍状态改变
            m_hero.m_team_state = 2;
            m_hero.m_team_pos = num+1;
            num = 0;
            //处理队长
            m_hero.m_team_member[num] = m_other_hero.at(i)->m_uid;
            num++;
            for(int j=0;j<4;j++){
                if(m_other_hero.at(i)->m_team_member[j] == 0){
                    m_other_hero.at(i)->m_team_member[j] = m_hero.m_uid;
                    break;
                }
            }
            heromsg tmp(*m_other_hero.at(i));
            tmp.m_type = 1;
            tcpsocket.write((char *)&tmp, sizeof(heromsg));
            //处理其他队员
            for(int j=0;j<4;j++){
                if(m_other_hero.at(i)->m_team_member[j] != 0){
                    for(int k=0;k<m_other_hero.size();k++){
                        if(m_other_hero.at(k)->m_uid == m_other_hero.at(i)->m_team_member[j]){
                            m_hero.m_team_member[num] = m_other_hero.at(k)->m_uid;
                            num++;
                            for(int kk=0;kk<4;kk++){
                                if(m_other_hero.at(k)->m_team_member[kk] == 0){
                                    m_other_hero.at(k)->m_team_member[kk] = m_hero.m_uid;
                                    break;
                                }
                            }
                            heromsg tmp(*m_other_hero.at(k));
                            tmp.m_type = 1;
                            tcpsocket.write((char *)&tmp, sizeof(heromsg));
                            break;
                        }
                    }
                }
            }
            //处理自己
            heromsg tmp_self(m_hero);
            tmp_self.m_type = 1;
            tcpsocket.write((char *)&tmp_self, sizeof(heromsg));
        }
    }
}

//初始化
void mainwindow::init(void){
    heroinfo::m_heroinfo[1].m_path = ":/hero/hero1_";
    heroinfo::m_heroinfo[1].m_icon_path = ":/hero/hero1.png";
    {
    int stand[8][2] = {{20,64},{16,66},{21,66},{28,70},{20,74},{18,76},{29,74},{29,69}};
    int move[8][2] = {{27,71},{23,70},{43,70},{56,73},{35,71},{21,75},{38,76},{34,75}};
    int attack[8][2] = {{33,84},{22,87},{43,86},{50,87},{32,88},{40,89},{44,88},{45,86}};
    int cast[8][2] = {{31,75},{0,0},{0,0},{0,0},{20,72},{0,0},{0,0},{0,0}};
    int be_attack[8][2] = {{20,63},{0,0},{0,0},{0,0},{45,66},{0,0},{0,0},{0,0}};
    int dead[8][2] = {{65,64},{0,0},{0,0},{0,0},{33,67},{0,0},{0,0},{0,0}};
    for(int i=0;i<8;i++){
        for(int j=0;j<2;j++){
            heroinfo::m_heroinfo[1].m_posfix[0][i][j] = stand[i][j];
            heroinfo::m_heroinfo[1].m_posfix[3][i][j] = move[i][j];
            heroinfo::m_heroinfo[1].m_posfix[5][i][j] = attack[i][j];
            heroinfo::m_heroinfo[1].m_posfix[6][i][j] = cast[i][j];
            heroinfo::m_heroinfo[1].m_posfix[8][i][j] = be_attack[i][j];
            heroinfo::m_heroinfo[1].m_posfix[10][i][j] = dead[i][j];
        }
    }
    heroinfo::m_heroinfo[1].m_atk = 300;
    heroinfo::m_heroinfo[1].m_def = 5000;
    heroinfo::m_heroinfo[1].m_hp = 1000;
    heroinfo::m_heroinfo[1].m_spd = 20;
    heroinfo::m_heroinfo[1].m_hurt_index = 6;
    heroinfo::m_heroinfo[1].m_skill_list[0] = 0;
    heroinfo::m_heroinfo[1].m_skill_list[1] = 1;
    heroinfo::m_heroinfo[1].m_skill_list[2] = 2;
    heroinfo::m_heroinfo[1].m_skill_list[3] = 3;
    heroinfo::m_heroinfo[1].m_skill_list[4] = 4;
    heroinfo::m_heroinfo[1].m_skill_list[5] = 9;
    }

    heroinfo::m_heroinfo[2].m_path = ":/hero/hero2_";
    heroinfo::m_heroinfo[2].m_icon_path = ":/hero/hero2.png";
    {
    int stand[8][2] = {{25,63},{20,61},{26,64},{39,69},{40,73},{26,69},{38,72},{37,70}};
    int move[8][2] = {{26,65},{19,64},{29,65},{35,66},{31,68},{31,74},{26,72},{30,69}};
    int attack[8][2] = {{65,114},{0,0},{0,0},{0,0},{40,74},{0,0},{0,0},{0,0}};
    int cast[8][2] = {{26,77},{0,0},{0,0},{0,0},{23,76},{0,0},{0,0},{0,0}};
    int be_attack[8][2] = {{25,62},{0,0},{0,0},{0,0},{50,63},{0,0},{0,0},{0,0}};
    int dead[8][2] = {{55,69},{0,0},{0,0},{0,0},{55,73},{0,0},{0,0},{0,0}};
    for(int i=0;i<8;i++){
        for(int j=0;j<2;j++){
            heroinfo::m_heroinfo[2].m_posfix[0][i][j] = stand[i][j];
            heroinfo::m_heroinfo[2].m_posfix[3][i][j] = move[i][j];
            heroinfo::m_heroinfo[2].m_posfix[5][i][j] = attack[i][j];
            heroinfo::m_heroinfo[2].m_posfix[6][i][j] = cast[i][j];
            heroinfo::m_heroinfo[2].m_posfix[8][i][j] = be_attack[i][j];
            heroinfo::m_heroinfo[2].m_posfix[10][i][j] = dead[i][j];
        }
    }
    heroinfo::m_heroinfo[2].m_atk = 300;
    heroinfo::m_heroinfo[2].m_def = 50;
    heroinfo::m_heroinfo[2].m_hp = 1000;
    heroinfo::m_heroinfo[2].m_spd = 20;
    heroinfo::m_heroinfo[2].m_hurt_index = 6;
    heroinfo::m_heroinfo[2].m_skill_list[0] = 0;
    heroinfo::m_heroinfo[2].m_skill_list[1] = 4;
    heroinfo::m_heroinfo[2].m_skill_list[2] = 5;
    heroinfo::m_heroinfo[2].m_skill_list[3] = 6;
    heroinfo::m_heroinfo[2].m_skill_list[4] = 7;
    heroinfo::m_heroinfo[2].m_skill_list[5] = 8;
    }

    //螳螂
    heroinfo::m_heroinfo[101].m_path = ":/monster/monster1_";
    {
    int stand[8][2] = {{47,96},{0,0},{0,0},{0,0},{32,85},{0,0},{0,0},{0,0}};
    int move[8][2] = {{54,94},{0,0},{0,0},{0,0},{44,70},{0,0},{0,0},{0,0}};
    int attack[8][2] = {{86,117},{0,0},{0,0},{0,0},{33,113},{0,0},{0,0},{0,0}};
    int cast[8][2] = {{57,111},{0,0},{0,0},{0,0},{47,115},{0,0},{0,0},{0,0}};
    int be_attack[8][2] = {{65,85},{0,0},{0,0},{0,0},{44,108},{0,0},{0,0},{0,0}};
    int dead[8][2] = {{54,91},{0,0},{0,0},{0,0},{47,74},{0,0},{0,0},{0,0}};
    for(int i=0;i<8;i++){
        for(int j=0;j<2;j++){
            heroinfo::m_heroinfo[101].m_posfix[0][i][j] = stand[i][j];
            heroinfo::m_heroinfo[101].m_posfix[3][i][j] = move[i][j];
            heroinfo::m_heroinfo[101].m_posfix[5][i][j] = attack[i][j];
            heroinfo::m_heroinfo[101].m_posfix[6][i][j] = cast[i][j];
            heroinfo::m_heroinfo[101].m_posfix[8][i][j] = be_attack[i][j];
            heroinfo::m_heroinfo[101].m_posfix[10][i][j] = dead[i][j];
        }
    }
    heroinfo::m_heroinfo[101].m_name = "螳螂";
    heroinfo::m_heroinfo[101].m_atk = 200;
    heroinfo::m_heroinfo[101].m_def = 50;
    heroinfo::m_heroinfo[101].m_hp = 1000;
    heroinfo::m_heroinfo[101].m_spd = 10;
    heroinfo::m_heroinfo[101].m_hurt_index = 6;
    heroinfo::m_heroinfo[101].m_skill_list[0] = 0;
    heroinfo::m_heroinfo[101].m_skill_list[1] = 1;
    heroinfo::m_heroinfo[101].m_skill_list[2] = 2;

    }

    //螳螂
    heroinfo::m_heroinfo[201].m_path = ":/monster/monster1_";
    {
    int stand[8][2] = {{47,96},{0,0},{0,0},{0,0},{32,85},{0,0},{0,0},{0,0}};
    int move[8][2] = {{54,94},{0,0},{0,0},{0,0},{44,70},{0,0},{0,0},{0,0}};
    int attack[8][2] = {{86,117},{0,0},{0,0},{0,0},{33,113},{0,0},{0,0},{0,0}};
    int cast[8][2] = {{57,111},{0,0},{0,0},{0,0},{47,115},{0,0},{0,0},{0,0}};
    int be_attack[8][2] = {{65,85},{0,0},{0,0},{0,0},{44,108},{0,0},{0,0},{0,0}};
    int dead[8][2] = {{54,91},{0,0},{0,0},{0,0},{47,74},{0,0},{0,0},{0,0}};
    for(int i=0;i<8;i++){
        for(int j=0;j<2;j++){
            heroinfo::m_heroinfo[201].m_posfix[0][i][j] = stand[i][j];
            heroinfo::m_heroinfo[201].m_posfix[3][i][j] = move[i][j];
            heroinfo::m_heroinfo[201].m_posfix[5][i][j] = attack[i][j];
            heroinfo::m_heroinfo[201].m_posfix[6][i][j] = cast[i][j];
            heroinfo::m_heroinfo[201].m_posfix[8][i][j] = be_attack[i][j];
            heroinfo::m_heroinfo[201].m_posfix[10][i][j] = dead[i][j];
        }
    }
    heroinfo::m_heroinfo[201].m_name = "螳螂";
    heroinfo::m_heroinfo[201].m_atk = 200;
    heroinfo::m_heroinfo[201].m_def = 50;
    heroinfo::m_heroinfo[201].m_hp = 1000;
    heroinfo::m_heroinfo[201].m_spd = 10;
    heroinfo::m_heroinfo[201].m_hurt_index = 6;
    }
    //露比
    heroinfo::m_heroinfo[211].m_path = ":/hero/hero2_";
    {
    int stand[8][2] = {{25,63},{20,61},{26,64},{39,69},{40,73},{26,69},{38,72},{37,70}};
    int move[8][2] = {{26,65},{19,64},{29,65},{35,66},{31,68},{31,74},{26,72},{30,69}};
    int attack[8][2] = {{65,114},{0,0},{0,0},{0,0},{40,74},{0,0},{0,0},{0,0}};
    int cast[8][2] = {{26,77},{0,0},{0,0},{0,0},{23,76},{0,0},{0,0},{0,0}};
    int be_attack[8][2] = {{25,62},{0,0},{0,0},{0,0},{50,63},{0,0},{0,0},{0,0}};
    int dead[8][2] = {{55,69},{0,0},{0,0},{0,0},{55,73},{0,0},{0,0},{0,0}};
    for(int i=0;i<8;i++){
        for(int j=0;j<2;j++){
            heroinfo::m_heroinfo[211].m_posfix[0][i][j] = stand[i][j];
            heroinfo::m_heroinfo[211].m_posfix[3][i][j] = move[i][j];
            heroinfo::m_heroinfo[211].m_posfix[5][i][j] = attack[i][j];
            heroinfo::m_heroinfo[211].m_posfix[6][i][j] = cast[i][j];
            heroinfo::m_heroinfo[211].m_posfix[8][i][j] = be_attack[i][j];
            heroinfo::m_heroinfo[211].m_posfix[10][i][j] = dead[i][j];
        }
    }
    heroinfo::m_heroinfo[211].m_name = "露比";
    heroinfo::m_heroinfo[211].m_atk = 300;
    heroinfo::m_heroinfo[211].m_def = 50;
    heroinfo::m_heroinfo[211].m_hp = 2000;
    heroinfo::m_heroinfo[211].m_spd = 20;
    heroinfo::m_heroinfo[211].m_hurt_index = 6;
    }
    //老鼠
    heroinfo::m_heroinfo[212].m_path = ":/monster/monster2_";
    {
    int stand[8][2] = {{24,46},{0,0},{0,0},{0,0},{23,46},{0,0},{0,0},{0,0}};
    int move[8][2] = {{33,69},{0,0},{0,0},{0,0},{20,60},{0,0},{0,0},{0,0}};
    int attack[8][2] = {{40,43},{0,0},{0,0},{0,0},{25,39},{0,0},{0,0},{0,0}};
    int cast[8][2] = {{31,54},{0,0},{0,0},{0,0},{40,54},{0,0},{0,0},{0,0}};
    int be_attack[8][2] = {{29,40},{0,0},{0,0},{0,0},{40,53},{0,0},{0,0},{0,0}};
    int dead[8][2] = {{27,46},{0,0},{0,0},{0,0},{27,44},{0,0},{0,0},{0,0}};
    for(int i=0;i<8;i++){
        for(int j=0;j<2;j++){
            heroinfo::m_heroinfo[212].m_posfix[0][i][j] = stand[i][j];
            heroinfo::m_heroinfo[212].m_posfix[3][i][j] = move[i][j];
            heroinfo::m_heroinfo[212].m_posfix[5][i][j] = attack[i][j];
            heroinfo::m_heroinfo[212].m_posfix[6][i][j] = cast[i][j];
            heroinfo::m_heroinfo[212].m_posfix[8][i][j] = be_attack[i][j];
            heroinfo::m_heroinfo[212].m_posfix[10][i][j] = dead[i][j];
        }
    }
    heroinfo::m_heroinfo[212].m_name = "精灵鼠";
    heroinfo::m_heroinfo[212].m_atk = 200;
    heroinfo::m_heroinfo[212].m_def = 50;
    heroinfo::m_heroinfo[212].m_hp = 1500;
    heroinfo::m_heroinfo[212].m_spd = 10;
    heroinfo::m_heroinfo[212].m_hurt_index = 6;
    }
    //刀
    heroinfo::m_heroinfo[213].m_path = ":/monster/monster3_";
    {
    int stand[8][2] = {{12,81},{0,0},{0,0},{0,0},{10,78},{0,0},{0,0},{0,0}};
    int move[8][2] = {{22,67},{0,0},{0,0},{0,0},{28,48},{0,0},{0,0},{0,0}};
    int attack[8][2] = {{62,130},{0,0},{0,0},{0,0},{46,142},{0,0},{0,0},{0,0}};
    int cast[8][2] = {{46,111},{0,0},{0,0},{0,0},{15,103},{0,0},{0,0},{0,0}};
    int be_attack[8][2] = {{11,76},{0,0},{0,0},{0,0},{29,82},{0,0},{0,0},{0,0}};
    int dead[8][2] = {{54,77},{0,0},{0,0},{0,0},{13,73},{0,0},{0,0},{0,0}};
    for(int i=0;i<8;i++){
        for(int j=0;j<2;j++){
            heroinfo::m_heroinfo[213].m_posfix[0][i][j] = stand[i][j];
            heroinfo::m_heroinfo[213].m_posfix[3][i][j] = move[i][j];
            heroinfo::m_heroinfo[213].m_posfix[5][i][j] = attack[i][j];
            heroinfo::m_heroinfo[213].m_posfix[6][i][j] = cast[i][j];
            heroinfo::m_heroinfo[213].m_posfix[8][i][j] = be_attack[i][j];
            heroinfo::m_heroinfo[213].m_posfix[10][i][j] = dead[i][j];
        }
    }
    heroinfo::m_heroinfo[213].m_name = "烈焰之刃";
    heroinfo::m_heroinfo[213].m_atk = 150;
    heroinfo::m_heroinfo[213].m_def = 50;
    heroinfo::m_heroinfo[213].m_hp = 1000;
    heroinfo::m_heroinfo[213].m_spd = 10;
    heroinfo::m_heroinfo[213].m_hurt_index = 6;
    }
    //普攻
    skillinfo::m_skillinfo[0].m_skill_name = "普通攻击";
    skillinfo::m_skillinfo[0].m_skill_type = 0;
    skillinfo::m_skillinfo[0].m_target_type = 0;
    skillinfo::m_skillinfo[0].m_attack_times = 1;
    skillinfo::m_skillinfo[0].m_damage_value1 = 1;
    skillinfo::m_skillinfo[0].m_damage_value2 = 0;
    skillinfo::m_skillinfo[0].m_be_attack_eff = ":/skill/skill0_1.gif";
    skillinfo::m_skillinfo[0].m_be_attack_posfix = QPoint(66,100);
    //连击
    skillinfo::m_skillinfo[1].m_skill_name = "连击乱舞";
    skillinfo::m_skillinfo[1].m_skill_type = 1;
    skillinfo::m_skillinfo[1].m_target_type = 0;
    skillinfo::m_skillinfo[1].m_attack_times = 3;
    skillinfo::m_skillinfo[1].m_damage_value1 = 0.8;
    skillinfo::m_skillinfo[1].m_damage_value2 = 0;
    skillinfo::m_skillinfo[1].m_attack_eff = ":/skill/skill1_0.gif";
    skillinfo::m_skillinfo[1].m_attack_posfix = QPoint(150,200);
    skillinfo::m_skillinfo[1].m_be_attack_eff = ":/skill/skill1_1.gif";
    skillinfo::m_skillinfo[1].m_be_attack_posfix = QPoint(100,140);
    //乾坤
    skillinfo::m_skillinfo[2].m_skill_name = "乾坤一掷";
    skillinfo::m_skillinfo[2].m_skill_type = 1;
    skillinfo::m_skillinfo[2].m_target_type = 0;
    skillinfo::m_skillinfo[2].m_attack_times = 1;
    skillinfo::m_skillinfo[2].m_damage_value1 = 2;
    skillinfo::m_skillinfo[2].m_damage_value2 = 0;
    skillinfo::m_skillinfo[2].m_attack_eff = ":/skill/skill1_0.gif";
    skillinfo::m_skillinfo[2].m_attack_posfix = QPoint(150,200);
    skillinfo::m_skillinfo[2].m_be_attack_eff = ":/skill/skill2_1.gif";
    skillinfo::m_skillinfo[2].m_be_attack_posfix = QPoint(100,140);
    //单冰
    skillinfo::m_skillinfo[3].m_skill_name = "冰冻魔法";
    skillinfo::m_skillinfo[3].m_skill_type = 2;
    skillinfo::m_skillinfo[3].m_target_type = 0;
    skillinfo::m_skillinfo[3].m_attack_times = 1;
    skillinfo::m_skillinfo[3].m_damage_value1 = 2;
    skillinfo::m_skillinfo[3].m_damage_value2 = 0;
    skillinfo::m_skillinfo[3].m_attack_eff = ":/skill/skill3_0.gif";
    skillinfo::m_skillinfo[3].m_attack_posfix = QPoint(65,105);
    skillinfo::m_skillinfo[3].m_be_attack_eff = ":/skill/skill3_1.gif";
    skillinfo::m_skillinfo[3].m_be_attack_posfix = QPoint(70,180);
    //强冰
    skillinfo::m_skillinfo[4].m_skill_name = "强力冰冻";
    skillinfo::m_skillinfo[4].m_skill_type = 2;
    skillinfo::m_skillinfo[4].m_target_type = 1;
    skillinfo::m_skillinfo[4].m_attack_times = 1;
    skillinfo::m_skillinfo[4].m_damage_value1 = 0.6;
    skillinfo::m_skillinfo[4].m_damage_value2 = 0;
    skillinfo::m_skillinfo[4].m_attack_eff = ":/skill/skill3_0.gif";
    skillinfo::m_skillinfo[4].m_attack_posfix = QPoint(65,105);
    skillinfo::m_skillinfo[4].m_be_attack_eff = ":/skill/skill3_1.gif";
    skillinfo::m_skillinfo[4].m_be_attack_posfix = QPoint(70,180);
    //超冰
    skillinfo::m_skillinfo[5].m_skill_name = "超级冰冻";
    skillinfo::m_skillinfo[5].m_skill_type = 2;
    skillinfo::m_skillinfo[5].m_target_type = 2;
    skillinfo::m_skillinfo[5].m_attack_times = 1;
    skillinfo::m_skillinfo[5].m_damage_value1 = 0.4;
    skillinfo::m_skillinfo[5].m_damage_value2 = 0;
    skillinfo::m_skillinfo[5].m_attack_eff = ":/skill/skill3_0.gif";
    skillinfo::m_skillinfo[5].m_attack_posfix = QPoint(65,105);
    skillinfo::m_skillinfo[5].m_be_attack_eff = ":/skill/skill3_1.gif";
    skillinfo::m_skillinfo[5].m_be_attack_posfix = QPoint(70,180);
    //单疗
    skillinfo::m_skillinfo[6].m_skill_name = "治疗魔法";
    skillinfo::m_skillinfo[6].m_skill_type = 2;
    skillinfo::m_skillinfo[6].m_target_type = 3;
    skillinfo::m_skillinfo[6].m_attack_times = 1;
    skillinfo::m_skillinfo[6].m_damage_value1 = 3;
    skillinfo::m_skillinfo[6].m_damage_value2 = 0;
    skillinfo::m_skillinfo[6].m_attack_eff = ":/skill/skill3_0.gif";
    skillinfo::m_skillinfo[6].m_attack_posfix = QPoint(65,105);
    skillinfo::m_skillinfo[6].m_be_attack_eff = ":/skill/skill6_1.gif";
    skillinfo::m_skillinfo[6].m_be_attack_posfix = QPoint(35,140);
    //强疗
    skillinfo::m_skillinfo[7].m_skill_name = "强力治疗";
    skillinfo::m_skillinfo[7].m_skill_type = 2;
    skillinfo::m_skillinfo[7].m_target_type = 4;
    skillinfo::m_skillinfo[7].m_attack_times = 1;
    skillinfo::m_skillinfo[7].m_damage_value1 = 0.8;
    skillinfo::m_skillinfo[7].m_damage_value2 = 0;
    skillinfo::m_skillinfo[7].m_attack_eff = ":/skill/skill3_0.gif";
    skillinfo::m_skillinfo[7].m_attack_posfix = QPoint(65,105);
    skillinfo::m_skillinfo[7].m_be_attack_eff = ":/skill/skill6_1.gif";
    skillinfo::m_skillinfo[7].m_be_attack_posfix = QPoint(35,140);
    //群疗
    skillinfo::m_skillinfo[8].m_skill_name = "超级治疗";
    skillinfo::m_skillinfo[8].m_skill_type = 2;
    skillinfo::m_skillinfo[8].m_target_type = 5;
    skillinfo::m_skillinfo[8].m_attack_times = 1;
    skillinfo::m_skillinfo[8].m_damage_value1 = 0.5;
    skillinfo::m_skillinfo[8].m_damage_value2 = 0;
    skillinfo::m_skillinfo[8].m_attack_eff = ":/skill/skill3_0.gif";
    skillinfo::m_skillinfo[8].m_attack_posfix = QPoint(65,105);
    skillinfo::m_skillinfo[8].m_be_attack_eff = ":/skill/skill6_1.gif";
    skillinfo::m_skillinfo[8].m_be_attack_posfix = QPoint(35,140);
    //复活
    skillinfo::m_skillinfo[9].m_skill_name = "气绝回生";
    skillinfo::m_skillinfo[9].m_skill_type = 2;
    skillinfo::m_skillinfo[9].m_target_type = 6;
    skillinfo::m_skillinfo[9].m_attack_times = 1;
    skillinfo::m_skillinfo[9].m_damage_value1 = 1.5;
    skillinfo::m_skillinfo[9].m_damage_value2 = 0;
    skillinfo::m_skillinfo[9].m_attack_eff = ":/skill/skill3_0.gif";
    skillinfo::m_skillinfo[9].m_attack_posfix = QPoint(65,105);
    skillinfo::m_skillinfo[9].m_be_attack_eff = ":/skill/skill9_1.gif";
    skillinfo::m_skillinfo[9].m_be_attack_posfix = QPoint(20,70);

    {
    int monster_id[10] = {211,212,212,212,212,213,213,213,213,213};
    for(int i=0;i<10;i++){
        monstergroupinfo::m_monstergroupinfo[1].m_monster_id[i] = monster_id[i];
    }
    }
}
