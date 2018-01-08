#-------------------------------------------------
#
# Project created by QtCreator 2017-12-21T10:52:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = magicbaby_server
TEMPLATE = app


SOURCES += main.cpp\
        serverdialog.cpp \
    heromsg.cpp \
    pet.cpp \
    battlemsg.cpp \
    battle.cpp \
    battleactor.cpp \
    monstergroupinfo.cpp \
    heroinfo.cpp \
    skillinfo.cpp \
    battledamagemsg.cpp

HEADERS  += serverdialog.h \
    heromsg.h \
    pet.h \
    battlemsg.h \
    battle.h \
    battleactor.h \
    monstergroupinfo.h \
    heroinfo.h \
    skillinfo.h \
    battledamagemsg.h

FORMS    += serverdialog.ui
