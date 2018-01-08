#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainwindow::init();
    mainwindow w;
    w.show();

    return a.exec();
}
