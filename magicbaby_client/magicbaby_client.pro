#-------------------------------------------------
#
# Project created by QtCreator 2017-12-17T12:39:13
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = magicbaby
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hero.cpp \
    heroinfo.cpp \
    monster.cpp \
    battle.cpp \
    battleactor.cpp \
    skillinfo.cpp \
    monstergroupinfo.cpp \
    pet.cpp \
    heromsg.cpp \
    battlemsg.cpp \
    battledamagemsg.cpp \
    tips.cpp \
    confirmbox.cpp

HEADERS  += mainwindow.h \
    hero.h \
    heroinfo.h \
    monster.h \
    battle.h \
    battleactor.h \
    skillinfo.h \
    monstergroupinfo.h \
    pet.h \
    heromsg.h \
    battlemsg.h \
    battledamagemsg.h \
    tips.h \
    confirmbox.h

FORMS    +=

RESOURCES += \
    hero.qrc

DISTFILES +=
