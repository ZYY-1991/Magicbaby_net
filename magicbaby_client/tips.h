#ifndef TIPS_H
#define TIPS_H

#include <QObject>
#include <QPainter>
#include <QString>
#include <QTimerEvent>

class tips : public QObject
{
    Q_OBJECT
public:
    explicit tips(QObject *parent = 0);
    ~tips();
    void timerEvent(QTimerEvent *);
    void display(QPainter &painter);
    void send_tips(QString str);

public:
    QString m_tips;
    int m_time;
    bool m_show;
};

#endif // TIPS_H
