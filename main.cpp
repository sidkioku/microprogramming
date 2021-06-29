#include "cpu.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CPU w;   
    w.show();
    return app.exec();
}
