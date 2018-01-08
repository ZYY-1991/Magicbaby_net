#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QTime>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QMovie>
#include <QRect>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMessageBox>
#include "hero.h"
#include "monster.h"
#include "battle.h"
#include "tips.h"
#include "confirmbox.h"

#define J 0x4a


class mainwindow : public QDialog
{
    Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = 0);
    ~mainwindow();
    static void init(void);

private:
    void timerEvent(QTimerEvent *);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void mouseMoveEvent(QMouseEvent *);

private slots:
    void onerror(QAbstractSocket::SocketError);
    void onconnect(void);
    void onreadyread(void);
    void team(void);
    void invate(void);
    void confirm_invate(void);

private:
    QTcpSocket tcpsocket;
    int m_flushtime;
    hero m_hero;
    monster m_monster;
    battle m_battle;
    int m_window;
    QPushButton m_team;
    QPushButton m_invate;
    QPushButton m_look;
    tips m_tips;
    confirmbox m_confirmbox;
    QVector <hero *> m_other_hero;
};

#endif // MAINWINDOW_H
