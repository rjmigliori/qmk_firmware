#include "mainwindow.h"
#include "communication.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Communication comm;
    MainWindow w(comm);

    w.show();

    return a.exec();
}
