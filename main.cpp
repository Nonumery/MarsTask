#include "marsdrilling.h"

#include <QApplication>
#include <QtWidgets>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MarsDrilling w;
    w.show();
    return a.exec();
}
