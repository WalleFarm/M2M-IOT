#include <QCoreApplication>
#include "MainInterface.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    MainInterface man_interface;
    return a.exec();
}
