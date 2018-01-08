#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QDialog>
#include <QPair>
#include <QTcpServer>
#include <QTcpSocket>
#include "heromsg.h"
#include "battlemsg.h"
#include "battle.h"

namespace Ui {
class serverdialog;
}

class serverdialog : public QDialog
{
    Q_OBJECT

public:
    explicit serverdialog(QWidget *parent = 0);
    ~serverdialog();
    static void init(void);
    void ondisconnect(void);
    void onbattledisconnect(void);

private:
    void timerEvent(QTimerEvent *);

private slots:
    void on_pushButton_clicked();
    void onnewconnection(void);
    void onreadyread(void);
    void onbattleconnection(void);
    void onbattlereadyread(void);


private:
    int m_uid;
    int m_battle_id;
    int m_flushtime;
    heromsg m_monster;
    Ui::serverdialog *ui;
    QTcpServer tcpserver;
    QTcpServer tcpbattleserver;
    QVector <QPair<QTcpSocket *,heromsg *> > tcpclientlist;
    QVector <QPair<QTcpSocket *,battlemsg *> > tcpbattleclientlist;
    QVector <battle *> battlelist;
};

#endif // SERVERDIALOG_H
