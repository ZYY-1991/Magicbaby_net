/********************************************************************************
** Form generated from reading UI file 'serverdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERDIALOG_H
#define UI_SERVERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_serverdialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *mapportEdit;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *battleportEdit;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *serverdialog)
    {
        if (serverdialog->objectName().isEmpty())
            serverdialog->setObjectName(QStringLiteral("serverdialog"));
        serverdialog->resize(439, 365);
        QFont font;
        font.setPointSize(20);
        serverdialog->setFont(font);
        verticalLayout = new QVBoxLayout(serverdialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(serverdialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        mapportEdit = new QLineEdit(serverdialog);
        mapportEdit->setObjectName(QStringLiteral("mapportEdit"));

        horizontalLayout->addWidget(mapportEdit);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(serverdialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);

        battleportEdit = new QLineEdit(serverdialog);
        battleportEdit->setObjectName(QStringLiteral("battleportEdit"));

        horizontalLayout_3->addWidget(battleportEdit);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton = new QPushButton(serverdialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_2->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(serverdialog);

        QMetaObject::connectSlotsByName(serverdialog);
    } // setupUi

    void retranslateUi(QDialog *serverdialog)
    {
        serverdialog->setWindowTitle(QApplication::translate("serverdialog", "serverdialog", 0));
        label->setText(QApplication::translate("serverdialog", "\345\234\260\345\233\276\347\253\257\345\217\243\357\274\232", 0));
        mapportEdit->setText(QApplication::translate("serverdialog", "8888", 0));
        label_2->setText(QApplication::translate("serverdialog", "\346\210\230\346\226\227\347\253\257\345\217\243\357\274\232", 0));
        battleportEdit->setText(QApplication::translate("serverdialog", "6666", 0));
        pushButton->setText(QApplication::translate("serverdialog", "\345\274\200\345\220\257\346\234\215\345\212\241\345\231\250", 0));
    } // retranslateUi

};

namespace Ui {
    class serverdialog: public Ui_serverdialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERDIALOG_H
