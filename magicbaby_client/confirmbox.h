#ifndef CONFIRMBOX_H
#define CONFIRMBOX_H

#include <QObject>
#include <QPainter>
#include <QPushButton>

class confirmbox : public QObject
{
    Q_OBJECT
public:
    explicit confirmbox(QObject *parent = 0);
    ~confirmbox();
    void display(QPainter &painter);

private slots:
    void cancel(void);

public:
    int m_target;
    bool m_show;
    QString m_msg;
    QPushButton m_confirm;
    QPushButton m_cancel;
};

#endif // CONFIRMBOX_H
